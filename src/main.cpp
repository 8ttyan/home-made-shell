#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <list>

#include "process.h"
#include "tokenizer.h"

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
		list<Process> procList;
		Tokenizer tokenWithPipe(str,"|");
		string command;
		while ( tokenWithPipe >> command ) {
			Process myProcess(command);
			if ( procList.size()>0 ) {
				procList.back().connectByPipe(myProcess);
			}
			procList.push_back(myProcess);
		}
		for (auto& proc : procList) {
			proc.forkExec();
		}
		for (auto& proc : procList) {
			proc.wait();
		}
	}
	return 0;
}

