
#include <sys/types.h>
#include <vector>
#include <string>

using namespace std;

class Process
{
public:
	Process(const char*);
	~Process();
	void forkExec();
	int wait();
	int getPid() const;
	string readStdout() const;

private:
	char** argumentsAsChars() const;
	string readFp(FILE*) const;

private:
	pid_t mPid;
	string mCommand;
	vector<string> mArguments;
	int mStdinFd;	//!< stdin of this process (write-only)
	int mStdoutFd;	//!< stdout of this process (read-only)
	int mStderrFd;	//!< stderr of this process (read-only)
	FILE* mStdoutFp;
	FILE* mStderrFp;
};
