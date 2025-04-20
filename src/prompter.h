
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
	static Prompter& getInstance()
	{
		static Prompter __instance;
		return __instance;
	};
	Prompter(const Prompter&) = delete;
	Prompter& operator=(const Prompter&) = delete;
	void newLine();
	bool operator >> (char& c);	//!< get character from stdin

private:
	Prompter();	// singleton
	void setSignalHandler();
private:
	FILE* mStdinFp;
	FILE* mStdoutFp;
	char mBuf[D_BUFSIZE];
	size_t mPos;
	bool mFirst;
};

