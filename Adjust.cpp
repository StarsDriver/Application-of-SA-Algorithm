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

void Link(ALGraph &graph,int i,int j,float length, float width =4) //创建邻接表的边,图、两顶点、路长,,,路宽
{
	Arc* p = new Arc(j);
	p->length = length;
	p->nextarc = graph.vertices[i].firstarc;
	graph.vertices[i].firstarc = p;
	//无向图，创建两次
	Arc* q = new Arc(i);
	q->length = length;
	q->width = width;
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

void dfs(ALGraph graph,int start,int end,vector<int> &path,vector<vector<int> > &result,vector<int> &visited)//用于寻找两个顶点间所有路径的dfs
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
//求关键路径

int main()
{
	//先建图
	//第一问结果
	/*
	0-->3  21.1912  4.16937  -->2  20  4.20442  -->1  24.9887  4.95383  
1-->4  41.751  4.76053  -->2  32.1424  4.12801  -->0  24.9887  4.95383  
2-->4  38.3622  4.25797  -->3  20.6785  4.56736  -->1  32.1424  4.12801  -->0  20  4.20442  
3-->4  28.7257  4.6312  -->2  20.6785  4.56736  -->0  21.1912  4.16937  
4-->7  22.7972  4.28732  -->6  20.305  4.64486  -->5  26.9726  4.81686  -->3  28.7257  4.6312  -->2  38.3622  4.25797  -->1  41.751  4.76053  
5-->8  30.2405  4.54001  -->6  30.7041  3.80251  -->4  26.9726  4.81686  
6-->8  20  4.86352  -->7  20.1251  4.27229  -->5  30.7041  3.80251  -->4  20.305  4.64486  
7-->8  31.0125  3.98268  -->6  20.1251  4.27229  -->4  22.7972  4.28732  
8-->7  31.0125  3.98268  -->6  20  4.86352  -->5  30.2405  4.54001  
	*/
	ALGraph graph;
	
	Link(graph,0,3,20,4.16937);Link(graph,0,2,36.96,4.20442);Link(graph,0,1,20,4.95383);
	Link(graph,1,4,20.60,4.76053);Link(graph,1,2,20,4.12801);
	Link(graph,2,4,45.93,4.25797);Link(graph,2,3,30.49,4.56736);
	Link(graph,3,4,47.28,4.6312);
	Link(graph,4,7,34.24,4.28732);Link(graph,4,6,43.06,4.64486);Link(graph,4,5,20.24,4.81686);
	Link(graph,5,8,35.72,4.54001);Link(graph,5,6,20.24,3.80251);
	Link(graph,6,8,35.72,4.86352);Link(graph,6,7,20,4.27229);
	Link(graph,7,8,20.34,3.98268);
	
	/*
	Link(graph,0,1,12.82);Link(graph,0,2,10);Link(graph,0,3,10.01);
	Link(graph,1,2,16.90);Link(graph,2,3,14.69);Link(graph,1,4,22.76);
	Link(graph,2,5,10.93);Link(graph,3,6,19.10);Link(graph,4,5,20.05);Link(graph,5,6,17.39);
	Link(graph,4,5,20.05);Link(graph,5,6,17.39);
	Link(graph,4,7,27.65);Link(graph,5,8,11.25);Link(graph,6,9,17.74);
	Link(graph,7,8,10);Link(graph,8,9,10.78);Link(graph,7,10,10);Link(graph,8,11,31.05);
	Link(graph,9,12,12.15);Link(graph,10,11,23.96);Link(graph,11,12,38.81);
	Link(graph,10,13,11.26);Link(graph,11,13,17.53);Link(graph,12,13,13.92);
	*/
	//再评估
	
	vector<vector<int>> allPath;
	findAllPath(graph,0,FLOWER_COUNT+1,allPath);
	vector<float> val_col(allPath.size(),0);
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
							val_col[i]+=p->length;
							break;
						}
						p = p->nextarc;
					}
				}
			}
	}
	float max = val_col[0];
	int record = 0;
	for(int i=1;i<val_col.size();i++)
	{
		
		if(val_col[i]<max)
		{
			record = i;
			max = val_col[i];
		}
		
	}
	for(auto x:allPath[record])
	{
		cout<<"path "<<x<<"  ";
	}
	cout<<endl;
	cout<<max;
	

}