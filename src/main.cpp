#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <list>

#include "lexicaltokenizer.h"
#include "parser.h"
#include "process.h"
#include "prompter.h"
#include "shell.h"
#include "test.h"
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

int main(int argc, const char *argv[])
{
	if ( argc>=3 ) {
		string arg1=argv[1];
		string arg2=argv[2];
		if ( arg1=="-test" ) {
			Test myTest(arg2);
			exit(0);
		}
	}
	while (1) {
		Prompter prompter(stdin,stdout);
		LexicalTokenizer lt(prompter);
		Shell shell;
		Parser parser;
		parser.run(lt, &shell);
		shell.exec();
	}
	return 0;
}

