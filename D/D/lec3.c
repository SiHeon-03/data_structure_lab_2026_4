#include <stdio.h>

int main(void) {
	printf("Hello DS!\n");
	printf("Add two numbers: %d", add_tmp(4,5));

	int a[10] = { 1,2,3,4,3,5,6,7,8,9,10 };
	printf("Add array: %d", add_array(a));
	return 0;
}

int add_tmp(int a, int b) {
	return a + b;
}

int add_array(int s[10]) {
	int s = 0;
	for (int i = 0;i < 10;i++) {
		s +=a[i];
		return s;
	}
}