#include<stdio.h>
char *a = "echo \"2017301390018\">gkf.txt";

int main(){
	char * c = a;
	while(*c != 0){
		c++;
	}
	int * d = (int *)c;
	d--;
	printf("push 0x%8x\n",*(d));
	printf("push 0x%8x\n",*(d - 1));
	printf("push 0x%8x\n",*(d - 2));
	printf("push 0x%8x\n",*(d - 3));
	printf("push 0x%8x\n",*(d - 4));
	printf("push 0x%8x\n",*(d - 5));
	printf("push 0x%8x\n",*(d - 6));

}
