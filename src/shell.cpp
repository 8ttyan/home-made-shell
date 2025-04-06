
#include "shell.h"

Sentence& Shell::appendSentence()
{
	mSentenceList.push_back( Sentence() );
	return mSentenceList.back();
}
Sentence& Shell::back()
{
	return mSentenceList.back();
}
int Shell::exec()
{
	int status=0;
	for (auto& sent : mSentenceList) {
		status = sent.exec();
	}
	return status;
}
