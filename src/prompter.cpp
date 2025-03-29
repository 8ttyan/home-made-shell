
#include <unistd.h>
#include <stdio.h>

#include "prompter.h"

Prompter::Prompter(FILE* pStdinFp, FILE* pStdoutFp)
{
	mStdinFp = pStdinFp;
	mStdoutFp = pStdoutFp;
	mBuf[0] = '\0';
	mPos = 0;
	mFirst = true;

	fprintf(mStdoutFp,"home-made-shell> ");
	fflush(mStdoutFp);
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

