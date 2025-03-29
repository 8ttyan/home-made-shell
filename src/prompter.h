
#pragma once
#define D_BUFSIZE 1024

/*!
 *	@class Prompter
 	@brief Read stream and automatically write prompt.
 	@details At first, Stream shows prompt like "home-made-shell>".
	When you call getc() after getc() returns '\n', 
	the Stream shows continuation-line-prompt '>'.
 */
class Prompter
{
public:
	Prompter(FILE* pStdinFp, FILE* pStdoutFp);
	bool operator >> (char& c);

private:
	FILE* mStdinFp;
	FILE* mStdoutFp;
	char mBuf[D_BUFSIZE];
	size_t mPos;
	bool mFirst;
};

