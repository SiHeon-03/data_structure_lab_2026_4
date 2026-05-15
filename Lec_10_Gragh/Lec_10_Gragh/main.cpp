#include "AdjMatGraph.h" //그래프 클래스 포함
#include "SrchAMGraph.h"

void main()
{
	SrchAMGraph g;
	for (int i = 0;i < 4;i++)
		g.insertVertex('A' + i); //정점 삽입: 'A','B',...
	g.insertEdge(0, 1);
	g.insertEdge(0, 3);
	g.insertEdge(1, 2);
	g.insertEdge(1, 3);
	g.insertEdge(2, 3);
	printf("인정 행렬로 표현한 그래프\n");
	g.display();
	//파일 출력
	FILE* fp;
	fopen_s(&fp, "../text_out.txt", "w");
	g.display(fp);
	fclose(fp);


	//DFS 탐색으로 방문
	printf("DFS==>");
	g.resetVisited(); //모든 정점을 방문하지 않았다고 하고
	g.DFS(0); //0번째 정점(A)에서 깊이 우선 탐색 시작
	printf("\n");
}
