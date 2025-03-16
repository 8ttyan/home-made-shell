#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "process.h"

void trim(char str[])
{
	for (int i=0; i<1024; i++) {
		if ( str[i]=='\n') {
			str[i]='\0';
			break;
		}
	}
}

int main()
{
	char str[1024];
	while (1) {
		printf("home-made-shell> ");
		if ( fgets(str,1024,stdin)==NULL ) break;
		trim(str);
		if ( strcmp(str,"exit")==0 ) {
			break;
		}
		if ( strlen(str)==0 ) continue;
		Process myProcess(str);
		myProcess.forkExec();
		while (1) {
			string str=myProcess.readStdout();
			if ( str.empty() ) break;
			printf("%s",str.c_str());
		}
		myProcess.wait();
	}
	return 0;
}

