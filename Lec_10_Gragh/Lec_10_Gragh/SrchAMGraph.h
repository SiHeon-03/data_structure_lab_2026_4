#pragma once
#include <cstdio>
#define MAX_VTXS 256

//DFS 구현(인접 행렬)
class SrchAMGraph : public AdjMatGraph
{
	bool visited[MAX_VTXS]; //정점의 방문 정보
public:
	void resetVisited() { //모든 정점을 방문하지 않았다고 설정
		for (int i = 0;i < size;i++)
			visited[i] = false;
	}
	bool isLinked(int u, int v) { return getEdge(u, v) != 0; }
	//깊이 우선 탐색 함수
	void DFS(int v) {
		visited[v] = true; //현재 정점을 방문함
		printf("%c", getVertex(v)); //정점의 이름 출력
		for (int w = 0;w < size;w++)
			if (isLinked(v, w) && visited[w] == false)
				DFS(w); //연결+방문X => 순환호출로 방문
	}
	void display(FILE* fp = stdout) {
		fprintf(fp, "%d\n", size); //정점의 개수 출력
		for (int i = 0;i < size;i++) { //각 행의 정보 출력
			fprintf(fp, "%c", getVertex(i)); //정점의 이름 출력
			for (int j = 0;j < size;j++) //간선 정보 출력
				fprintf(fp, "%3d", getEdge(i, j));
			fprintf(fp, "\n");
		}
	}
};