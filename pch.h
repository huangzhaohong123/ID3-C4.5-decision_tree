
//#define PCH_H
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <string.h>
#include <queue>
#include <stack>
using namespace std;

#define ROW 14
#define COL 5
#define log2 0.69314718055//这里指log以e为底的loge(2)的定义，方便后面使用换底公式

//定义决策树结点，这里运用的是兄弟-孩子双叉链表
typedef struct TNode
{
	char data[20];
	char weight[20];
	TNode* firstchild, * nextsibling;
}*tree;

//定义训练样本链表的结点
typedef struct LNode
{
	char  OutLook[20];
	char  Temperature[20];
	char  Humidity[20];
	char  Wind[20];
	char  PlayTennis[20];

	LNode* next;
}*link;

//定义属性链表的结点
typedef struct AttrNode
{
	char   attributes[15];//属性
	int	   attr_Num;//属性的个数

	AttrNode* next;
}*Attributes;

//定义属性类别
const char* OutLook_kind[3] = { "Sunny","OverCast","Rain" };
const char* Temperature_kind[3] = { "Hot","Mild","Cool" };
const char* Humidity_kind[2] = { "High","Normal" };
const char* Wind_kind[2] = { "Weak","Strong" };

//广义表表示决策树
void treelists(tree T);//不带分支
void treelists1(tree T, int& i);//带分支

//构建决策树的基本函数
void InitAttr(Attributes& attr_link, const char* Attributes_kind[], int Attr_kind[]);//构造属性链表
void InitLink(link& L, const char* Examples[][COL]);//构造训练样本链表
void PN_Num(link L, int& positve, int& negative);//计算正负样本

//ID3算法构建决策树
double Gain(int positive, int negative, const char* atrribute, link L, Attributes attr_L);//计算信息增益
void ID3(tree& T, link L, link Target_Attr, Attributes attr);//ID3算法构建决策树

//C4.5算法构建决策树
double Gain_Ratio(int positive, int negative, const char* atrribute, link L, Attributes attr_L);//计算信息增益率
void C4_5(tree& T, link L, link Target_Attr, Attributes attr);//C4.5算法构建决策树

//展示训练样本
void show(const char* Examples[][COL]);//终端展示
void show_txt(link LL, const char* Attributes_kind[], const char* Examples[][COL]);//打印有关样本数据

//测试数据
void Test(tree T, char train[4][20], char max[20], stack <char*>& a);//输出分类类别
void route(tree T, char train[4][20]);//输出测试数据在决策树中的遍历路径

//可视化决策树
void graphic1(tree T);//可视化ID3算法构建的决策树
void graphic2(tree T);//可视化C4.5算法构建的决策树

//基本函数(备用)
int TreeHeight(tree T);//求树的高度
void InOrderTraverse1(tree T);//先序遍历