
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "process.h"

Process::Process(const char* pCommand)
:mCommand(pCommand)
, mPid(0)
{
}

void Process::forkExec()
{
	pid_t pid = fork();
	if ( pid<0 ) {
		printf("failed to exec process %s\n", mCommand.c_str());
		return;
	}
	if ( pid==0 ) {	// child process
		execlp(mCommand.c_str(),mCommand.c_str(),NULL);
		return;
	}
	mPid = pid;
}

int Process::wait()
{
	int status=0;
	waitpid(mPid, &status, 0);
	return status;
}

int Process::getPid() const
{
	return mPid;
}

