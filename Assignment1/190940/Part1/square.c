#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{
	unsigned long long int n;
	char** s=argv;
	char s1[7]="root";
	char s2[7]="double";
	char s3[7]="square";
	char S[100000]="";
	char file[9]="./"; //for storing the next operation
	char* s4=(char*)malloc(40);//for the number 
	int t=0;
	int i=0;
	while(i<(argc-1)){
		if(t==1){
			printf("UNABLE TO EXECUTE");
			return 0;
		}
	    if(!strcmp(s[i+1],s1) || !strcmp(s[i+1],s2) || !strcmp(s[i+1],s3)){
			if(i==0){
				strcat(file,s[i+1]);
			}
			else{
				strcat(S,s[i+1]);
			    strcat(S," ");
			}
		}
		else {
			t=1;
			s4=s[i+1];
		}
		i++;
	}

	//find the number and square it.
	if(t){
	    n=atoll(s4);
	    n=n*n;
	}

	else {
		printf("UNABLE TO EXECUTE");
		return 0;
	}

	//if there is no other operation
	if(!strcmp(file,"./")){
		printf("%lld",n);
		return 0;
	}

	//if there are some more operations.
	sprintf(s4,"%lld",n);
	strcat(S,s4);
	execl(file,file,S,NULL);
	return 0;
}
