#include <cstdio>

int findArrayMax(int score[], int n)	//자료구조: 배열 array, n은 배열길이
{
	int tmp = score[0];
	for (int i = 1; i < n; i++) {		// 알고리즘
		if (score[i] > tmp) {
			tmp = score[i];
		}
	}
	return tmp;
}

//주 함수
void main()
{
	int score[5] = { 1, 10, 2, 4, 5 };
	int out;
	out = findArrayMax(score, 5);
	printf("result = %d", out);
}