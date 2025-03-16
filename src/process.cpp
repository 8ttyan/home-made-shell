
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sstream>
#include "process.h"

Process::Process(const char* pCommand)
: mPid(-1)
, mStdinFd(-1)
, mStdoutFd(-1)
, mStderrFd(-1)
, mStdoutFp(NULL)
, mStderrFp(NULL)
{
	string cmdStr(pCommand);
	istringstream iss(cmdStr);
	string s;
	iss >> s;
	mCommand = s;
	mArguments.push_back(s);
	while ( iss >> s ) {
		mArguments.push_back(s);
	}
}

Process::~Process()
{
	if ( mStdinFd!=-1 ) close(mStdinFd);
	if ( mStdoutFd!=-1 ) close(mStdoutFd);
	if ( mStderrFd!=-1 ) close(mStderrFd);
}

// The system-call pipe(int fd[2]) return two file descriptors.
// The first element of int fd[2] is descriptor for reading, the second is writing.
#define PIPE_READING 0
#define PIPE_WRITING 1

void Process::forkExec()
{
	// preparing file-descriptor before fork
	int stdinFd[2], stdoutFd[2], stderrFd[2];
	pipe(stdinFd);
	pipe(stdoutFd);
	pipe(stderrFd);

	pid_t pid = fork();
	if ( pid<0 ) {	// fork error
		printf("failed to exec process %s\n", mCommand.c_str());
	} else if ( pid>0 ) {	// parent process
		mPid = pid;
		close(stdinFd[PIPE_READING]);
		close(stdoutFd[PIPE_WRITING]);
		close(stderrFd[PIPE_WRITING]);
		mStdinFd = stdinFd[PIPE_WRITING];
		mStdoutFd = stdoutFd[PIPE_READING];
		mStderrFd = stderrFd[PIPE_READING];
		mStdoutFp = fdopen(mStdoutFd,"r");
		mStderrFp = fdopen(mStderrFd,"r");
		if ( mStdoutFp==NULL ) printf("Cannot relate mStdoutFd to mstdoutFp\n");
		if ( mStderrFp==NULL ) printf("Cannot relate mStdoutFd to mstderrFp\n");
	} else {				// child process
		close(stdinFd[PIPE_WRITING]);
		close(stdoutFd[PIPE_READING]);
		close(stderrFd[PIPE_READING]);
		// connect pipe
		close(STDIN_FILENO);  dup2(stdinFd[PIPE_READING],STDIN_FILENO);   close(stdinFd[PIPE_READING]);
		close(STDOUT_FILENO); dup2(stdoutFd[PIPE_WRITING],STDOUT_FILENO); close(stdoutFd[PIPE_WRITING]);
		close(STDERR_FILENO); dup2(stderrFd[PIPE_WRITING],STDERR_FILENO); close(stderrFd[PIPE_WRITING]);

		char** argList = argumentsAsChars();
		execvp(mCommand.c_str(),argList);
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

string Process::readStdout() const
{
	return readFp(mStdoutFp);
}
string Process::readStderr() const
{
	return readFp(mStderrFp);
}

string Process::readFp(FILE* pFp) const
{
	char buf[1024];
	buf[0] = '\0';
	if ( fgets(buf,1024,pFp)==NULL ) return string();
	string str(buf);
	return str;
}

