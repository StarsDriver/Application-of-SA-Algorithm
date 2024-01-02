#include<iostream>
#include<vector>
#include<fstream>
using namespace std;

int main()
{
	vector<float> flows(13);//1,12表示结点序号1-12可能承受最大流量
	flows[1]=14.86;
	flows[2]=19.73;
	flows[3]=14.81;
	flows[4]=14.39;
	flows[5]=19.52;
	flows[6]=15;
	flows[7]=13.82;
	flows[8]=18.85;
	flows[9]=15;
	flows[10]=14.52;
	flows[11]=19.25;
	flows[12]=14.85;
	vector<float> flows2 = flows;
	for(int i=0;i<flows.size();i++)
	{
		flows[i] = flows[i]/19.73;
	}
	
	vector<float> num(13);
	num[1]=189;
	num[2]=247;
	num[3]=189;
	num[4]=199;
	num[5]=253;
	num[6]=199;
	num[7]=199;
	num[8]=253;
	num[9]=199;
	num[10]=189;
	num[11]=247;
	num[12]=189;
	vector<float> num2 = num;
	for(int i=0;i<num.size();i++)
	{
		num[i] = num[i]/253;
	}
	
	
	vector<float> result(13);
	for(int i=0;i<num.size();i++)
	{
		result[i]=flows[i]*num[i];
	}
	
	for(int i=0;i<result.size();i++)
	{
		cout<<result[i]<<endl;
	}
	
	ofstream ofs;
	ofs.open("record.txt",ios::out);
	for(int i=1;i<num.size();i++)
	{
		ofs<<"\t"<<num2[i]<<"\t"<<num[i]<<"\t"<<flows2[i]<<"\t"<<flows[i]<<"\t"<<result[i]<<endl;
	}
	
	
	
	
}