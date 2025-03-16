
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sstream>
#include "process.h"
#include "tokenizer.h"

Process::Process(const string& pCommand)
: mPid(-1)
, mStdinFd(STDIN_FILENO)
, mStdoutFd(STDOUT_FILENO)
, mStderrFd(STDERR_FILENO)
{
//	printf("Process %s\n", pCommand.c_str());
	Tokenizer tokens(pCommand, " ");
	string s;
	tokens >> s;
	mCommand = s;
	mArguments.push_back(s);
	while ( tokens >> s ) {
		mArguments.push_back(s);
	}
}

Process::~Process()
{
}

// The system-call pipe(int fd[2]) return two file descriptors.
// The first element of int fd[2] is descriptor for reading, the second is writing.
#define PIPE_READING 0
#define PIPE_WRITING 1

void Process::connectByPipe(Process& pNextProcess)
{
	int fd[2];
	if ( pipe(fd)!=0 ) {
		printf("failed to create pipe\n");
	}
	mStdoutFd = fd[PIPE_WRITING];
	pNextProcess.mStdinFd = fd[PIPE_READING];
}

void Process::forkExec()
{
//	printf("start %s\n",mCommand.c_str());
	pid_t pid = fork();
	if ( pid<0 ) {	// fork error
		printf("failed to exec process %s\n", mCommand.c_str());
	} else if ( pid>0 ) {	// parent process
		mPid = pid;
		if ( mStdinFd!=STDIN_FILENO ) close(mStdinFd);
		if ( mStdoutFd!=STDOUT_FILENO ) close(mStdoutFd);
		if ( mStderrFd!=STDERR_FILENO ) close(mStderrFd);
	} else {				// child process
		// connect pipe
		if ( mStdinFd!=STDIN_FILENO ) {
			//printf("%s: change stdin\n", mCommand.c_str());
			close(STDIN_FILENO);
			dup2(mStdinFd,STDIN_FILENO);
			close(mStdinFd);
		}
		if ( mStdoutFd!=STDOUT_FILENO ) {
			//printf("%s: change stdout\n", mCommand.c_str());
			close(STDOUT_FILENO);
			dup2(mStdoutFd,STDOUT_FILENO);
			close(mStdoutFd);
		}
		if ( mStderrFd!=STDERR_FILENO ) {
			close(STDERR_FILENO);
			dup2(mStderrFd,STDERR_FILENO);
			close(mStderrFd);
		}

		char** argList = argumentsAsChars();
		execvp(mCommand.c_str(),argList);
		printf("Failed to exec %s\n", mCommand.c_str());
		exit(0);
	}
}

char** Process::argumentsAsChars() const
{
	char** argList = new char*[mArguments.size()+1];
	for (size_t i=0; i<mArguments.size(); i++) {
		const string& argStr=mArguments[i];
		char* argChar = new char[argStr.size()+1];
		memcpy(argChar, argStr.c_str(), argStr.size()+1);
		argList[i]=argChar;
	}
	argList[mArguments.size()]=NULL;
	return argList;
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

