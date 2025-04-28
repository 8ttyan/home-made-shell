
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for L_RDONLY,L_WRONLY
#include <sys/wait.h>
#include <string.h>
#include <sstream>
#include "process.h"
#include "tokenizer.h"

Process::Process(const vector<string>& pArgs)
: mPid(-1)
{
	mCommand = pArgs[0];
	mArguments = pArgs;
}
Process::Process(const string& pCommand)
: mPid(-1)
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
{
}
void Process::setArgs(const vector<string>& pArgs)
{
	if ( pArgs.size()==0 ) return;
	mCommand = pArgs[0];
	mArguments = pArgs;
}
void Process::redirect(int pFileNo, const string pToFile, bool pAppend)
{
	mRedirectDestFile[pFileNo] = pToFile;
	mRedirectAppendFlg[pFileNo] = pAppend;
}
void Process::duplicate(DUPLICATE pDuplicate)
{
	mDuplicate = pDuplicate;
}
Process::~Process()
{
}

pid_t Process::forkExec(pid_t pPGID, int pStdinFN, int pStdoutFN)
{
//	printf("start %s\n",mCommand.c_str());
	if ( runBuiltInCommands() ) return 0;
	pid_t pid = fork();
	if ( pid<0 ) {	// fork error
		printf("failed to exec process %s\n", mCommand.c_str());
	} else if ( pid>0 ) {	// parent process
		mPid = pid;
		// set ProcessGroupID(PGID)
		if ( setpgid(pid,pPGID)==-1 ) {
			printf("Failed to set PGID\n");
		}
	} else {				// child process
		// connect pipe
		if ( pStdinFN!=STDIN_FILENO ) {
			//printf("%s: change stdin\n", mCommand.c_str());
			close(STDIN_FILENO);
			dup2(pStdinFN,STDIN_FILENO);
			close(pStdinFN);
		}
		if ( pStdoutFN!=STDOUT_FILENO ) {
			//printf("%s: change stdout\n", mCommand.c_str());
			close(STDOUT_FILENO);
			dup2(pStdoutFN,STDOUT_FILENO);
			close(pStdoutFN);
		}
		if ( mRedirectDestFile[STDIN_FILENO].empty()==false ) {
			int fd = open(mRedirectDestFile[STDIN_FILENO].c_str(), O_RDONLY);
			if ( fd==-1 ) {
				printf("No such file %s\n", mRedirectDestFile[STDIN_FILENO].c_str());
				exit(0);
			}
			close(STDIN_FILENO);
			dup2(fd, STDIN_FILENO);
		}
		if ( mRedirectDestFile[STDOUT_FILENO].empty()==false ) {
			int opt = 0;
			if ( mRedirectAppendFlg[STDOUT_FILENO] ) opt=O_APPEND;
			int fd = open(mRedirectDestFile[STDOUT_FILENO].c_str(), O_WRONLY|O_CREAT|opt, 0666);
			if ( fd==-1 ) {
				printf("Failed to open file %s\n", mRedirectDestFile[STDOUT_FILENO].c_str());
				exit(0);
			}
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
		}
		if ( mRedirectDestFile[STDERR_FILENO].empty()==false ) {
			int opt = 0;
			if ( mRedirectAppendFlg[STDOUT_FILENO] ) opt=O_APPEND;
			int fd = open(mRedirectDestFile[STDERR_FILENO].c_str(), O_WRONLY|O_CREAT|opt, 0666);
			if ( fd==-1 ) {
				printf("Failed to open file %s\n", mRedirectDestFile[STDERR_FILENO].c_str());
				exit(0);
			}
			close(STDERR_FILENO);
			dup2(fd, STDERR_FILENO);
		}
		if ( mDuplicate==DUPLICATE::STDERR_TO_STDOUT ) {
			close(STDOUT_FILENO);
			dup2(STDERR_FILENO, STDOUT_FILENO);
		} else if ( mDuplicate==DUPLICATE::STDOUT_TO_STDERR ) {
			close(STDERR_FILENO);
			dup2(STDOUT_FILENO, STDERR_FILENO);
		}
		// set ProcessGroupID(PGID)
		if ( setpgid(0,pPGID)==-1 ) {
			printf("Failed to set PGID\n");
		}
		if ( getpid()==getpgrp() ) {
			signal(SIGTTOU, SIG_IGN);
			if ( tcsetpgrp(STDIN_FILENO,getpgrp())==-1 ) { // to interupt process by ctrl-c.
				printf("failed to tcsetpgrp\n");
			}
			signal(SIGTTOU, SIG_DFL);
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

