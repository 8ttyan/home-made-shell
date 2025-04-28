
#include "processgroup.h"
#include <termios.h>
#include <signal.h>

ProcessGroup::ProcessGroup()
{
}
Process& ProcessGroup::appendProcess()
{
	mProcessList.push_back( Process() );
	return mProcessList.back();
}

// The system-call pipe(int fd[2]) return two file descriptors.
// The first element of int fd[2] is descriptor for reading, the second is writing.
#define PIPE_READING 0
#define PIPE_WRITING 1

int ProcessGroup::execAndWait()
{

	// save terminal settings
	pid_t myPgid = getpgrp();
	struct termios shell_tmodes;
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(STDIN_FILENO, myPgid);
	signal(SIGTTOU, SIG_DFL);
	tcgetattr(STDIN_FILENO, &shell_tmodes);

	int pgid=0;	// Process Group ID (is equal to first process's PID.)
	int stdinFN=STDIN_FILENO;
	for (auto& proc : mProcessList) {
		int stdoutFN = STDOUT_FILENO;
		int nextStdInFN = STDIN_FILENO;
		if ( &proc != &mProcessList.back() ) {
			int fd[2];
			pipe(fd);
			stdoutFN = fd[PIPE_WRITING];
			nextStdInFN = fd[PIPE_READING];
		}
		int pid = proc.forkExec(pgid,stdinFN,stdoutFN);
		if (stdoutFN!=STDOUT_FILENO) close(stdoutFN);
		if (stdinFN!=STDIN_FILENO)   close(stdinFN);
		stdinFN = nextStdInFN;
		if ( pgid==0 ) pgid=pid;
	}
	int status=0;
	for (auto& proc : mProcessList) {
		status = proc.wait();
	}
	// revert terminal settings
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(STDIN_FILENO,myPgid);
	signal(SIGTTOU, SIG_DFL);
	tcsetattr(STDIN_FILENO,TCSADRAIN,&shell_tmodes);
	return status;
}

