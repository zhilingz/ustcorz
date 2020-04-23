#include <iostream>
#include <fstream> 
#include <stdlib.h>
#include <time.h>
const int NUM_ID = 30000;//NUM_ID应该大于最大的ID数max
const int NUM_CIR = 3000000;//环的最大数目（考虑大量重复的环）
const int EDGE = 280000;//EDGE应该大于边的总数
const int DEGREE = 30;//应该大于最大的出度
using namespace std;

/*
优化：
1.找环的时候只能让最小的出现在环头，若是环中出现了最小，则直接舍弃
（好处：避免重复，环内有序，首元素有序）
2.找环前剪枝（拓扑排序除去不可能成环的点）
3.尽量使用全局变量，减少动态分配的时间
*/

/*全局变量*/
//0表示未在访问，1表示正在访问
bool visited[NUM_ID];
typedef unsigned int ui;
ui queueID[NUM_ID];//queueID是存储入度为0的队列，0号元素为总数
char tem_degree[NUM_ID];//复制一份入度数组，避免改变G
ui p, q;//用于拓扑排序，辅助变量
ui numcirl = 0;//环的总数量
ui df[2 * EDGE];//数据的文件，存放在df（data of file）数组中，转账 收转 转账...
ui i, j, k, i_circle;//专门用在循环中
ui ID1, ID2;//创建图时的辅助变量
ui edge_DFS;//DFS中的辅助变量
//将环的序号按环的长度收集至sortidx
//好处是，不用对环整体移动，只需要将环对应的序号(idx)排好序即可;
ui sortidx[NUM_CIR];

//定义结构体，全局变量
typedef struct {
	ui data[7];//存一个环
	ui length;//环的长度
}CIR;//一个环的信息
CIR circle[NUM_CIR];//元素为环结构体的数组

//定义图的结点的结构体，邻接矩阵
typedef struct {
	char abandon;//如果这个顶点没丢弃（拓扑排序中，入/出度为0），这个值为1
	char degree[2];//0出度，1入度
	ui edge[DEGREE];//出边
}Graph;
/*邻接矩阵：
序号：即用户ID，随机访问减少时间开销
第0列：该顶点的出度（这个ID账号给多少个人转账）
第1列：入度（这个ID账号收了多少人的转账）
第n列（n>=2）：该顶点的出边邻接结点
*/
Graph G[NUM_ID];

//numcirl++，新环必须要复制上一个环的内容
void DFS(ui ID) {
	
	visited[ID] = 1;
	circle[numcirl].data[circle[numcirl].length] = ID;
	circle[numcirl].length++;
	
	for (ui i_DFS = 0; i_DFS < G[ID].degree[0]; i_DFS++) {
		edge_DFS = G[ID].edge[i_DFS];
		if (G[edge_DFS].abandon == 1) continue;//跳过丢弃的点
		if (edge_DFS < circle[numcirl].data[0]) continue;//必须保证环头最小
		if (circle[numcirl].length >= 3 && circle[numcirl].length <= 6) {
			//若edge_DFS为环头
			if (edge_DFS == circle[numcirl].data[0]) {
				numcirl++;
				circle[numcirl].length = 0;
				//将上一个环的前部分复制到下一个环,visited不要动
				for (j = 0;; j++) {
					circle[numcirl].data[j] = circle[numcirl - 1].data[j];
					circle[numcirl].length++;
					if (circle[numcirl].data[j] == ID) break;
				}
				continue;
			}
		}

		//1.若length==1，此句无效
		//2.若length==2，则就算成环也不满足题目要求
		//3.若length>=3，则不在环头，且已经访问过了
		if (visited[edge_DFS] != 0) continue;

		if (circle[numcirl].length == 6) {
			//ID的出度(edge_DFS)的出度中有没有环头
			for (k = 0; k < G[edge_DFS].degree[0]; k++) {
				//若edge_DFS的出度中有环头
				if (G[edge_DFS].edge[k] == circle[numcirl].data[0]) {
					circle[numcirl].data[circle[numcirl].length] = edge_DFS;
					circle[numcirl].length++;
					numcirl++;
					circle[numcirl].length = 0;
					//将上一个环的前部分复制到下一个环,visited不要动
					for (j = 0; j < 6; j++) {
						circle[numcirl].data[j] = circle[numcirl - 1].data[j];
						circle[numcirl].length++;
					}
					break;//对于这个edge_DFS不可能再有出度为环头了
				}
			}
			continue;//看看ID的下一个出度中是否有环头
		}

		//此句下面的代码不要使用edge_DFS（全局变量）
		//因为DFS中已经对edge_DFS重新赋值
		DFS(edge_DFS);
	}
	visited[ID] = 0;
	circle[numcirl].length--;
}


int main()
{
	long start = clock();  //开始时间
	long finish;//结束时间，用于测试代码运行时间

	//打开读取数据的文件，存放在df（data of file）数组中，转账 收转 转账...
	FILE* fin = NULL;
	ui n = 0;
	ui max = 0;//整张图里最大的ID
	if ((fin = fopen("test_data.txt", "rb")) == NULL) {
		printf("error open test_data.txt\n");
		return 0;
	}
	if (fin == 0) return 0;

	while (fscanf(fin, "%u", df + n) != EOF) {
		if (df[n] > max) max = df[n];
		n++;
		fseek(fin, 1, 1);
		fscanf(fin, "%u", df + n);
		if (df[n] > max) max = df[n];
		fseek(fin, 2, 1);
		while (fgetc(fin) != '\n');
		n++;
	}
	fclose(fin);

	//计算运行时间
	finish = clock();
	cout << "读文件时间: " << finish - start << "ms\n";

	//创建有向图(邻接矩阵)
	for (i = 0; i <= max; i++) {
		G[i].degree[0] = 0;//所有出度初始化为0
		G[i].degree[1] = 0;//所有入度初始化为0
		G[i].abandon = 1;
		visited[i] = 0;//随便也初始化访问记录数组
	}
	for (i = 0; i < n; i = i + 2) {
		ID1 = df[i];
		ID2 = df[i + 1];
		G[ID1].edge[G[ID1].degree[0]] = ID2;
		G[ID1].degree[0]++;//出度加一
		G[ID2].degree[1]++;//入度加一
		if (G[ID1].degree[0] >= DEGREE) {
			printf("DEGREE too small");
			return 0;
		}
	}

	//计算运行时间
	finish = clock();
	cout << "建立有向图时间: " << finish - start << "ms\n";

	//拓扑排序剪枝，abandon置1
	queueID[0] = 0;
	for (i = 0; i <= max; i++) {
		tem_degree[i] = G[i].degree[1];
		if (tem_degree[i] == 0 && G[i].degree[0] != 0) {
			queueID[queueID[0] + 1] = i;
			queueID[0]++;
		}
		if (tem_degree[i] != 0 || G[i].degree[0] != 0) {
			G[i].abandon = 0;
		}
	}
	while (queueID[0] != 0) {
		p = queueID[queueID[0]];
		queueID[0]--;
		G[p].abandon = 1;
		//printf("%u 出度：%u 入读：%u\n", p,G[p].degree[0], G[p].degree[1]);
		for (char d = 0; d < G[p].degree[0]; d++) {
			q = G[p].edge[d];
			tem_degree[q]--;
			if (tem_degree[q] == 0) {
				queueID[queueID[0] + 1] = q;
				queueID[0]++;
			}
		}
	}

	//计算运行时间
	finish = clock();
	cout << "剪枝时间: " << finish - start << "ms\n";

	//找环，环内已经有序，且无重复
	circle[0].length = 0;
	for (i_circle = 0; i_circle <= max; i_circle++) {
		if (G[i_circle].abandon == 1) continue;
		DFS(i_circle);
	}

	//计算运行时间
	finish = clock();
	cout << "找环时间: " << finish - start << "ms\n";

	FILE* fout = NULL;
	if ((fout = fopen("myresult.txt", "wb")) == NULL) {
		printf("error open result.txt\n");
		return 0;
	}
	if (fout == 0) return 0;
	fprintf(fout, "%u\n", numcirl);
	//先输出长度为3的环，再输出长度为4的环。。。
	for (k = 3; k <= 7; k++) {
		for (i = 0, j; i < numcirl; i++) {
			if (circle[i].length != k)continue;
			for (j = 0; j < circle[i].length - 1; j++) {
				fprintf(fout, "%u,", circle[i].data[j]);
			}
			fprintf(fout, "%u\n", circle[i].data[j]);//每行最后不输出逗号
		}
	}
	
	//计算运行时间
	finish = clock();
	cout << "写入文件时间: " << finish - start << "ms\n";
}