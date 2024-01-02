#define FLOWER_AREA 0.5 //花卉种植区占地面积
#define FLOWER_COUNT 7
#define MAX_WIDTH 5
#define MIN_WIDTH 3
#define MAX_LENGTH 60
#define MIN_LENGTH 20
#define MAX_AREA 2100 //道路所占最大面积
#define MIN_AREA 1900 //道路所占最小面积

//模拟退火参数
#define eps 1e-9 //精度
#define delta 0.99 //降温系数

#include<iostream>
#include<vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
using namespace std;

class Arc  //邻接表中的下一个节点
{
public:
	int adjvex;  //下一节点在邻接表中的索引
	float width,length; //道路的长度，宽度
	float new_width,new_length; //新的宽度，新的长度，扰动时用
	float best_width,best_length;//记录最好的情况
	float flow;//流量，在计算最大流时使用
	float maxWidth; //道路长宽约束
	float minWidth;
	Arc* nextarc;  //下一节点
	
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
class FlowerBed  //种植区节点
{
public:
	int area;  //种植区占地面积
	Arc* firstarc;
	
	FlowerBed()
	{
		firstarc = NULL;
	}
	
};
class ALGraph //图的邻接表
{
public:
	FlowerBed vertices[FLOWER_COUNT+2];//顶点数组,加入入口出口顶点
	int vexnum; // 顶点数量
	
	ALGraph()
	{
		vexnum = FLOWER_COUNT+2;
	}
};

void Link(ALGraph &graph,int i,int j,float length) //创建邻接表的边,图、两顶点、路长
{
	Arc* p = new Arc(j);
	p->length = length;
	p->nextarc = graph.vertices[i].firstarc;
	graph.vertices[i].firstarc = p;
	//无向图，创建两次
	Arc* q = new Arc(i);
	q->length = length;
	q->nextarc = graph.vertices[j].firstarc;
	graph.vertices[j].firstarc = q;
}
//按邻接表的形式输出无向图
void PrintAG(ALGraph G)
{	
	Arc *p;
	//printf("\n无向图G的邻接表形式：");
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
void dfs(ALGraph graph,int start,int end,vector<int> &path,vector<vector<int>> &result,vector<int> &visited)//用于寻找两个顶点间所有路径的dfs
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

void findAllPath(ALGraph graph,int start,int end,vector<vector<int>> &result)
{
	vector<int> visited(FLOWER_COUNT+2);
	for(int i=0;i<visited.size();i++)
	{
		visited[i] = 0;
	}
	vector<int> path;
	dfs(graph,start,end,path,result,visited);
}


void cal_val(ALGraph graph,vector<vector<int>> allPath,vector<float> &val,vector<float> &way)
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
void cal_area_sum(ALGraph graph,vector<vector<int>> allPath,vector<float> &result)
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
void dfs1(ALGraph graph,int start,int end,vector<int> &visited,vector<int> &result)  //只找一条路的dfs
{
	visited[start] = 1;
	result.push_back(start);
	
	Arc* p = graph.vertices[start].firstarc;
	while(p!=NULL&&found==0)
	{
		if(p->adjvex==end&&p->flow!=0)
		{
			result.push_back(p->adjvex);
			found=1;
		}else if(visited[p->adjvex]==0&&abs(p->flow)>1e-6) ////////////////////浮点数不等于零的判断条件！
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
//重置流量值
void reset_flows(ALGraph &graph)
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
void re_update(ALGraph graph)//反向更新，刚开始用到
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
void record_best(ALGraph &graph)
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
float maxFlow(ALGraph graph)  //ford fulk求解网络最大流，无向图看作两个有向边
{
	reset_flows(graph);
	vector<int> onePath;
	float min;
	found = 0;
	findAWay(graph,0,FLOWER_COUNT+1,onePath);//找到一条增广路
	
	while(onePath.size()!=0)
	{
		//找最小的流量
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
		//正向边减去这个流量
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
		//反向边加上这个流量
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
		findAWay(graph,0,FLOWER_COUNT+1,onePath);//重新找一条路径；
	}
	float maxFlow=0;
	//计算最大流
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


//添加扰动时记得限制范围
void distrube(ALGraph &graph)  //为图各权值添加随机扰动，进入新状态
{
	//为边的权值添加扰动
	for(int i=0;i<graph.vexnum;i++)
	{
		Arc* p = graph.vertices[i].firstarc;
		while(p)
		{
			if(p->adjvex>i)//无向图，在邻接表中有两值表示同一边，只单向扰动一次
			{
				double rand1 = -1 + (double)rand() / RAND_MAX * 2;
				float new_length=p->length+rand1;
				double rand2 = -0.1 + (double)rand() / RAND_MAX * 0.2;/////////////////////////////////////////////////////////////////////////////////////
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
				//另一侧也更新一次
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
float cal_area(ALGraph graph)  //计算全部道路所占面积
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
float cal_cur_area(ALGraph graph)  //计算当前全部道路所占面积
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
float cal_best_area(ALGraph graph)  //计算最好状态全部道路所占面积
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

void record_result(ALGraph G,float best_val,float best_aval, float best_flow, float area)//将结果写入文本文件
{
	ofstream ofs;
	ofs.open("record.txt",ios::out);
	
	Arc *p;
	//printf("\n无向图G的邻接表形式：");
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

void cal_bestflow(ALGraph &graph)//计算最大流，输出每条边的流量
{
	reset_flows_best(graph);
	vector<int> onePath;
	float min;
	found = 0;
	findAWay(graph,0,FLOWER_COUNT+1,onePath);//找到一条增广路
	
	while(onePath.size()!=0)
	{
		//找最小的流量
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
		//正向边减去这个流量
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
		//反向边加上这个流量
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
		findAWay(graph,0,FLOWER_COUNT+1,onePath);//重新找一条路径；
	}
	float maxFlow=0;
	//计算最大流
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



//先构建完全图退火一次，后面根据结果舍去边再退火一次
int main()
{
 	ALGraph graph;
 	vector<vector<int>> allPath;//两个顶点间所有路径
 	//初始化面积
 	graph.vertices[0].area=0;//入口出口面积为0
 	graph.vertices[FLOWER_COUNT+1].area=0;
 	for(int i=1;i<=3;i++)
 	{
 		graph.vertices[i].area=1300;	
	}
	graph.vertices[4].area = 2200;
	for(int i=5;i<=7;i++)
	{
		graph.vertices[i].area = 1300;
	}
	//初始化面积结束
	//创建邻接表
	Link(graph,0,1,30);Link(graph,0,2,30);Link(graph,0,3,30);
	Link(graph,1,2,20);Link(graph,2,3,20);
	Link(graph,1,4,40);Link(graph,2,4,40);Link(graph,3,4,40);
	Link(graph,4,5,40);Link(graph,4,6,40);Link(graph,4,7,40);
	Link(graph,5,6,20);Link(graph,6,7,20);
	Link(graph,5,8,30);Link(graph,6,8,30);Link(graph,7,8,30);
	PrintAG(graph);
	//创建完成
	//寻找入口到出口的所有路径
	findAllPath(graph,0,FLOWER_COUNT+1,allPath);
	cout<<"size::"<<allPath.size()<<endl;
	/*	
	for(auto i:allPath)
	{
		for(auto j:i)
		{
			cout<<j<<"  ";
		}
		cout<<"\n";
	}
*/

	//寻找完成

///////////////////////////////////////////////SA
	re_update(graph);
	float cur_val=0,best_val=0,new_val=0;
	float cur_flow=0,best_flow=0,new_flow=0;
	float cur_aval=0,best_aval=0,new_aval=0;
	float area;
	int path_num = allPath.size();
	vector<float> val(path_num); //看过的景点价值和
	vector<float> area_sum(path_num);//走过的面积和
	vector<float> way(path_num); //走过的路程和
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
	
	double T =1000;//初始温度
	while(T>eps)
//	for(int k=0;k<100;k++)
	{
		//扰动得到新图
		distrube(graph);
		//计算新的观赏价值
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
	
		//计算新的最大流量
		new_flow = maxFlow(graph);
		//计算新的面积
		area = cal_area(graph);
		
		if(new_val>cur_val&&new_flow>cur_flow&&new_aval>cur_aval&&area>MIN_AREA&&area<MAX_AREA)//更优的状态
		{
			update(graph);
			cur_val = new_val;
			cur_aval = new_aval;
			cur_flow = new_flow;
			if(new_val>best_val&&new_flow>best_flow&&new_aval>best_aval)//记录最优状态
			{
				best_flow = new_flow;
				best_val = new_val;
				best_aval = new_aval;
				record_best(graph);
			}
		}else if(area>MIN_AREA&&area<MAX_AREA)//有概率接受,但面积为强制约束
		{
			//通过差距约束概率
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
			//找最大差距
			for(int k =0;k<delta_all.size();k++)
			{
				if(delta_all[k]>max_delta)
				{
					max_delta = delta_all[k];
				}
			}
			if(exp((-delta*10)/T)*RAND_MAX>rand())//更新概率与差距和温度都有关
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
		
		//输出面积前重新计算一遍
		area = cal_cur_area(graph);
		cout<<area<<endl;
		
		T *= delta; //降温
	
	}
	area = cal_best_area(graph);
	cout<<best_val<<"  "<<best_aval<<"  "<<best_flow<<"  "<<area<<" "<<endl;
	cal_bestflow(graph);//更新flow
	record_result(graph,best_val,best_aval,best_flow,area);
	cout<<endl<<RAND_MAX<<endl;

//SA完成

	
}
