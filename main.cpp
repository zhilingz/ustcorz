#include <iostream>
#include <vector>
#include <fstream> 
#include "time.h" 
#include <stdlib.h>
constexpr auto NUM_ID = 7000; 
constexpr auto NUM_CIR = 230;
constexpr auto LEN = 5100;
using namespace std;
/*
有向图存储找有向环，
*/

int visited[NUM_ID];
typedef struct{
	unsigned int data[7] = { 0 };
	unsigned int length=0;
}CIR;
CIR circle[NUM_CIR];

	bool DFS(unsigned int G[][9], unsigned int ID, int numcirl) {
	visited[ID] = numcirl;
	circle[numcirl].data[circle[numcirl].length++] = ID;
	if (circle[numcirl].length == 7) {//环的长度要小于8
		for (unsigned int k=1; k <= G[ID][0]; k++) {//ID有没有出度是环头
			if (G[ID][k] == circle[numcirl].data[0]) return 1;//找到环
		}
		circle[numcirl].length = 0;
		visited[ID] = -1;
		return 0;//不是环
	}
	for (unsigned int i = 1; i <= G[ID][0]; i++) {
		if (circle[numcirl].length > 2)//环的长度要大于2
			for (unsigned int j = 1; j <= G[ID][0]; j++)
				if (G[ID][j] == circle[numcirl].data[0]) return 1;//找到环
		if (visited[G[ID][i]] == numcirl) continue;
		if (DFS(G, G[ID][i], numcirl)) return 1;
		else {
			visited[ID] = -1;
			return 0;
		}
	}
	circle[numcirl].length = 0;
	return 0;
}

int main()
{
	long start = clock();  //开始时间
	long finish;//结束时间，用于测试代码运行时间
	long t;//时间间隔
   
	//打开读取数据的文件
	FILE* fin = NULL;
	if (fopen_s(&fin, "test_data.txt", "rb") != 0) {
		printf("error open test_data.txt\n");
		return 0;
	}
	if (fin == 0) return 0;

	unsigned int* df1 = (unsigned int*)malloc(sizeof(unsigned int) * NUM_ID);
	unsigned int* df2 = (unsigned int*)malloc(sizeof(unsigned int) * NUM_ID);
	unsigned int n = 0;
	unsigned int max=0;//最大的ID
	while (fscanf_s(fin, "%u", df1 + n) != EOF) {
		fseek(fin, 1, 1);
		fscanf_s(fin, "%u", df2 + n);
		if (df1[n] > max) max = df1[n];
		if (df2[n] > max) max = df2[n];
		fseek(fin, 2, 1);
		while (fgetc(fin) != '\n');
		n++;
	}
	fclose(fin);
	
	unsigned int G[NUM_ID][9];//有向图 

	for (unsigned int i = 0; i <= max; i++){
		G[i][0] = 0;
		visited[i] = -1;
	}

	unsigned int ID;
	
	//创建有向图，邻接矩阵
	for (unsigned int i = 0; i < n; i++) {
		ID = df1[i];
		G[ID][G[ID][0]+1] = df2[i];
		G[ID][0]++;//出度加一
	}

	int numcirl=0;//环的总数量
	for (unsigned int i = 0; i <= max; i++) {
		if (i == 6000)
			printf("d");
		circle[numcirl].length = 0;//初始化
		if (G[i][0] == 0) continue;
		circle[numcirl].data[circle[numcirl].length++] = i;
		visited[i] = numcirl;
		ID = G[i][1];
		if (i > 4000 && circle[112].length != 6)
			printf("d");

		if (DFS(G, ID, numcirl)) numcirl++;
		else {
			visited[i] = -1;
			circle[numcirl].length = 0;
		}
	}
	unsigned int tmp[7], len, minID, flag, *sort;
	for (unsigned int i = 0; i < numcirl; i++) {//环内排序
		sort = &circle[i].data[0];
		len = circle[i].length;
		minID = *sort;

		flag = 0;
		for (unsigned int j = 1; j < len; j++) {//找最小
			if (*(sort+j) < minID) {
				minID = *(sort+j);
				flag = j;
			} 
		}
		if (flag == 0)continue;//顺序正确
		for (unsigned int j = 0; j < len; j++) {
			tmp[j] = *(sort + j);
		}
		for (unsigned int j = 0; j < len; j++) {
			*(sort + j) = tmp[(flag + j) % len];
		}
	}
	
	unsigned int* sortID = (unsigned int*)malloc(sizeof(unsigned int) * numcirl),head=0,rear=numcirl-1;
   //将环的序号按环的长度收集，第一趟：3，7；第二趟：4，5；第三趟：6；
	for (unsigned int j= 0; j < numcirl; j++) {//环间排序
		if (circle[j].length == 3) sortID[head++] = j;
		if (circle[j].length == 7) sortID[rear--] = j;
	}
	for (unsigned int j = 0; j < numcirl; j++) {//环间排序
		if (circle[j].length == 4) sortID[head++] = j;
		if (circle[j].length == 6) sortID[rear--] = j;
	}
	for (unsigned int j = 0; j < numcirl; j++) {//环间排序
		if (circle[j].length == 5) sortID[head++] = j;
	}

	//输出到文件
	FILE* fout = NULL;
	if (fopen_s(&fout, "myresult.txt", "wb") != 0) {
		printf("error open result.txt\n");
		return 0;
	}
	if (fout == 0) return 0;
	unsigned int idx;
	fprintf(fout, "%u\n", numcirl);
	for (unsigned int i = 0, j; i < numcirl; i++) {
		idx = sortID[i];//按正确的顺序输出环
		for (j = 0; j < circle[idx].length - 1; j++) {
			fprintf(fout, "%u,", circle[idx].data[j]);
		}
		fprintf(fout, "%u\n", circle[idx].data[j]);//每行最后不输出逗号
	}
	

	printf("%u", max);
	finish = clock();   //结束时间
	cout << "时间: " << finish - start << "ms\n\n"; //输出时间
	t = finish - start;
}

