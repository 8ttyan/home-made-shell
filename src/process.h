
#include <sys/types.h>
#include <vector>
#include <string>

using namespace std;

class Process
{
public:
	Process(const char*);
	void forkExec();
	int wait();
	int getPid() const;

private:
	char** argumentsAsChars() const;

private:
	pid_t mPid;
	string mCommand;
	vector<string> mArguments;
};
