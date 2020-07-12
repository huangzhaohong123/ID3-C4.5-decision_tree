

#include "pch.h"
#include <iostream>

const char* Examples[ROW][COL] = {
	//"OverCast","Cool","High","Strong","No",
	//"Rain","Hot","Normal","Strong","Yes",
	"Sunny","Hot","High","Weak","No",
	"Sunny","Hot","High","Strong","No",
	"OverCast","Hot","High","Weak","Yes",
	"Rain","Mild","High","Weak","Yes",
	"Rain","Cool","Normal","Weak","Yes",
	"Rain","Cool","Normal","Strong","No",
	"OverCast","Cool","Normal","Strong","Yes",
	"Sunny","Mild","High","Weak","No",
	"Sunny","Cool","Normal","Weak","Yes",
	"Rain","Mild","Normal","Weak","Yes",
	"Sunny","Mild","Normal","Strong","Yes",
	"OverCast","Mild","Normal","Strong","Yes",
	"OverCast","Hot","Normal","Weak","Yes",
	"Rain","Mild","High","Strong","No"
};
const char* Attributes_kind[4] = { "OutLook","Temperature","Humidity","Wind" };
int	   Attr_kind[4] = { 3,3,2,2 };

int main()
{



	//char* kind[COL - 1];
	link LL, p;
	Attributes attr_L, q;

	tree T, T_C;

	T = new TNode;
	T->firstchild = T->nextsibling = NULL;
	strcpy_s(T->weight, "");
	strcpy_s(T->data, "");

	T_C = new TNode;
	T_C->firstchild = T_C->nextsibling = NULL;
	strcpy_s(T_C->weight, "");
	strcpy_s(T_C->data, "");

	attr_L = new AttrNode;
	attr_L->next = NULL;

	LL = new LNode;
	LL->next = NULL;

	int choice;
	printf("					该系统通过决策树预测在何种天气下适合出门打球\n");
	printf("				$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("				按键1：导入训练样本，显示训练数据\n");
	printf("				按键2：构建决策树,并展示信息增益信息\n");
	printf("				按键3：通过广义表来展示训练出来的决策树\n");
	printf("				按键4：输入测试数据展示该数据在决策树中的遍历路径，并输出分类结果\n");
	printf("				按键5：改进算法，优越性展示\n");
	printf("				按键6：可视化决策树(该程序使用会直接释放内存)，建议关闭程序时使用\n");
	printf("				按键0: 退出系统\n");
	printf("				$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

	cout << "选择操作：";
	cin >> choice;
	while (choice != 0)
	{
		switch (choice)
		{
		case 1:
		{
		InitLink(LL, Examples);
		printf("成功导入样本数据\n\n");
		show(Examples);
		int choice1;
		printf("是否将训练样本信息进行打印(按键1:是;其他按键:否):");
		cin >> choice1;

		if (choice1 == 1)
		{
			if (LL->next != NULL)
			{
				show_txt(LL, Attributes_kind, Examples);
				printf("操作成功，请到相应目录找寻文件!\n");
			}
			else
				printf("训练样本链表未建立，建立后可呈现正负样本数!\n");
		}
		break;
		}
		case 2:
			if (LL->next != NULL)
			{
				InitAttr(attr_L, Attributes_kind, Attr_kind);
				ID3(T, LL, NULL, attr_L);
				if (T)
					printf("成功构建决策树！\n\n");
				else
					printf("决策树为空\n\n");
			}
			else
				printf("未导入训练样本，无法构建决策树！！！\n");
			break;
		case 3:
			if (T->firstchild != NULL)
			{
				int n;
				printf("是否展现树的分支权值（属性类别）(按键1.是;按键2.否):");
				cin >> n;
				if (n == 1)
				{
					printf("说明：{}内为分支权值（属性类别）\n");
					int i = 0;
					treelists1(T,i);
				}
				else  if (n == 2)
					treelists(T);
				else
					printf("操作错误!!!\n");
				cout << endl << endl;
			}
			if (T->firstchild == NULL)
				printf("决策树未构建!!!\n");
			break;
		case 4:
		{
			if (T->firstchild != NULL)
			{
				char train[COL - 1][20];
				cout << endl;
				int g = 0;
				printf("预测在某天气下是否出门打球\n");
				printf("仔细阅读输入数据规范要求\n");
				printf("----------------------------------------\n");
				cout << "四种属性:" << "OutLook" << ',' << "Temperature" << ',' << "Humidity" << ',' << "Wind" << endl;
				cout << "OutLook属性类别为:" << "Sunny" << ',' << "OverCast" << ',' << "Rain" << endl;
				cout << "Temperature属性类别为:" << "Hot" << ',' << "Mild" << ',' << "Cool" << endl;
				cout << "Humidity属性类别为:" << "High" << ',' << "Normal" << endl;
				cout << "Wind属性类别为:" << "Weak" << ',' << "Strong" << endl;
				printf("----------------------------------------\n");
				printf("请输入OutLook的属性类别：");
				cin >> train[0];
				if (!(strcmp(train[0], "Sunny") == 0 || strcmp(train[0], "OverCast") == 0 || strcmp(train[0], "Rain") == 0))
				{
					printf("操作错误，待输入完全后，请重新操作！！！\n");
					g++;
				}
				printf("请输入Temprature的属性类别：");
				cin >> train[1];
				if (!(strcmp(train[1], "Hot") == 0 || strcmp(train[1], "Mild") == 0 || strcmp(train[1], "Cool") == 0))
				{
					printf("操作错误，待输入完全后，请重新操作！！！\n");
					g++;
				}
				printf("请输入Humidity的属性类别：");
				cin >> train[2];
				if (!(strcmp(train[2], "High") == 0 || strcmp(train[2], "Normal") == 0))
				{
					printf("操作错误，待输入完全后，请重新操作！！！\n");
					g++;
				}
				printf("请输入Wind的属性类别：");
				cin >> train[3];
				if (!(strcmp(train[3], "Weak") == 0 || strcmp(train[3], "Strong") == 0))
				{
					printf("操作错误，待输入完全后，请重新操作！！！\n");
					g++;
				}

				if (g == 0)
				{
					stack<char*> a;
					//int g = 0;
					Test(T, train, T->data, a);
					//cout<<g<<' '<<a.size()<<endl;
					//cout << a.top() << endl;
					if (strcmp(a.top(), "Yes") == 0 || strcmp(a.top(), "No") == 0)
					{
						printf("预测结果(PlayTennis)：");

						if (strcmp(a.top(), "Yes") == 0)
							printf("可以出门打球！\n");
						else
							if (strcmp(a.top(), "No") == 0)
								printf("唉，不能出门打球了。。。\n");

						printf("输出测试数据在ID3算法构建出的决策树下经过的路径:");
						printf("%s", T->data);
						route(T->firstchild, train);
						cout << endl;
						printf("是否比较在C4.5算法下测试数据经过在决策树中经过的路径(按键1:是；其他按键:否):");
						int choice1;
						cin >> choice1;
						if (choice1 == 1)
						{
							if (T_C->firstchild != NULL)
							{
								printf("输出测试数据在C4.5算法构建出的决策树下经过的路径:");
								printf("%s", T_C->data);
								route(T_C->firstchild, train);
							}
							else
								printf("还未通过C4.5算法建立决策树，请选择按键5优化算法构建决策树!!!\n");
						}
						cout << endl;
					}
				}
				else
				{
					printf("重要的事说三遍：操作失误，请重新操作！！！\n");
				}
			}
			else
				printf("决策树未构建!\n");

			break;
		}

		case 5:
		{
			if (LL->next == NULL)
				printf("训练样本未导入!\n");
			else
			{
			printf("通过C4.5算法构建决策树，优化于ID3算法\n");
			C4_5(T_C, LL, NULL, attr_L);
			if (T_C)
				printf("成功构建决策树！\n\n");
			else
				printf("决策树为空\n\n");
			printf("以广义表形式展现决策树:\n");
			if (T_C != NULL)
			{
				printf("不带分支:");
				treelists(T_C);
				cout << endl;
				printf("带分支:");
				int i = 0;
				treelists1(T_C,i);
				cout << endl << endl;
			}
			if (T_C == NULL)
				printf("决策树未构建!!!\n");
			}
			break;
		}
		case 6:
		{
			int choice1;
			printf("决策树可视化（按键1:ID3算法构建的决策树;按键2：C4.5算法构建的决策树）:");
			cin >> choice1;
			if (choice1 == 1)
			{
				if (T->firstchild != NULL)
					graphic1(T);
				else
					printf("树未构建!\n");
			}
				
			else if (choice1 == 2)
			{
				if (T_C->firstchild != NULL)
					graphic2(T_C);
				else
					printf("树未构建!\n");
			}
			else
				printf("操作错误!\n");
			break;
		}
		default:printf("操作失误!\n"); break;


		}
		cout << "选择操作：";
		cin >> choice;
	}
	printf("谢谢使用！！！\n");
	cout << endl;
	return 0;
}


