
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sstream>
#include "process.h"

Process::Process(const char* pCommand)
: mPid(-1)
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

void Process::forkExec()
{
	pid_t pid = fork();
	if ( pid<0 ) {
		printf("failed to exec process %s\n", mCommand.c_str());
		return;
	}
	if ( pid>0 ) {	// parent process
		mPid = pid;
		return;
	}
	// child process
	char** argList = argumentsAsChars();
	execvp(mCommand.c_str(),argList);
	return;
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

