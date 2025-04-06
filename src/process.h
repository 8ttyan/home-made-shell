#pragma once

#include <sys/types.h>
#include <vector>
#include <string>
#include <unistd.h> // for pid_t

using namespace std;

class Process
{
public:
	Process(const vector<string>& pArgs);
	Process(const string&);
	Process();
	~Process();
	void setArgs(const vector<string>& pArgs);
	void connectByPipe(Process&);
	pid_t forkExec(pid_t pPGID);
	int wait();
	int getPid() const;

private:
	char** argumentsAsChars() const;

private:
	pid_t mPid;
	string mCommand;
	vector<string> mArguments;
	int mStdinFd;
	int mStdoutFd;
	int mStderrFd;
};

