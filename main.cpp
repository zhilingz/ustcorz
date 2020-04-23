#include <iostream>
#include <fstream> 
#include <stdlib.h>
#include <time.h>
const int NUM_ID = 30000;//NUM_IDӦ�ô�������ID��max
const int NUM_CIR = 3000000;//���������Ŀ�����Ǵ����ظ��Ļ���
const int EDGE = 280000;//EDGEӦ�ô��ڱߵ�����
const int DEGREE = 30;//Ӧ�ô������ĳ���
using namespace std;

/*
�Ż���
1.�һ���ʱ��ֻ������С�ĳ����ڻ�ͷ�����ǻ��г�������С����ֱ������
���ô��������ظ�������������Ԫ������
2.�һ�ǰ��֦�����������ȥ�����ܳɻ��ĵ㣩
3.����ʹ��ȫ�ֱ��������ٶ�̬�����ʱ��
*/

/*ȫ�ֱ���*/
//0��ʾδ�ڷ��ʣ�1��ʾ���ڷ���
bool visited[NUM_ID];
typedef unsigned int ui;
ui queueID[NUM_ID];//queueID�Ǵ洢���Ϊ0�Ķ��У�0��Ԫ��Ϊ����
char tem_degree[NUM_ID];//����һ��������飬����ı�G
ui p, q;//�����������򣬸�������
ui numcirl = 0;//����������
ui df[2 * EDGE];//���ݵ��ļ��������df��data of file�������У�ת�� ��ת ת��...
ui i, j, k, i_circle;//ר������ѭ����
ui ID1, ID2;//����ͼʱ�ĸ�������
ui edge_DFS;//DFS�еĸ�������
//��������Ű����ĳ����ռ���sortidx
//�ô��ǣ����öԻ������ƶ���ֻ��Ҫ������Ӧ�����(idx)�ź��򼴿�;
ui sortidx[NUM_CIR];

//����ṹ�壬ȫ�ֱ���
typedef struct {
	ui data[7];//��һ����
	ui length;//���ĳ���
}CIR;//һ��������Ϣ
CIR circle[NUM_CIR];//Ԫ��Ϊ���ṹ�������

//����ͼ�Ľ��Ľṹ�壬�ڽӾ���
typedef struct {
	char abandon;//����������û���������������У���/����Ϊ0�������ֵΪ1
	char degree[2];//0���ȣ�1���
	ui edge[DEGREE];//����
}Graph;
/*�ڽӾ���
��ţ����û�ID��������ʼ���ʱ�俪��
��0�У��ö���ĳ��ȣ����ID�˺Ÿ����ٸ���ת�ˣ�
��1�У���ȣ����ID�˺����˶����˵�ת�ˣ�
��n�У�n>=2�����ö���ĳ����ڽӽ��
*/
Graph G[NUM_ID];

//numcirl++���»�����Ҫ������һ����������
void DFS(ui ID) {
	
	visited[ID] = 1;
	circle[numcirl].data[circle[numcirl].length] = ID;
	circle[numcirl].length++;
	
	for (ui i_DFS = 0; i_DFS < G[ID].degree[0]; i_DFS++) {
		edge_DFS = G[ID].edge[i_DFS];
		if (G[edge_DFS].abandon == 1) continue;//���������ĵ�
		if (edge_DFS < circle[numcirl].data[0]) continue;//���뱣֤��ͷ��С
		if (circle[numcirl].length >= 3 && circle[numcirl].length <= 6) {
			//��edge_DFSΪ��ͷ
			if (edge_DFS == circle[numcirl].data[0]) {
				numcirl++;
				circle[numcirl].length = 0;
				//����һ������ǰ���ָ��Ƶ���һ����,visited��Ҫ��
				for (j = 0;; j++) {
					circle[numcirl].data[j] = circle[numcirl - 1].data[j];
					circle[numcirl].length++;
					if (circle[numcirl].data[j] == ID) break;
				}
				continue;
			}
		}

		//1.��length==1���˾���Ч
		//2.��length==2�������ɻ�Ҳ��������ĿҪ��
		//3.��length>=3�����ڻ�ͷ�����Ѿ����ʹ���
		if (visited[edge_DFS] != 0) continue;

		if (circle[numcirl].length == 6) {
			//ID�ĳ���(edge_DFS)�ĳ�������û�л�ͷ
			for (k = 0; k < G[edge_DFS].degree[0]; k++) {
				//��edge_DFS�ĳ������л�ͷ
				if (G[edge_DFS].edge[k] == circle[numcirl].data[0]) {
					circle[numcirl].data[circle[numcirl].length] = edge_DFS;
					circle[numcirl].length++;
					numcirl++;
					circle[numcirl].length = 0;
					//����һ������ǰ���ָ��Ƶ���һ����,visited��Ҫ��
					for (j = 0; j < 6; j++) {
						circle[numcirl].data[j] = circle[numcirl - 1].data[j];
						circle[numcirl].length++;
					}
					break;//�������edge_DFS���������г���Ϊ��ͷ��
				}
			}
			continue;//����ID����һ���������Ƿ��л�ͷ
		}

		//�˾�����Ĵ��벻Ҫʹ��edge_DFS��ȫ�ֱ�����
		//��ΪDFS���Ѿ���edge_DFS���¸�ֵ
		DFS(edge_DFS);
	}
	visited[ID] = 0;
	circle[numcirl].length--;
}


int main()
{
	long start = clock();  //��ʼʱ��
	long finish;//����ʱ�䣬���ڲ��Դ�������ʱ��

	//�򿪶�ȡ���ݵ��ļ��������df��data of file�������У�ת�� ��ת ת��...
	FILE* fin = NULL;
	ui n = 0;
	ui max = 0;//����ͼ������ID
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

	//��������ʱ��
	finish = clock();
	cout << "���ļ�ʱ��: " << finish - start << "ms\n";

	//��������ͼ(�ڽӾ���)
	for (i = 0; i <= max; i++) {
		G[i].degree[0] = 0;//���г��ȳ�ʼ��Ϊ0
		G[i].degree[1] = 0;//������ȳ�ʼ��Ϊ0
		G[i].abandon = 1;
		visited[i] = 0;//���Ҳ��ʼ�����ʼ�¼����
	}
	for (i = 0; i < n; i = i + 2) {
		ID1 = df[i];
		ID2 = df[i + 1];
		G[ID1].edge[G[ID1].degree[0]] = ID2;
		G[ID1].degree[0]++;//���ȼ�һ
		G[ID2].degree[1]++;//��ȼ�һ
		if (G[ID1].degree[0] >= DEGREE) {
			printf("DEGREE too small");
			return 0;
		}
	}

	//��������ʱ��
	finish = clock();
	cout << "��������ͼʱ��: " << finish - start << "ms\n";

	//���������֦��abandon��1
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
		//printf("%u ���ȣ�%u �����%u\n", p,G[p].degree[0], G[p].degree[1]);
		for (char d = 0; d < G[p].degree[0]; d++) {
			q = G[p].edge[d];
			tem_degree[q]--;
			if (tem_degree[q] == 0) {
				queueID[queueID[0] + 1] = q;
				queueID[0]++;
			}
		}
	}

	//��������ʱ��
	finish = clock();
	cout << "��֦ʱ��: " << finish - start << "ms\n";

	//�һ��������Ѿ����������ظ�
	circle[0].length = 0;
	for (i_circle = 0; i_circle <= max; i_circle++) {
		if (G[i_circle].abandon == 1) continue;
		DFS(i_circle);
	}

	//��������ʱ��
	finish = clock();
	cout << "�һ�ʱ��: " << finish - start << "ms\n";

	FILE* fout = NULL;
	if ((fout = fopen("myresult.txt", "wb")) == NULL) {
		printf("error open result.txt\n");
		return 0;
	}
	if (fout == 0) return 0;
	fprintf(fout, "%u\n", numcirl);
	//���������Ϊ3�Ļ������������Ϊ4�Ļ�������
	for (k = 3; k <= 7; k++) {
		for (i = 0, j; i < numcirl; i++) {
			if (circle[i].length != k)continue;
			for (j = 0; j < circle[i].length - 1; j++) {
				fprintf(fout, "%u,", circle[i].data[j]);
			}
			fprintf(fout, "%u\n", circle[i].data[j]);//ÿ������������
		}
	}
	
	//��������ʱ��
	finish = clock();
	cout << "д���ļ�ʱ��: " << finish - start << "ms\n";
}