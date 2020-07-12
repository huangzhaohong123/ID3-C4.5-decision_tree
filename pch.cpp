#include "pch.h"


//以广义表的形式输出树
void treelists(tree T)
{
	tree p;
	if (T == NULL)
		return;
	cout << T->data;//输出根结点
	p = T->firstchild;
	if (p)
	{
		cout << "(";//层次加'('
		treelists(p);//层次递归
		p = p->nextsibling;
		while (p)
		{
			cout << ',';//层内加','
			treelists(p);//层内递归
			p = p->nextsibling;
		}
		cout << ")";
	}

}

void treelists1(tree T,int &i)
{
	tree p;
	if (!T)
		return;
	if(i!=0)
	cout << "{" << T->weight << "}";

	i++;
	cout << T->data;

	p = T->firstchild;
	if (p)
	{
		cout << "(";
		while (p)
		{
			treelists1(p,i);
			p = p->nextsibling;
			if(p)
			cout << ',';
		}
		cout << ")";
	}
}

//建立属性链表（参数：待构建链表，属性种类，属性种类数）
void InitAttr(Attributes& attr_link, const char* Attributes_kind[], int Attr_kind[])
{
	Attributes p;
	for (int i = 0; i < COL - 1; i++)
	{
		p = new AttrNode;//结点初始化
		p->next = NULL;

		strcpy_s(p->attributes, Attributes_kind[i]);
		p->attr_Num = Attr_kind[i];

		//头插法（后面部分验证代码需要注意）
		p->next = attr_link->next;
		attr_link->next = p;
	}
}

//建立训练样本链表（参数待训练的样本，给定的样本数据）
void InitLink(link& LL, const char* Examples[][COL])
{
	link p;

	for (int i = 0; i < ROW; i++)
	{
		//结点初始化
		p = new LNode;
		p->next = NULL;

		strcpy_s(p->OutLook, Examples[i][COL - 5]);
		strcpy_s(p->Temperature, Examples[i][COL - 4]);
		strcpy_s(p->Humidity, Examples[i][COL - 3]);
		strcpy_s(p->Wind, Examples[i][COL - 2]);
		strcpy_s(p->PlayTennis, Examples[i][COL - 1]);

		//头插法
		p->next = LL->next;
		LL->next = p;
	}
}


//寻找正负样本 
void PN_Num(link L, int& positve, int& negative)
{
	positve = 0;
	negative = 0;
	link p;

	//找最终分类结果
	p = L->next;
	while (p)
	{
		if (strcmp(p->PlayTennis, "No") == 0)
			negative++;
		else if (strcmp(p->PlayTennis, "Yes") == 0)
			positve++;

		p = p->next;
	}
}


//计算信息增益（重点）
//参数：正样本数，负样本数，待计算属性，训练样本链表，属性链表
double Gain(int positive, int negative, const char* atrribute, link L, Attributes attr_L)
{
	int atrr_kinds;//每个属性的类别数
	int attr_th = 0;//第几个属性（这里是根据属性链表的顺序，由于是头插法构建，故顺序与插入顺序颠倒），仅用以验证

	Attributes p = attr_L->next;
	link q = L->next;

	//确定该属性的类别数
	while (p)
	{
		if (strcmp(p->attributes, atrribute) == 0)
		{
			atrr_kinds = p->attr_Num;//获得其属性类别数
			break;
		}
		p = p->next;
		attr_th++;
	}
	//printf("attr_th:%d,atrr_kinds:%d\n", attr_th, atrr_kinds);

	double entropy, gain = 0;

	//信息熵的计算（公式：entropy = -p1*log2(p1) - p2*log2(p2)）
	double p1 = 1.0 * positive / (positive + negative);//正样本占总样本的比例
	double p2 = 1.0 * negative / (positive + negative);//负样本占总样本的比例
	entropy = -p1 * log(p1) / log2 - p2 * log(p2) / log2;//公式计算,这里用到数学公式中换底公式的小技巧
	gain = entropy;


	//定义一个3*atrr_kinds的数组，目的为了存储该属性类别下的一系列信息，来计算条件熵
	int** kinds = new int* [3];//c++中定义二维数组的方法
	for (int j = 0; j < 3; j++)
	{
		kinds[j] = new int[atrr_kinds];
		//printf("%d\n",kinds[j]);
		//printf("kinds[%d]=%d\n",j,kinds[j]);
	}

	//初始化
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < atrr_kinds; i++)
		{
			kinds[j][i] = 0;
		}
	}

	/*初始化效果（以OutLook为例）：
			Sunny		OverCast	Rain
	总：	0			0			0
	正：	0			0			0
	负：	0			0			0

	  进行统计后效果（以OutLook为例）：
			Sunny		OverCast	Rain
	总：	5			4			5
	正：	2			4			3
	负：	3			0			2
	*/

	//Gain函数的目的是为了求某一个属性的信息增益（故需要在训练样本中找出该属性的条件熵）
	//将定义的二维数组填满信息
	while (q)
	{
		if (strcmp("OutLook", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)//计算属性类别的信息（样本占比数，正样本数，负样本数）
			{
				if (strcmp(q->OutLook, OutLook_kind[i]) == 0)
				{
					kinds[0][i]++;//计算样本占比数

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;//计算正样本数
					else
						kinds[2][i]++;//计算负样本数
				}
			}
		}
		else if (strcmp("Temperature", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Temperature, Temperature_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;
					else
						kinds[2][i]++;
				}
			}
		}
		else if (strcmp("Humidity", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Humidity, Humidity_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;// 
					else
						kinds[2][i]++;
				}
			}
		}
		else if (strcmp("Wind", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Wind, Wind_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;
					else
						kinds[2][i]++;
				}
			}
		}
		q = q->next;
	}

	//计算信息增益，定义一个atrr_kinds的数组（目的存储entropy()如上注释）
	double* gain_kind = new double[atrr_kinds];

	/*
	条件熵公式的计算（需要）：每个属性类别的正负样本及其占比，类别之间的占比
	以OotLook为例：
	entropy(S)= -2/5 * log2(2/5) - 3/5 * log2(3/5)
	entropy(O)= -4/4 * log2(4/4) - 0/4 * log2(0/4)=0
	entropy(R)= -3/5 * log2(3/5) - 2/5 * log2(2/5)
	entropy(SOR)= 5/14entropy(S) + 4/14entropy(O) + 5/14entropy(R)
	gain = entropy(信息熵) - entropy(SOR)
	*/

	//上方公式计算
	for (int j = 0; j < atrr_kinds; j++)
	{
		if (kinds[0][j] != 0 && kinds[1][j] != 0 && kinds[2][j] != 0)
		{
			p1 = 1.0 * kinds[1][j] / kinds[0][j];
			p2 = 1.0 * kinds[2][j] / kinds[0][j];

			gain_kind[j] = -p1 * log(p1) / log2 - p2 * log(p2) / log2;//换底公式
			gain = gain - (1.0 * kinds[0][j] / (positive + negative)) * gain_kind[j];
		}
		else
			gain_kind[j] = 0;//通过上方注释可得出该结论
	}
	return gain;
}


//ID3算法中需要及时清空子属性链和子训练样本链
void FreeLink(link& Link)
{
	link p, q;
	p = Link->next;
	while (p)
	{
		q = p;
		p = p->next;
		delete(q);
	}
	Link->next = NULL;
}



//通过ID3算法构建决策树
//参数：树（待构建），训练样本链表，目标链（为了表示当属性链为空时的普遍情况，以防万一），属性链
void ID3(tree& T, link L, link Target_Attr, Attributes attr)
{
	//定义p,p1是为了构建attr_chilid的辅助，max是为了找到最大增益的属性
	Attributes p, max, attr_child, p1;

	//定义q和q1是为了构建link_child的辅助
	link q, link_child, q1;

	//定义r是为了构建新的结点以构树
	//定义tree_p的目的是为了当我们构建完每一层的第一个结点后需要改变操作对象（由T到T->firstchild）
	tree r, tree_p;

	//计算总训练样本中的正负样本
	int positive = 0, negative = 0;
	PN_Num(L, positive, negative);

	//初始化两个子集合（两个子集合是构建树的关键）
	attr_child = new AttrNode;
	attr_child->next = NULL;

	link_child = new LNode;
	link_child->next = NULL;

	if (positive == 0)//全是反例
	{
		strcpy_s(T->data, "No");
	}
	else if (negative == 0)//全是正例
	{
		strcpy_s(T->data, "Yes");
	}

	p = attr->next; //属性链表
	double gain, g = 0;

	if (p)
	{
		//建立属性子链表
		while (p)
		{
			//计算所有属性中哪个属性的信息增益最大，做为决策树的根结点
			gain = Gain(positive, negative, p->attributes, L, attr);
			cout << p->attributes << "的信息增益为：" << gain << endl;
			if (gain > g)
			{
				g = gain;
				max = p;
			}
			p = p->next;
		}
		strcpy_s(T->data, max->attributes);//增加决策树的节点

		cout << "信息增益最大的属性：max->attributes = " << max->attributes << endl << endl;

		//创建属性子链表（树的每一层只需要创建一次属性子链表）
		p = attr->next;
		while (p)
		{
			if (strcmp(p->attributes, max->attributes) != 0)//属性链中不为信息最大增益的属性进行链表构建
			{
				//初始化
				p1 = new AttrNode;
				p1->next = NULL;
				strcpy_s(p1->attributes, p->attributes);
				p1->attr_Num = p->attr_Num;
				//头插法
				p1->next = attr_child->next;
				attr_child->next = p1;
			}
			p = p->next;
		}

		//由于我们已经得到信息增益最大的点，我们就需要构建其分支（即属性类别，也可理解为权重）
		//而我们构建决策树的方式是利用兄弟孩子链表进行构建，因此构建第一步需要找到所有层的第一个结点（孩子结点）
		if (strcmp("OutLook", max->attributes) == 0)
		{
			//结点初始化
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			//为结点的分支赋值（属性类别）
			strcpy_s(r->weight, OutLook_kind[0]);
			T->firstchild = r;

			//建立样本子链表（目的找第一个孩子结点）
			q = L->next;
			while (q)
			{
				//将q->OutLook为“Sunny”的数据进行链表建立
				if (strcmp(q->OutLook, OutLook_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Temperature", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Temperature_kind[0]);
			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Temperature, Temperature_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Humidity", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Humidity_kind[0]);
			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Humidity, Humidity_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Wind", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Wind_kind[0]);

			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Wind, Wind_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}

		//上述过程，分别建立了属性子链表和训练样本子链表
		int p = 0, n = 0;
		PN_Num(link_child, p, n);//在子训练样本中找正负样本
		if (p != 0 && n != 0)
		{
			//递归（建立每一层的第一个节点，T->firstchild是重点）
			ID3(T->firstchild, link_child, Target_Attr, attr_child);
			FreeLink(link_child);//因为link_child会一直不一样，所以占用的空间要及时清空
		}
		else if (p == 0)//当样本都为负样本时
		{
			strcpy_s(T->firstchild->data, "No");
			FreeLink(link_child);
		}
		else if (n == 0)//当样本都为正样本时
		{
			strcpy_s(T->firstchild->data, "Yes");
			FreeLink(link_child);
		}

		/*
		（假设）样本例子（按建立第一个结点后的效果）：
		LookOut
		   |
		Humidity — Temperature — Wind
		*/

		//建立每一层上的其他节点
		//因为我们构建决策树是利用
		tree_p = T->firstchild;//由于我们在上面的操作中已经将每层的第一个结点构建完成，所以现在的操作目标应该从每层的第一个子节点的兄弟结点来操作

		for (int i = 1; i < max->attr_Num; i++)//注意这里的下标从1开始，因为我们已经建立了每层的第一个结点
		{
			//每层的max是固定的，但是分支（weight）是不一样的，因此对应的link_child也不一样
			//需要区分出是哪一种属性
			if (strcmp("OutLook", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, OutLook_kind[i]);//这里是决策树分支的赋值
				tree_p->nextsibling = r;//对每层的兄弟结点进行操作

				q = L->next;
				while (q)
				{
					if (strcmp(q->OutLook, OutLook_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}
			else if (strcmp("Temperature", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Temperature_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Temperature, Temperature_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}

			}
			else if (strcmp("Humidity", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Humidity_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Humidity, Humidity_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}
			else if (strcmp("Wind", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Wind_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Wind, Wind_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}

			//通过正负样本和训练样本子链表，属性子链表的关系，递归建树
			int p = 0, n = 0;
			PN_Num(link_child, p, n);
			if (p != 0 && n != 0)
			{
				//这里操作对象是兄弟结点
				ID3(tree_p->nextsibling, link_child, Target_Attr, attr_child);
				FreeLink(link_child);
			}
			else if (p == 0)
			{
				strcpy_s(tree_p->nextsibling->data, "No");
				FreeLink(link_child);
			}
			else if (n == 0)
			{
				strcpy_s(tree_p->nextsibling->data, "Yes");
				FreeLink(link_child);
			}

			tree_p = tree_p->nextsibling;//建立所有的孩子结点
		}//建立决策树结束
	}
	else
	{
		q = L->next;
		strcpy_s(T->data, q->PlayTennis);
		//这个地方要赋以训练样本Example中最普遍的Target_attributes的值
	}
}

void show(const char* Examples[][COL])
{
	int i, j;
	printf("OutLook         Temperature     Humidity        Wind            PlayTennis\n");
	printf("--------------------------------------------------------------------------\n");
	for (i = 0; i < ROW; i++)
	{
		if (strcmp(Examples[i][0], "OverCast") == 0)
			printf("%s        ", Examples[i][0]);
		else
			printf("%s\t\t", Examples[i][0]);
		printf("%s\t\t%s\t\t%s\t\t%s\n", Examples[i][1], Examples[i][2], Examples[i][3], Examples[i][4]);
	}
}

//测试，输出分类类别
void Test(tree T, char train[4][20], char max[20], stack <char*>& a)
{
	tree p;
	//g++;
	//queue<char*> a;
	if (T == NULL)
		return;
	if (strcmp(T->data, max) == 0)//根结点没有分支
		p = T->firstchild;
	else
		p = T;
	//cout << T->data << endl;
	a.push(T->data);
	int j, fig = 0;
	int i = 0;
	if (p != NULL)
	{

		for (j = 0; j < 4; j++)
		{
			if (strcmp(train[j], p->weight) == 0)
				fig++;
		}
		if (fig != 0)
		{
			p = p->firstchild;
			//cout << T->data << "----->";
			Test(p, train, max, a);
		}
		else
		{
			p = p->nextsibling;
			//cout << T->data << "----->";
			Test(p, train, max, a);
		}
		//cout << T->data << endl;

	}
}

//输出决策树经过路径
void route(tree T, char train[4][20])
{
	tree p;
	if (T == NULL)
		return;
	p = T;
	int j, fig = 0;
	int i = 0;
	if (p != NULL)
	{

		for (j = 0; j < 4; j++)
		{
			if (strcmp(train[j], p->weight) == 0)
				fig++;
		}
		if (fig != 0)
		{
			if (strcmp(T->data, "Yes") == 0 || strcmp(T->data, "No") == 0)
			{
				cout << "---" << '(' << T->weight << ')' << "--->" << T->data;
				p = p->firstchild;
			}
			if (strcmp(T->data, "Yes") != 0 && strcmp(T->data, "No") != 0)
			{
				p = p->firstchild;
				cout << "---" << '(' << T->weight << ')' << "--->" << T->data;
			}
			route(p, train);
		}
		else
		{
			p = p->nextsibling;
			//cout << "---" << '(' << T->weight << ')' << "--->" << T->data;
			route(p, train);
		}
	}
}


void InOrderTraverse1(tree T)
{
	//先序遍历
	if (T)
	{
		InOrderTraverse1(T->firstchild);
		InOrderTraverse1(T->nextsibling);
		cout << T->data << ' ';
	}
}

int TreeHeight(tree T)
{
	tree p;
	int h, maxh = 0;
	if (!T)
		return 0;
	else
	{
		p = T->firstchild;
		while (p)
		{
			h = TreeHeight(p);
			if (maxh < h)
				maxh = h;
			p = p->nextsibling;
		}
		return (maxh + 1);
	}
}


double Gain_Ratio(int positive, int negative, const char* atrribute, link L, Attributes attr_L)
{
	int atrr_kinds;//每个属性的类别数
	int attr_th = 0;//第几个属性（这里是根据属性链表的顺序，由于是头插法构建，故顺序与插入顺序颠倒），仅用以验证

	Attributes p = attr_L->next;
	link q = L->next;

	//确定该属性的类别数
	while (p)
	{
		if (strcmp(p->attributes, atrribute) == 0)
		{
			atrr_kinds = p->attr_Num;
			break;
		}
		p = p->next;
		attr_th++;
	}
	//printf("attr_th:%d,atrr_kinds:%d\n", attr_th, atrr_kinds);

	double entropy, gain = 0;

	//信息熵的计算（公式：entropy = -p1*log2(p1) - p2*log2(p2)）
	double p1 = 1.0 * positive / (positive + negative);//正样本占总样本的比例
	double p2 = 1.0 * negative / (positive + negative);//负样本占总样本的比例
	entropy = -p1 * log(p1) / log2 - p2 * log(p2) / log2;//公式计算,这里用到数学公式中换底公式的小技巧
	gain = entropy;


	//定义一个3*atrr_kinds的数组，目的为了存储该属性类别下的一系列信息，来计算条件熵
	int** kinds = new int* [3];//c++中定义二维数组的方法
	for (int j = 0; j < 3; j++)
	{
		kinds[j] = new int[atrr_kinds];
		//printf("%d\n",kinds[j]);
		//printf("kinds[%d]=%d\n",j,kinds[j]);
	}

	//初始化
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < atrr_kinds; i++)
		{
			kinds[j][i] = 0;
		}
	}

	/*初始化效果（以OutLook为例）：
			Sunny		OverCast	Rain
	总：	0			0			0
	正：	0			0			0
	负：	0			0			0

	  进行统计后效果（以OutLook为例）：
			Sunny		OverCast	Rain
	总：	5			4			5
	正：	2			4			3
	负：	3			0			2
	*/

	//Gain函数的目的是为了求某一个属性的信息增益（故需要在训练样本中找出该属性的条件熵）
	//将定义的二维数组填满信息
	while (q)
	{
		if (strcmp("OutLook", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)//计算属性类别的信息（样本占比数，正样本数，负样本数）
			{
				if (strcmp(q->OutLook, OutLook_kind[i]) == 0)
				{
					kinds[0][i]++;//计算样本占比数

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;//计算正样本数
					else
						kinds[2][i]++;//计算负样本数
				}
			}
		}
		else if (strcmp("Temperature", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Temperature, Temperature_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;
					else
						kinds[2][i]++;
				}
			}
		}
		else if (strcmp("Humidity", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Humidity, Humidity_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;// 
					else
						kinds[2][i]++;
				}
			}
		}
		else if (strcmp("Wind", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Wind, Wind_kind[i]) == 0)
				{
					kinds[0][i]++;

					if (strcmp(q->PlayTennis, "Yes") == 0)
						kinds[1][i]++;
					else
						kinds[2][i]++;
				}
			}
		}
		q = q->next;
	}

	//计算信息增益，定义一个atrr_kinds的数组（目的存储entropy()如上注释）
	double* gain_kind = new double[atrr_kinds];

	/*
	条件熵公式的计算（需要）：每个属性类别的正负样本及其占比，类别之间的占比
	以OotLook为例：
	entropy(S)= -2/5 * log2(2/5) - 3/5 * log2(3/5)
	entropy(O)= -4/4 * log2(4/4) - 0/4 * log2(0/4)=0
	entropy(R)= -3/5 * log2(3/5) - 2/5 * log2(2/5)
	entropy(SOR)= 5/14entropy(S) + 4/14entropy(O) + 5/14entropy(R)
	gain = entropy(信息熵) - entropy(SOR)
	*/

	//上方公式计算
	for (int j = 0; j < atrr_kinds; j++)
	{
		if (kinds[0][j] != 0 && kinds[1][j] != 0 && kinds[2][j] != 0)
		{
			p1 = 1.0 * kinds[1][j] / kinds[0][j];
			p2 = 1.0 * kinds[2][j] / kinds[0][j];

			gain_kind[j] = -p1 * log(p1) / log2 - p2 * log(p2) / log2;//换底公式
			gain = gain - (1.0 * kinds[0][j] / (positive + negative)) * gain_kind[j];
		}
		else
			gain_kind[j] = 0;//通过上方注释可得出该结论
	}

	//计算该属性的不同类别在总样本中的占比，计算分裂率
	double* split_kind = new double[atrr_kinds];//计算分裂率
	int* split_th = new int[atrr_kinds];//存储该属性类别在样本中的数量
	//初始化
	for (int i = 0; i < atrr_kinds; i++)
		split_th[i] = 0;

	q = L->next;
	while (q)
	{
		if (strcmp("OutLook", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->OutLook, OutLook_kind[i]) == 0)
					split_th[i]++;
			}
		}
		else if (strcmp("Temperature", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Temperature, Temperature_kind[i]) == 0)
					split_th[i]++;
			}
		}
		else if (strcmp("Humidity", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Humidity, Humidity_kind[i]) == 0)
					split_th[i]++;
			}
		}
		else if (strcmp("Wind", atrribute) == 0)
		{
			for (int i = 0; i < atrr_kinds; i++)
			{
				if (strcmp(q->Wind, Wind_kind[i]) == 0)
					split_th[i]++;
			}
		}
		q = q->next;
	}

	/*
	以OutLook为例：
	Sunny	OverCast	Rain
	5		4			5
	split_kind[0] = -5/14 * log2(5/14)
	split_kind[1] = -4/14 * log2(4/14)
	split_kind[2] = -5/14 * log2(5/14)
	split = split_kind[0] + split_kind[1] +split_kind[2]
	gain_ratio = gain/spilit
	*/

	//cout << "该属性的信息增益为" << gain << endl;

	double num;
	double split = 0;

	/*
	for (int i = 0; i < atrr_kinds; i++)
	{
		cout << split_th[i] << endl;
	}*/
	

	for (int i = 0; i < atrr_kinds; i++)
	{
		if (split_th[i] != 0 )
		{
			num = 1.0 * split_th[i] / (positive + negative);
			if (num != 2)
				split_kind[i] = -1 * num * log(num) / log2;
			if (num == 2)
				split_kind[i] = 0;
		}
		else
			split_kind[i] = 0;

		split += split_kind[i];
	}
	//cout << split <<','<<split_kind[0]<<','<<split_kind[1]<<','<<split_kind[2]<< endl;
	double gain_ratio;

	if (gain != 0)
		gain_ratio = gain / split;
	else
		gain_ratio = 0;

	return gain_ratio;
}





void C4_5(tree& T, link L, link Target_Attr, Attributes attr)
{
	//定义p,p1是为了构建attr_chilid的辅助，max是为了找到最大增益的属性
	Attributes p, max, attr_child, p1;

	//定义q和q1是为了构建link_child的辅助
	link q, link_child, q1;

	//定义r是为了构建新的结点以构树
	//定义tree_p的目的是为了当我们构建完每一层的第一个结点后需要改变操作对象（由T到T->firstchild）
	tree r, tree_p;

	//计算总训练样本中的正负样本
	int positive = 0, negative = 0;
	PN_Num(L, positive, negative);

	//初始化两个子集合（两个子集合是构建树的关键）
	attr_child = new AttrNode;
	attr_child->next = NULL;

	link_child = new LNode;
	link_child->next = NULL;

	if (positive == 0)//全是反例
	{
		strcpy_s(T->data, "No");
	}
	else if (negative == 0)//全是正例
	{
		strcpy_s(T->data, "Yes");
	}

	p = attr->next; //属性链表
	double gain, g = 0;

	if (p)
	{
		//建立属性子链表
		while (p)
		{
			//计算所有属性中哪个属性的信息增益最大，做为决策树的根结点
			gain = Gain_Ratio(positive, negative, p->attributes, L, attr);
			cout << p->attributes << "的信息增益率为：" << gain << endl;
			if (gain > g)
			{
				g = gain;
				max = p;
			}
			p = p->next;
		}
		strcpy_s(T->data, max->attributes);//增加决策树的节点

		cout << "信息增益率最大的属性：max->attributes = " << max->attributes << endl << endl;

		//创建属性子链表（树的每一层只需要创建一次属性子链表）
		p = attr->next;
		while (p)
		{
			if (strcmp(p->attributes, max->attributes) != 0)//属性链中不为信息最大增益的属性进行链表构建
			{
				//初始化
				p1 = new AttrNode;
				p1->next = NULL;
				strcpy_s(p1->attributes, p->attributes);
				p1->attr_Num = p->attr_Num;
				//头插法
				p1->next = attr_child->next;
				attr_child->next = p1;
			}
			p = p->next;
		}

		//由于我们已经得到信息增益最大的点，我们就需要构建其分支（即属性类别，也可理解为权重）
		//而我们构建决策树的方式是利用兄弟孩子链表进行构建，因此构建第一步需要找到所有层的第一个结点（孩子结点）
		if (strcmp("OutLook", max->attributes) == 0)
		{
			//结点初始化
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			//为结点的分支赋值（属性类别）
			strcpy_s(r->weight, OutLook_kind[0]);
			T->firstchild = r;

			//建立样本子链表（目的找第一个孩子结点）
			q = L->next;
			while (q)
			{
				//将q->OutLook为“Sunny”的数据进行链表建立
				if (strcmp(q->OutLook, OutLook_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Temperature", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Temperature_kind[0]);
			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Temperature, Temperature_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Humidity", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Humidity_kind[0]);
			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Humidity, Humidity_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}
		else if (strcmp("Wind", max->attributes) == 0)
		{
			r = new TNode;
			r->firstchild = r->nextsibling = NULL;
			strcpy_s(r->weight, Wind_kind[0]);

			T->firstchild = r;
			q = L->next;
			while (q)
			{
				if (strcmp(q->Wind, Wind_kind[0]) == 0)
				{
					q1 = new LNode;
					strcpy_s(q1->OutLook, q->OutLook);
					strcpy_s(q1->Humidity, q->Humidity);
					strcpy_s(q1->Temperature, q->Temperature);
					strcpy_s(q1->Wind, q->Wind);
					strcpy_s(q1->PlayTennis, q->PlayTennis);
					q1->next = NULL;

					q1->next = link_child->next;
					link_child->next = q1;
				}
				q = q->next;
			}
		}

		//上述过程，分别建立了属性子链表和训练样本子链表
		int p = 0, n = 0;
		PN_Num(link_child, p, n);//在子训练样本中找正负样本
		if (p != 0 && n != 0)
		{
			//递归（建立每一层的第一个节点，T->firstchild是重点）
			C4_5(T->firstchild, link_child, Target_Attr, attr_child);
			FreeLink(link_child);//因为link_child会一直不一样，所以占用的空间要及时清空
		}
		else if (p == 0)//当样本都为负样本时
		{
			strcpy_s(T->firstchild->data, "No");
			FreeLink(link_child);
		}
		else if (n == 0)//当样本都为正样本时
		{
			strcpy_s(T->firstchild->data, "Yes");
			FreeLink(link_child);
		}

		/*
		（假设）样本例子（按建立第一个结点后的效果）：
		LookOut
		   |
		Humidity — Temperature — Wind
		*/

		//建立每一层上的其他节点
		//因为我们构建决策树是利用
		tree_p = T->firstchild;//由于我们在上面的操作中已经将每层的第一个结点构建完成，所以现在的操作目标应该从每层的第一个子节点的兄弟结点来操作

		for (int i = 1; i < max->attr_Num; i++)//注意这里的下标从1开始，因为我们已经建立了每层的第一个结点
		{
			//每层的max是固定的，但是分支（weight）是不一样的，因此对应的link_child也不一样
			//需要区分出是哪一种属性
			if (strcmp("OutLook", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, OutLook_kind[i]);//这里是决策树分支的赋值
				tree_p->nextsibling = r;//对每层的兄弟结点进行操作

				q = L->next;
				while (q)
				{
					if (strcmp(q->OutLook, OutLook_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}
			else if (strcmp("Temperature", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Temperature_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Temperature, Temperature_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}

			}
			else if (strcmp("Humidity", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Humidity_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Humidity, Humidity_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}
			else if (strcmp("Wind", max->attributes) == 0)
			{
				r = new TNode;
				r->firstchild = r->nextsibling = NULL;
				strcpy_s(r->weight, Wind_kind[i]);
				tree_p->nextsibling = r;
				q = L->next;
				while (q)
				{
					if (strcmp(q->Wind, Wind_kind[i]) == 0)
					{
						q1 = new LNode;
						strcpy_s(q1->OutLook, q->OutLook);
						strcpy_s(q1->Humidity, q->Humidity);
						strcpy_s(q1->Temperature, q->Temperature);
						strcpy_s(q1->Wind, q->Wind);
						strcpy_s(q1->PlayTennis, q->PlayTennis);
						q1->next = NULL;

						q1->next = link_child->next;
						link_child->next = q1;
					}
					q = q->next;
				}
			}

			//通过正负样本和训练样本子链表，属性子链表的关系，递归建树
			int p = 0, n = 0;
			PN_Num(link_child, p, n);
			if (p != 0 && n != 0)
			{
				//这里操作对象是兄弟结点
				C4_5(tree_p->nextsibling, link_child, Target_Attr, attr_child);
				FreeLink(link_child);
			}
			else if (p == 0)
			{
				strcpy_s(tree_p->nextsibling->data, "No");
				FreeLink(link_child);
			}
			else if (n == 0)
			{
				strcpy_s(tree_p->nextsibling->data, "Yes");
				FreeLink(link_child);
			}

			tree_p = tree_p->nextsibling;//建立所有的孩子结点
		}//建立决策树结束
	}
	else
	{
		q = L->next;
		strcpy_s(T->data, q->PlayTennis);
		//这个地方要赋以训练样本Example中最普遍的Target_attributes的值
	}
}

void show_txt(link LL, const char* Attributes_kind[], const char* Examples[][COL])
{
	FILE* fp;
	if ((fp = fopen("train.txt", "w+")) == NULL)
	{
		printf("File open error!\n");
		exit(0);
	}
	fprintf(fp, "%s: %s	%s	%s\n", Attributes_kind[0], OutLook_kind[0], OutLook_kind[1], OutLook_kind[2]);
	fprintf(fp, "%s: %s	%s	%s\n", Attributes_kind[1], Temperature_kind[0], Temperature_kind[1], Temperature_kind[2]);
	fprintf(fp, "%s: %s	%s\n", Attributes_kind[2], Humidity_kind[0], Humidity_kind[1]);
	fprintf(fp, "%s: %s	%s\n", Attributes_kind[3], Wind_kind[0], Wind_kind[1]);
	fprintf(fp, "\n\n");

	fprintf(fp, "%s		%s          %s	               %s	               PlayTennis\n", Attributes_kind[0], Attributes_kind[1], Attributes_kind[2], Attributes_kind[3]);
	for (int i = 0; i < ROW; i++)
	{
		fprintf(fp, "%s		", Examples[i][0]);
		fprintf(fp, "%s		", Examples[i][1]);
		fprintf(fp, "%s		", Examples[i][2]);
		fprintf(fp, "%s		", Examples[i][3]);
		fprintf(fp, "%s\n", Examples[i][4]);
	}
	fprintf(fp, "\n\n");

	int positive = 0, negative = 0;
	PN_Num(LL, positive, negative);
	fprintf(fp, "正样本:%d;负样本:%d\n", positive, negative);

	if (fclose(fp)) {
		printf("Can not close the file!\n");
		exit(0);
	}
}

//graphic工具，所想即所得
void graphic1(tree T)
{
	FILE* stream1;
	freopen_s(&stream1,"graph1.dot", "w+", stdout);
	if (stdout == NULL)
	{
		printf("File open error!\n");
		exit(0);
	}
	cout << "digraph G{" << endl;
	cout << T->data << "->" << T->firstchild->data << '[' << "label=" << '"' << T->firstchild->weight << '"' << ']' << ';' << endl;
	cout << T->firstchild->data << "->" << T->firstchild->firstchild->data << '1' << '[' << "label=" << '"' << T->firstchild->firstchild->weight << '"' << ']' << ';' << endl;
	cout << T->firstchild->data << "->" << T->firstchild->firstchild->nextsibling->data << '1' << '[' << "label=" << '"' << T->firstchild->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout << T->data << "->" << T->firstchild->nextsibling->data  << '2' << '[' << "label=" << '"' << T->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout << T->data << "->" << T->firstchild->nextsibling->nextsibling->data << '[' << "label=" << '"' << T->firstchild->nextsibling->nextsibling->weight << '"' << ']' << ';' << endl; 
	cout << T->firstchild->nextsibling->nextsibling->data<<"->"<<T->firstchild->nextsibling->nextsibling->firstchild->data << '[' << "label=" << '"' << T->firstchild->nextsibling->nextsibling->firstchild->weight << '"' << ']' << ';' << endl;
	cout << T->firstchild->nextsibling->nextsibling->data << "->" << T->firstchild->nextsibling->nextsibling->firstchild->nextsibling->data << '[' << "label=" << '"' << T->firstchild->nextsibling->nextsibling->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout << "}" << endl;
	//fclose(stdout);
	if (fclose(stdout))
	{
		printf("Can not close the file!\n");
		exit(0);
	}
	system("dot -Tpng graph1.dot -o sample1.png");
}


void graphic2(tree T)
{
	FILE* stream1;
	freopen_s(&stream1, "graph2.dot", "w+", stdout);
	if (stdout == NULL)
	{
		printf("File open error!\n");
		exit(0);
	}
	cout << "digraph G{" << endl;

	cout << T->data << "->" << T->firstchild->data <<'1'<< '[' << "label=" << '"' << T->firstchild->weight << '"' << ']' << ';' << endl;
	cout << T->data << "->" << T->firstchild->nextsibling->data<<'1'  << '[' << "label=" << '"' << T->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->data<<'1'<<"->"<<T->firstchild->firstchild->data<<'1' << '[' << "label=" << '"' << T->firstchild->firstchild->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->data<<'1'<<"->"<<T->firstchild->firstchild->nextsibling->data<<'1' << '[' << "label=" << '"' << T->firstchild->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->data<<'1'<<"->"<<T->firstchild->firstchild->nextsibling->nextsibling->data<<'2'<< '[' << "label=" << '"' << T->firstchild->firstchild->nextsibling->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->firstchild->nextsibling->nextsibling->data<<'2'<<"->"<<T->firstchild->firstchild->nextsibling->nextsibling->firstchild->data <<'2'<< '[' << "label=" << '"' << T->firstchild->firstchild->nextsibling->nextsibling->firstchild->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->firstchild->nextsibling->nextsibling->data<<'2'<<"->"<<T->firstchild->firstchild->nextsibling->nextsibling->firstchild->nextsibling->data<<'2' << '[' << "label=" << '"' << T->firstchild->firstchild->nextsibling->nextsibling->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->nextsibling->data<<'1'<<"->"<<T->firstchild->nextsibling->firstchild->data<<'3' << '[' << "label=" << '"' << T->firstchild->nextsibling->firstchild->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->nextsibling->data<<'1'<<"->"<<T->firstchild->nextsibling->firstchild->nextsibling->data <<'2'<< '[' << "label=" << '"' << T->firstchild->nextsibling->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->nextsibling->firstchild->nextsibling->data<<'2'<<"->"<<T->firstchild->nextsibling->firstchild->nextsibling->firstchild->data <<'4'<< '[' << "label=" << '"' << T->firstchild->nextsibling->firstchild->nextsibling->firstchild->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->nextsibling->firstchild->nextsibling->data<<'2'<<"->"<<T->firstchild->nextsibling->firstchild->nextsibling->firstchild->nextsibling->data <<'5'<< '[' << "label=" << '"' << T->firstchild->nextsibling->firstchild->nextsibling->firstchild->nextsibling->weight << '"' << ']' << ';' << endl;
	cout<<T->firstchild->nextsibling->firstchild->nextsibling->data<<'2'<<"->"<<T->firstchild->nextsibling->firstchild->nextsibling->firstchild->nextsibling->nextsibling->data<<'4' << '[' << "label=" << '"' << T->firstchild->nextsibling->firstchild->nextsibling->firstchild->nextsibling->nextsibling->weight << '"' << ']' << ';' << endl;

	cout << "}" << endl;

	//fclose(stdout);
	if (fclose(stdout))
	{
		printf("Can not close the file!\n");
		exit(0);
	}
	system("dot -Tpng graph2.dot -o sample2.png");
}