#include <stdio.h>

void main()
{
	int i, j;
	int* ptr;

	ptr = &i;
	*ptr = 10;
	printf("i: %d\n", i); //i에 간접적으로 값 저장
	printf("i의 주소 : %u\n", &i);

}