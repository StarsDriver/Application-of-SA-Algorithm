//8Ƭ��̳���ȷֲ�

#define FLOWER_AREA 0.5 //������ֲ��ռ�����
#define FLOWER_COUNT 12
#define MAX_WIDTH 5
#define MIN_WIDTH 3
#define MAX_LENGTH 60
#define MIN_LENGTH 10
#define MAX_AREA 2200 //��·��ռ������
#define MIN_AREA 1900 //��·��ռ��С���

//ģ���˻����
#define eps 1e-9 //����
#define delta 0.98 //����ϵ��

#include<iostream>
#include<vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
using namespace std;

class Arc  //�ڽӱ��е���һ���ڵ�
{
public:
	int adjvex;  //��һ�ڵ����ڽӱ��е�����
	float width,length; //��·�ĳ��ȣ����
	float new_width,new_length; //�µĿ�ȣ��µĳ��ȣ��Ŷ�ʱ��
	float best_width,best_length;//��¼��õ����
	float flow;//�������ڼ��������ʱʹ��
	float maxWidth; //��·����Լ��
	float minWidth;
	Arc* nextarc;  //��һ�ڵ�
	
	Arc(int adjvex)
	{
		this->adjvex = adjvex;
		nextarc = NULL;
		maxWidth = 5;minWidth = 3;
		width = 4;
		flow = width;
		length = 1000;
	}
};
class FlowerBed  //��ֲ���ڵ�
{
public:
	int area;  //��ֲ��ռ�����
	Arc* firstarc;
	
	FlowerBed()
	{
		firstarc = NULL;
	}
	
};
class ALGraph //ͼ���ڽӱ�
{
public:
	FlowerBed vertices[FLOWER_COUNT+2];//��������,������ڳ��ڶ���
	int vexnum; // ��������
	
	ALGraph()
	{
		vexnum = FLOWER_COUNT+2;
	}
};

void Link(ALGraph &graph,int i,int j,float length) //�����ڽӱ�ı�,ͼ�������㡢·��
{
	Arc* p = new Arc(j);
	p->length = length;
	p->nextarc = graph.vertices[i].firstarc;
	graph.vertices[i].firstarc = p;
	//����ͼ����������
	Arc* q = new Arc(i);
	q->length = length;
	q->nextarc = graph.vertices[j].firstarc;
	graph.vertices[j].firstarc = q;
}
//���ڽӱ����ʽ�������ͼ
void PrintAG(ALGraph G)
{	
	Arc *p;
	//printf("\n����ͼG���ڽӱ���ʽ��");
	for(int i = 0; i < G.vexnum; i++)
	{	printf("\n %d",i);
		if( G.vertices[i].firstarc )
		{	printf("-->");
			for(p = G.vertices[i].firstarc; p; p = p->nextarc)
			{	printf("%d %.2f %.2f",p->adjvex,p->length,p->width);
				if(p->nextarc) printf("-->");
			}
		}
	}
	printf("\n");
}
void dfs(ALGraph graph,int start,int end,vector<int> &path,vector<vector<int> > &result,vector<int> &visited)//����Ѱ���������������·����dfs
{
	visited[start] = 1;
	path.push_back(start);
	if(start==end)
	{
		result.push_back(path);
	}else
	{
		Arc* p = graph.vertices[start].firstarc;
		while(p)
		{
			if(visited[p->adjvex]==0)
			{
				dfs(graph,p->adjvex,end,path,result,visited);
			}
			p = p->nextarc;
		}
	}
	path.pop_back();
	visited[start]=0;
}

void findAllPath(ALGraph graph,int start,int end,vector<vector<int> > &result)
{
	vector<int> visited(FLOWER_COUNT+2);
	for(int i=0;i<visited.size();i++)
	{
		visited[i] = 0;
	}
	vector<int> path;
	dfs(graph,start,end,path,result,visited);
}


void cal_val(ALGraph graph,vector<vector<int> > allPath,vector<float> &val,vector<float> &way)
{
	for(int i=0;i<val.size();i++)
	{
		val[i]=0;way[i]=0;
	}
	for(int i=0;i<allPath.size();i++)
	{
		for(int j=0;j<allPath[i].size();j++)
		{
			val[i]+=graph.vertices[allPath[i][j]].area;
			if(j>0)
			{
				Arc* p = graph.vertices[allPath[i][j-1]].firstarc;
				while(p)
				{
					if(p->adjvex==allPath[i][j])
					{
						way[i]+=p->new_length;
						break;
					}
					p = p->nextarc;
				}
			}
		}
	}
}
void cal_area_sum(ALGraph graph,vector<vector<int> > allPath,vector<float> &result)
{
	for(int i=0;i<result.size();i++)
	{
		result[i]=0;
	}
	for(int i=0;i<allPath.size();i++)
	{
		for(int j=0;j<allPath[i].size();j++)
		{
			if(j>0)
			{
				Arc* p = graph.vertices[allPath[i][j-1]].firstarc;
				while(p)
				{
					if(p->adjvex==allPath[i][j])
					{
						result[i]+=(p->new_length)*(p->new_width);
						break;
					}
					p = p->nextarc;
				}
			}
		}
	}
}

int found =0;
void dfs1(ALGraph graph,int start,int end,vector<int> &visited,vector<int> &result)  //ֻ��һ��·��dfs
{
	visited[start] = 1;
	result.push_back(start);
	
	Arc* p = graph.vertices[start].firstarc;
	while(p!=NULL&&found==0)
	{
		if(p->adjvex==end&&abs(p->flow)>1e-6)
		{
			result.push_back(p->adjvex);
			found=1;
		}else if(visited[p->adjvex]==0&&p->flow!=0)
		{
			dfs1(graph,p->adjvex,end,visited,result);
		}
		p = p->nextarc;
	}
	if(found==0)
	{
		result.pop_back();
	}
}
void findAWay(ALGraph graph,int start,int end,vector<int> &result)
{
	vector<int> visited(FLOWER_COUNT+2,0);
	dfs1(graph,start,end,visited,result);
}
//��������ֵ
void reset_flows(ALGraph graph)
{
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p =graph.vertices[i].firstarc;
		while(p)
		{
			p->flow = p->new_width;
			p = p->nextarc;
		}
	}
}
void update(ALGraph graph)
{
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p =graph.vertices[i].firstarc;
		while(p)
		{
			p->width = p->new_width;
			p->length = p->new_length;
			p = p->nextarc;
		}
	}
}
void re_update(ALGraph graph)//������£��տ�ʼ�õ�
{
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p =graph.vertices[i].firstarc;
		while(p)
		{
			p->new_width = p->width;
			p->new_length = p->length;
			p = p->nextarc;
		}
	}
}
void record_best(ALGraph graph)
{
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p =graph.vertices[i].firstarc;
		while(p)
		{
			p->best_width = p->new_width;
			p->best_length = p->new_length;
			p = p->nextarc;
		}
	}
}
void reset_flows_best(ALGraph &graph)
{
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p =graph.vertices[i].firstarc;
		while(p)
		{
			p->flow = p->best_width;
			p = p->nextarc;
		}
	}
}
float maxFlow(ALGraph graph)  //ford fulk������������������ͼ�������������
{
	reset_flows(graph);
	vector<int> onePath;
	float min;
	found = 0;
	findAWay(graph,0,FLOWER_COUNT+1,onePath);//�ҵ�һ������·
	
	while(onePath.size()!=0)
	{
		//����С������
		min = 1000;
		for(int i=0;i<onePath.size();i++)
		{
			if(i>0)
			{
				Arc* p = graph.vertices[onePath[i-1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						min = p->flow<min?p->flow:min;
					}
					p=p->nextarc;
				}
			}
		}
		//����߼�ȥ�������
		for(int i=0;i<onePath.size();i++)
		{
			if(i>0)
			{
				Arc* p = graph.vertices[onePath[i-1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						p->flow-=min;
						break;
					}
					p=p->nextarc;
				}
			}
		}
		//����߼����������
		for(int i=onePath.size()-1;i>=0;i--)
		{
			if(i<onePath.size()-1)
			{
				Arc* p = graph.vertices[onePath[i+1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						p->flow+=min;
						break;
					}
					p=p->nextarc;
				}
			}
		}
		onePath.clear();
		found = 0;
		findAWay(graph,0,FLOWER_COUNT+1,onePath);//������һ��·����
	}
	float maxFlow=0;
	//���������
	Arc* q = graph.vertices[0].firstarc;
	int i=0;
	while(q)
	{
		maxFlow += q->new_width-q->flow;
		i++;
		q = q->nextarc;
	}
	return maxFlow;
	//cout<<maxFlow;
}


//����Ŷ�ʱ�ǵ����Ʒ�Χ
void distrube(ALGraph &graph)  //Ϊͼ��Ȩֵ�������Ŷ���������״̬
{
	//Ϊ�ߵ�Ȩֵ����Ŷ�
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p = graph.vertices[i].firstarc;
		while(p)
		{
			if(p->adjvex>i)//����ͼ�����ڽӱ�������ֵ��ʾͬһ�ߣ�ֻ�����Ŷ�һ��
			{
				double rand1 = -1 + (double)rand() / RAND_MAX * 2;
				float new_length=p->length+rand1;
				double rand2 = -0.09 + (double)rand() / RAND_MAX * 0.2;/////////////////////////////////////////////////////////////////////////////////////
				float new_width = p->width + rand2;
				if(new_width>MAX_WIDTH)
				{
					new_width = MAX_WIDTH;
					
				}else if(new_width<MIN_WIDTH)
				{
					new_width = MIN_WIDTH;
				}
				p->new_width = new_width;
				
				if(new_length>MAX_LENGTH)
				{
					new_length = MAX_LENGTH;
					
				}else if(new_length<MIN_LENGTH)
				{
					new_length = MIN_LENGTH;
				}
				p->new_length = new_length;
				//��һ��Ҳ����һ��
				Arc* q = graph.vertices[p->adjvex].firstarc;
				while(q)
				{
					if(q->adjvex ==i)
					{
						q->new_length  = new_length;
						q->new_width = new_width;
						break;
					}
					q = q->nextarc;
				}
			}
			p = p->nextarc;
			
		}
	}
}
float cal_area(ALGraph graph)  //����ȫ����·��ռ���
{
	float area = 0;
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p = graph.vertices[i].firstarc;
		while(p)
		{
			if(p->adjvex>i)
			{
				area += (p->new_width)*(p->new_length);
			}
			p = p->nextarc;
		}
	}
	return area;
}
float cal_cur_area(ALGraph graph)  //���㵱ǰȫ����·��ռ���
{
	float area = 0;
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p = graph.vertices[i].firstarc;
		while(p)
		{
			if(p->adjvex>i)
			{
				area += (p->width)*(p->length);
			}
			p = p->nextarc;
		}
	}
	return area;
}
float cal_best_area(ALGraph graph)  //�������״̬ȫ����·��ռ���
{
	float area = 0;
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p = graph.vertices[i].firstarc;
		while(p)
		{
			if(p->adjvex>i)
			{
				area += (p->best_width)*(p->best_length);
			}
			p = p->nextarc;
		}
	}
	return area;
}
void cal_bestflow(ALGraph &graph)//��������������ÿ���ߵ�����
{
	reset_flows_best(graph);
	vector<int> onePath;
	float min;
	found = 0;
	findAWay(graph,0,FLOWER_COUNT+1,onePath);//�ҵ�һ������·
	
	while(onePath.size()!=0)
	{
		//����С������
		min = 1000;
		for(int i=0;i<onePath.size();i++)
		{
			if(i>0)
			{
				Arc* p = graph.vertices[onePath[i-1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						min = p->flow<min?p->flow:min;
					}
					p=p->nextarc;
				}
			}
		}
		//����߼�ȥ�������
		for(int i=0;i<onePath.size();i++)
		{
			if(i>0)
			{
				Arc* p = graph.vertices[onePath[i-1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						p->flow-=min;
						break;
					}
					p=p->nextarc;
				}
			}
		}
		//����߼����������
		for(int i=onePath.size()-1;i>=0;i--)
		{
			if(i<onePath.size()-1)
			{
				Arc* p = graph.vertices[onePath[i+1]].firstarc;
				while(p)
				{
					if(p->adjvex==onePath[i])
					{
						p->flow+=min;
						break;
					}
					p=p->nextarc;
				}
			}
		}
		onePath.clear();
		found = 0;
		findAWay(graph,0,FLOWER_COUNT+1,onePath);//������һ��·����
	}
	float maxFlow=0;
	//���������
	Arc* q = graph.vertices[0].firstarc;
	int i=0;
	while(q)
	{
		maxFlow += q->best_width-q->flow;
		i++;
		q = q->nextarc;
	}
	cout<<"max_flow:  "<<maxFlow<<endl;
	
}


void record_result(ALGraph G,float best_val,float best_aval, float best_flow, float area)//�����д���ı��ļ�
{
	ofstream ofs;
	ofs.open("record.txt",ios::out);
	
	Arc *p;
	//printf("\n����ͼG���ڽӱ���ʽ��");
	for(int i = 0; i < G.vexnum; i++)
	{	ofs<<"\n"<<i;
		if( G.vertices[i].firstarc )
		{	ofs<<"-->";
			for(p = G.vertices[i].firstarc; p; p = p->nextarc)
			{	
				ofs<<p->adjvex<<"  "<<p->best_length<<"  "<<p->best_width<<"  "<<p->best_width-p->flow;
				if(p->nextarc) ofs<<"-->";
			}
		}
	}
	ofs<<endl;
	ofs<<"best_val: "<<best_val<<endl;
	ofs<<"best_aval: "<<best_aval<<endl;
	ofs<<"best_flow: "<<best_flow<<endl;
	ofs<<"final_area: "<<area<<endl;
	
	ofs.close();
}





//�ȹ�����ȫͼ�˻�һ�Σ�������ݽ����ȥ�����˻�һ��
int main()
{
 	ALGraph graph;
 	vector<vector<int> > allPath;//�������������·��
 	//��ʼ�����
 	graph.vertices[0].area=0;//��ڳ������Ϊ0
 	graph.vertices[FLOWER_COUNT+1].area=0;
	for(int i=1;i<=12;i++)
	{
		graph.vertices[i].area = 833;
	}
	//��ʼ���������
	//�����ڽӱ�
	Link(graph,0,1,12.5);Link(graph,0,2,12.5);Link(graph,0,3,12.5);
	Link(graph,1,2,25);Link(graph,1,4,25);
	Link(graph,2,3,25);Link(graph,2,5,25);
	Link(graph,3,6,25);
	Link(graph,4,5,25);Link(graph,4,7,25);
	Link(graph,5,6,25);Link(graph,5,8,25);
	Link(graph,6,9,25);
	Link(graph,7,8,25);Link(graph,7,10,25);
	Link(graph,8,9,25);Link(graph,8,11,25);
	Link(graph,9,12,25);
	Link(graph,10,11,25);Link(graph,11,12,25);
	Link(graph,10,13,12.5);Link(graph,11,13,12.5);Link(graph,12,13,12.5);
	PrintAG(graph);
	//�������
	//Ѱ����ڵ����ڵ�����·��
	findAllPath(graph,0,FLOWER_COUNT+1,allPath);
	cout<<"size:"<<allPath.size()<<endl;
/*	
	for(auto i:allPath)
	{
		for(auto j:i)
		{
			cout<<j<<"  ";
		}
		cout<<"\n";
	}
	//������ָ�����ߵĽ��
	vector<int> count_pro(FLOWER_COUNT+2,0);
	for(auto i:allPath)
	{
		for(auto j:i)
		{
			count_pro[j]++;
		}
	}
	for(int i=0;i<count_pro.size();i++)
	{
		cout<<i<<":  "<<count_pro[i]<<endl;
	}
*/
	//������� 

	//Ѱ�����

///////////////////////////////////////////////SA
 
	re_update(graph);
	float cur_val=0,best_val=0,new_val=0;
	float cur_flow=0,best_flow=0,new_flow=0;
	float cur_aval=0,best_aval=0,new_aval=0;
	float area;
	int path_num = allPath.size();
	vector<float> val(path_num); //�����ľ����ֵ��
	vector<float> area_sum(path_num);//�߹��������
	vector<float> way(path_num); //�߹���·�̺�
	cal_val(graph,allPath,val,way);
	cal_area_sum(graph,allPath,area_sum);
	for(int i=0;i<val.size();i++)
	{
		cur_val+=val[i]/way[i];

		cur_aval+=val[i]/area_sum[i];

	}
	cur_val = cur_val/path_num;
	cur_aval = cur_val/path_num;
	best_val = cur_val;
	best_aval = cur_aval;
	cur_flow = maxFlow(graph);best_flow = cur_flow;
	cout<<endl<<cur_val<<endl;
	cout<<cur_aval<<endl;
	cout<<cur_flow<<endl;
	
	double T =1000;//��ʼ�¶�
//	while(T>eps)
	{
		//�Ŷ��õ���ͼ
		distrube(graph);
		//�����µĹ��ͼ�ֵ
		cal_val(graph,allPath,val,way);
		cal_area_sum(graph,allPath,area_sum);
		new_val = 0;new_aval = 0;
		for(int i=0;i<val.size();i++)
		{
			new_val+=val[i]/way[i];
			new_aval+=val[i]/area_sum[i];
		}
		new_val = new_val/path_num;
		new_aval = new_aval/path_num;
	
		//�����µ��������
		new_flow = maxFlow(graph);
		//�����µ����
		area = cal_area(graph);
		cout<<endl<<area<<endl;
		if(new_val>cur_val&&new_flow>cur_flow&&new_aval>cur_aval&&area>MIN_AREA&&area<MAX_AREA)//���ŵ�״̬
		{
			update(graph);
			cur_val = new_val;
			cur_aval = new_aval;
			cur_flow = new_flow;
			if(new_val>best_val&&new_flow>best_flow&&new_aval>best_aval)//��¼����״̬
			{
				best_flow = new_flow;
				best_val = new_val;
				best_aval = new_aval;
				record_best(graph);
			}
		}else if(area>MIN_AREA&&area<MAX_AREA)//�и��ʽ���,�����Ϊǿ��Լ��
		{
			//ͨ�����Լ������
			//exp(-delta / T) * RAND_MAX > rand()
			vector<float> delta_all;
			if(new_val<cur_val)
			{
				delta_all.push_back((cur_val-new_val)/cur_val);
			}
			if(new_flow<cur_flow)
			{
				delta_all.push_back((cur_flow-new_flow)/cur_flow);
			}
			if(new_aval<cur_aval)
			{
				delta_all.push_back(((cur_aval-new_aval))/cur_aval);
			}
			float max_delta = delta_all[0];
			//�������
			for(int k =0;k<delta_all.size();k++)
			{
				if(delta_all[k]>max_delta)
				{
					max_delta = delta_all[k];
				}
			}
			if(exp((-delta*10)/T)*RAND_MAX>rand())//���¸���������¶ȶ��й�
			{
				update(graph);
				cur_val = new_val;
				cur_aval = new_aval;
				cur_flow = new_flow;
			}
			
		}
		PrintAG(graph);
		cout<<endl<<cur_val<<endl;
		cout<<cur_aval<<endl;
		cout<<cur_flow<<endl;
		
		//������ǰ���¼���һ��
		area = cal_cur_area(graph);
		cout<<area<<endl;
		
		T *= delta; //����
	
	}
	area = cal_best_area(graph);
	cout<<best_val<<"  "<<best_aval<<"  "<<best_flow<<"  "<<area<<" "<<endl;
	cal_bestflow(graph);//����flow
	record_result(graph,best_val,best_aval,best_flow,area);

//SA���

	
}
