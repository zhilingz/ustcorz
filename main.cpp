#include <iostream>
#include <fstream> 
#include <stdlib.h>
#include <time.h>
const int NUM_ID = 30000;//NUM_ID应该大于最大的ID数max
const int NUM_CIR = 10000;//环的最大数目（考虑大量重复的环）
const int LEN = 280000;//LEN应该大于用户的总数
const int DEGREE = 10;//应该大于最大的出度
using namespace std;

//访问标记数组，-1表示不在已找出的所有环中，-2表示在已找出的所有环中
int visited[NUM_ID];

//定义结构体，全局变量
typedef struct{
	unsigned int data[7];//存一个环
	unsigned int length;//环的长度
}CIR;//一个环的信息
CIR* circle;//元素为环结构体的数组

// 返回1表示a代表的环字典序大于b 返回2表示小于 返回0表示等于
unsigned int compare(unsigned int a, unsigned int b) {
	unsigned int length_a = circle[a].length;
	unsigned int length_b = circle[b].length;
	if (length_a > length_b)
		return 1;
	else if (length_a < length_b)
		return 2;

	for (unsigned int i = 0; i < length_a; i++) {
		if (circle[a].data[i] > circle[b].data[i])
			return 1;
		if (circle[a].data[i] < circle[b].data[i])
			return 2;
	}
	return 0;
}

void merge(unsigned int* A, unsigned int L1, unsigned int R1, unsigned int L2, unsigned int R2) {
	unsigned int i = L1, j = L2;
	unsigned int temp[10000], index = 0;
	while (i <= R1 && j <= R2) {
		if (compare(A[i], A[j]) != 1) {
			temp[index++] = A[i++];
		}
		else {
			temp[index++] = A[j++];
		}
	}
	while (i <= R1) temp[index++] = A[i++];
	while (j <= R2) temp[index++] = A[j++];
	for (i = 0; i < index; i++) {
		A[L1 + i] = temp[i];
	}
}
unsigned int min(unsigned int i, unsigned int j) {
	if (i < j)
		return i;
	else
	{
		return j;
	}

}

void mergesort(unsigned int* A, unsigned int n) {
	for (unsigned int step = 2; step / 2 <= n; step *= 2) {
		for (unsigned int i = 1; i <= n; i += step) {
			unsigned int mid = i + step / 2 - 1;
			if (mid + 1 <= n) {
				merge(A, i, mid, mid + 1, min(i + step - 1, n));
			}
		}
	}
}


//类DFS算法，找出所有环（return之前都要删除访问标记）
bool DFS(unsigned int G[][DEGREE], unsigned int ID, unsigned int* p_numcirl) {
	CIR* p_circle = circle+*p_numcirl;
	visited[ID] = *p_numcirl;
	p_circle->data[p_circle->length++] = ID;
	if (p_circle->length == 7) {//环的长度要小于等于7
		for (unsigned int k = 1; k <= G[ID][0]; k++) //ID有没有出度是环头
			if (G[ID][k] == p_circle->data[0]) {
				visited[ID] = -2;
				return 1;//找到环
			}
		//p_circle->length = 0;
		visited[ID] = -1;
		p_circle->length--;
		return 0;//不是环
	}
	for (unsigned int i = 1; i <= G[ID][0]; i++) {
		if (p_circle->length > 2)//环的长度要大于等于3
			for (unsigned int j = 1; j <= G[ID][0]; j++)//ID有没有出度是环头
				if (G[ID][j] == p_circle->data[0]) {
					visited[ID] = -2;
					return 1;//找到环
				}
		if (visited[G[ID][i]] == *p_numcirl) continue;
		if (DFS(G, G[ID][i], p_numcirl)) {//找到环
			if (i == G[ID][0]) {//若无其它的出度则返回1
				visited[ID] = -2;
				return 1;
			}
			else {//若还有其他出度，则继续找环
				(*p_numcirl)++;
				p_circle = circle + *p_numcirl;
				p_circle->length = 0;
				for (unsigned int j = 0; ; j++) {//将上一个环的前部分复制到下一个环
					p_circle->data[j] = (p_circle-1)->data[j];
					p_circle->length++;
					visited[p_circle->data[j]] = *p_numcirl;
					if (p_circle->data[j] == ID) break;
				}
				continue;
			}
		}
	}
	circle[*p_numcirl].length--;//退出递归，且不成环
	visited[ID] = -1;
	return 0;
 }

int main()
{
	long start = clock();  //开始时间
	long finish;//结束时间，用于测试代码运行时间
   
	//打开读取数据的文件，存放在df（data of file）数组中
	FILE* fin = NULL;
	unsigned int* df1 = (unsigned int*)malloc(sizeof(unsigned int) * LEN);
	unsigned int* df2 = (unsigned int*)malloc(sizeof(unsigned int) * LEN);
	unsigned int n = 0;
	unsigned int max = 0;//整张图里最大的ID

	//fopen打开文件
	if ((fin = fopen("test_data.txt", "rb")) == NULL) {
		printf("error open test_data.txt\n");
		return 0;
	}
	if (fin == 0) return 0;

	while (fscanf(fin, "%u", df1 + n) != EOF) {
		fseek(fin, 1, 1);
		fscanf(fin, "%u", df2 + n);
		if (df1[n] > max) max = df1[n];
		if (df2[n] > max) max = df2[n];
		fseek(fin, 2, 1);
		while (fgetc(fin) != '\n');
		n++;
	}
	fclose(fin);

	//创建有向图(邻接矩阵)
	/*邻接矩阵：
	序号：即用户ID，随机访问减少时间开销
	第0列：该顶点的出度（这个ID账号给多少个人转账）
	第n列（n>=1）：该顶点的出边邻接结点
	*/
	unsigned int G[NUM_ID][DEGREE];
	unsigned int ID;
	for (unsigned int i = 0; i <= max; i++) {
		G[i][0] = 0;//所有出度初始化为0
		visited[i] = -1;//随便也初始化访问记录数组
	}
	for (unsigned int i = 0; i < n; i++) {
		ID = df1[i];
		G[ID][G[ID][0]+1] = df2[i];
		G[ID][0]++;//出度加一
		if (G[ID][0] > DEGREE) {
			printf("出度大于DEGREE，应该设大一些");
			return 0;
		}
	}
	free(df1);
	free(df2);

	//DFS找出所有环
	circle = (CIR*)malloc(sizeof(CIR) * NUM_ID);
	unsigned int numcirl=0;//环的总数量
	for (unsigned int i = 0; i <= max; i++) {
		circle[numcirl].length = 0;//初始化环的长度
		if (G[i][0] == 0) continue;//没有出度
		if (DFS(G, i, &numcirl)) 
			numcirl++;//若成环，则环数加一
	}

	//环内排序
	unsigned int tmp[7], len, minID, flag, *sort;
	for (unsigned int i = 0; i < numcirl; i++) {
		//用更加简短的变量代替，代码更加简洁
		if (circle[i].data[0] == 6001)
			printf("\n");
		sort = &circle[i].data[0];
		len = circle[i].length;
		minID = *sort;
		flag = 0;
		for (unsigned int j = 1; j < len; j++)//找最小
			if (*(sort+j) < minID) {
				minID = *(sort+j);
				flag = j;//flag是最小ID在环内的位置
			} 
		if (flag == 0)continue;//若顺序正确则不用调整
		//若顺序不正确，整体位移（flag+j）个单位
		for (unsigned int j = 0; j < len; j++)
			tmp[j] = *(sort + j);
		for (unsigned int j = 0; j < len; j++)
			*(sort + j) = tmp[(flag + j) % len];
	}
	
	//将环的序号按环的长度收集至sortidx，好处是，不用对环整体移动，只需要将环对应的序号(idx)排好序即可;
	unsigned int* sortidx;
	sortidx = (unsigned int*)malloc(sizeof(unsigned int) * numcirl);
	for (unsigned int i = 0; i < numcirl; i++) {
		sortidx[i] = i;
	}
	mergesort(sortidx, numcirl - 1);

	//已经排序好，删去重复
	unsigned int fcount=0;//fcount(final count circle)为最终的不重复的个数-1
	for (unsigned int i = 1,j; i < numcirl;i++) {
		for (j = 0; j < circle[sortidx[fcount]].length; j++) {
			if (circle[sortidx[fcount]].data[j] != circle[sortidx[i]].data[j]) break;
		}
		if (j == circle[sortidx[fcount]].length)
			if (circle[sortidx[i]].length == circle[sortidx[fcount]].length) continue;
		fcount++;
		sortidx[fcount] = sortidx[i];
	}

	//按sortidx中的序号依次将环输出到文件
	FILE* fout = NULL;
	if ((fout = fopen("myresult.txt", "wb"))==NULL) {
		printf("error open result.txt\n");
		return 0;
	}
	if (fout == 0) return 0;
	unsigned int idx;
	fprintf(fout, "%u\n", fcount + 1);
	for (unsigned int i = 0, j; i <= fcount; i++) {
		idx = sortidx[i];
		for (j = 0; j < circle[idx].length - 1; j++) {
			fprintf(fout, "%u,", circle[idx].data[j]);
		}
		fprintf(fout, "%u\n", circle[idx].data[j]);//每行最后不输出逗号
	}
	free(circle);
	//计算运行时间
	finish = clock();
	cout << "时间: " << finish - start << "ms\n"; 
}
