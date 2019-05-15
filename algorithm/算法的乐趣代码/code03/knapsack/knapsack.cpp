// knapsack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>

typedef struct tagObject
{
    int weight;
    int price;
    int status; //0:未选中；1:已选中；2:已经不可选
}OBJECT;

typedef struct tagKnapsackProblem
{
    std::vector<OBJECT> objs;
    int totalC;
}KNAPSACK_PROBLEM;

typedef int (*SELECT_POLICY)(std::vector<OBJECT>& objs, int c);

int Choosefunc1(std::vector<OBJECT>& objs, int c)
{
    int index = -1;
    int mp = 0;
    for(int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if((objs[i].status == 0) && (objs[i].price > mp))
        {
            mp = objs[i].price;
            index = i;
        }
    }

    return index;
}

int Choosefunc2(std::vector<OBJECT>& objs, int c)
{
    int index = -1;
    int mw = 10000;
    for(int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if((objs[i].status == 0) && (objs[i].weight < mw))
        {
            mw = objs[i].weight;
            index = i;
        }
    }

    return index;
}

int Choosefunc3(std::vector<OBJECT>& objs, int c)
{
    int index = -1;
    double ms = 0.0;
    for(int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if(objs[i].status == 0)
        {
            double si = objs[i].price;
            si = si / objs[i].weight;
            if(si > ms)
            {
                ms = si;
                index = i;
            }
        }
    }

    return index;
}

void PrintResult(std::vector<OBJECT>& objs)
{
    int totalW = 0;
    int totalP = 0;
    for(int i = 0; i < static_cast<int>(objs.size()); i++)
    {
        if(objs[i].status == 1)
        {
            totalW += objs[i].weight;
            totalP += objs[i].price;
            std::cout << "object " << i + 1 << ": weight=" << objs[i].weight << 
                                               ", price=" << objs[i].price << std::endl;
        }
    }
    std::cout << "total weight : " << totalW << ", total price : " << totalP << std::endl; 
}

void GreedyAlgo(KNAPSACK_PROBLEM *problem, SELECT_POLICY spFunc)
{
    int idx;
    int ntc = 0;

    //spFunc 每次选最符合策略的那个物品，选后再检查
    while((idx = spFunc(problem->objs, problem->totalC - ntc)) != -1)
    {
        //所选物品是否满足背包承重要求？
        if((ntc + problem->objs[idx].weight) <= problem->totalC)
        {
            problem->objs[idx].status = 1;
            ntc += problem->objs[idx].weight;
        }
        else
        {
            //不能选这个物品了，做个标记后重新选
            problem->objs[idx].status = 2; 
        }
    }

    PrintResult(problem->objs);
}

const int MIN=0x80000000;
const int N=7;   //物品数量
const int V=150;  //背包容量
int f[N+1][V+1];

int Package(int *W,int *C,int N,int V)
{
 int i,j;
 memset(f,0,sizeof(f));  //初始化为0

 for(i=0;i<=N;i++)
 for(j=1;j<=V;j++)               //此步骤是解决是否恰好满足背包容量，
  f[i][j]=MIN;                //若“恰好”满足背包容量，即正好装满背包，则加上此步骤，若不需要“恰好”，则初始化为0
    
 for(i=1;i<=N;i++)
  for(j=C[i];j<=V;j++)
  {
   f[i][j]=(f[i-1][j]>f[i-1][j-C[i]]+W[i])?f[i-1][j]:(f[i-1][j-C[i]]+W[i]);
   std::cout<<"f["<<i<<"]["<<j<<"]="<<f[i][j]<<std::endl;
  }
 return f[N][V];
}

void DPAlgo()
{
 int W[8]={0,10,40,30,50,35,40,30};      //物品权重
 int C[8]={0,35,30,60,50,40,10,25};      //物品大小
 int result=Package(W,C,N,V);
 if(result>0)
 {
  std::cout<<std::endl;
  std::cout<<"the opt value:"<<result<<std::endl;
  int i=N,j=V;
  while(i)
  {
   if(f[i][j]==(f[i-1][j-C[i]]+W[i]))
   {
    std::cout<<i<<":"<<"w="<<W[i]<<",c="<<C[i]<<std::endl;
    j-=C[i];
   }
   i--;
  }
 }
 else
     std::cout<<"can not find the opt value"<<std::endl;
}

OBJECT objects[] = { {35,10,0}, {30,40,0}, {60,30,0}, {50,50,0},
                     {40,35,0}, {10,40,0}, {25,30,0} };

int main(int argc, char* argv[])
{

    KNAPSACK_PROBLEM problem;

    problem.objs.assign(objects, objects + 7);
    problem.totalC = 150;

    //GreedyAlgo(&problem, Choosefunc1);
    //GreedyAlgo(&problem, Choosefunc2);
    //GreedyAlgo(&problem, Choosefunc3);
    DPAlgo();

	return 0;
}

