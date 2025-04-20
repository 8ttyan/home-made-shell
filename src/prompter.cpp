
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "prompter.h"

Prompter::Prompter()
{
	mStdinFp = stdin;
	mStdoutFp = stdout;
	mBuf[0] = '\0';
	mPos = 0;
	mFirst = true;
	setSignalHandler();
}
void Prompter::newLine()
{
	fprintf(mStdoutFp,"home-made-shell> ");
	fflush(mStdoutFp);
	mFirst = true;
}
bool Prompter::operator >> (char& c)
{
	c = mBuf[mPos++];
	if ( c=='\0' ) {
		if ( mFirst ) {
			mFirst = false;
		} else {
			fprintf(mStdoutFp,"> ");
			fflush(mStdoutFp);
		}
		if ( fgets(mBuf,D_BUFSIZE,mStdinFp)==NULL ) return false;
		mPos = 0;
		c = mBuf[mPos++];
	}
	return true;
}

void signalHandler(int signo)
{
	// nothing to do
}

void Prompter::setSignalHandler()
{
	struct sigaction act = {0};
	act.sa_handler = signalHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);
}

