#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

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
		printf("$");
		if ( fgets(str,1024,stdin)==NULL ) break;
		trim(str);
		if ( strcmp(str,"exit")==0 ) {
			break;
		}
		//puts(str);
		pid_t pid = fork();
		if ( pid<0 ) {	// error
			puts("fork error");
			continue;
		} else if ( pid==0 ) {	// child process
			//puts("child process");
			//puts("exec");
			execlp(str,str,NULL);
			//puts("exec error");
			continue;	// error of exec
		} else {	// shell (parent process)
			//printf("child = %d\n", pid);
			int status=0;
			//puts("wait");
			waitpid(pid, &status, 0);
			//puts("end");
		}
	}
	return 0;
}

