
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sstream>
#include "process.h"
#include "tokenizer.h"

Process::Process(const vector<string>& pArgs)
: mPid(-1)
, mStdinFd(STDIN_FILENO)
, mStdoutFd(STDOUT_FILENO)
, mStderrFd(STDERR_FILENO)
{
	mCommand = pArgs[0];
	mArguments = pArgs;
}
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
Process::Process()
: mPid(-1)
, mStdinFd(STDIN_FILENO)
, mStdoutFd(STDOUT_FILENO)
, mStderrFd(STDERR_FILENO)
{
}
void Process::setArgs(const vector<string>& pArgs)
{
	if ( pArgs.size()==0 ) return;
	mCommand = pArgs[0];
	mArguments = pArgs;
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

pid_t Process::forkExec(pid_t pPGID)
{
//	printf("start %s\n",mCommand.c_str());
	if ( runBuiltInCommands() ) return 0;
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
		// set ProcessGroupID(PGID)
		if ( setpgid(0,pPGID)==-1 ) {
			printf("Failed to set PGID\n");
		}
		char** argList = argumentsAsChars();
		execvp(mCommand.c_str(),argList);
		printf("Failed to exec %s\n", mCommand.c_str());
		exit(0);
	}
	return pid;
}

bool Process::runBuiltInCommands()
{
	if ( mCommand=="cd" ) {
		if ( mArguments.size()<=1 ) return true;
		chdir(mArguments[1].c_str());
		return true;
	}
	if ( mCommand=="exit" ) {
		int status=0;
		if ( mArguments.size()>=2 ) {
			try {
				status = std::stoi(mArguments[1]);
			} catch (...) {
				// nothing to do
			}
		}
		//printf("exit %d\n",status);
		exit(status);
		return true;
	}
	return false;
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

