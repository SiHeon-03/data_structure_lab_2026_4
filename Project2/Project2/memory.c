#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct studentTag {
	char name[10];
	int age;
	double gpa;

}student;

int main(void)
{
	student* p;
	p = (student*)malloc(sizeof(student));

	strcpy(p->name, "Park");
	p->age = 20;

	printf("Name: %s\n", (*p).name);
	printf("Age: %d\n", (*p).age);

	
	free(p);
	return 0;
}