#pragma once

#include <sys/types.h>
#include <vector>
#include <string>
#include <array>
#include <unistd.h> // for pid_t

using namespace std;
enum class DUPLICATE
{
	NONE,
	STDOUT_TO_STDERR,
	STDERR_TO_STDOUT,
};

class Process
{
public:
	Process(const vector<string>& pArgs);
	Process(const string&);
	Process();
	~Process();
	void setArgs(const vector<string>& pArgs);
	void redirect(int pFileNo, const string pToFile, bool pAppend);
	void duplicate(DUPLICATE);
	pid_t forkExec(pid_t pPGID, int pStdinFN, int pStdoutFN);
	int wait();
	int getPid() const;

	// shell built-in commands
	bool runBuiltInCommands();

private:
	char** argumentsAsChars() const;

private:
	pid_t mPid;
	string mCommand;
	vector<string> mArguments;
	array<string,3> mRedirectDestFile;
	array<bool,3> mRedirectAppendFlg;
	DUPLICATE mDuplicate;
};

