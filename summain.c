#include <stdio.h>

extern sumfunc;

int main(){
	int a = 10;
	int b = 13;
	int c = sum(a, b);
	printf("%d", c);
	return 0;
}
