#pragma once

#include <list>
using namespace std;
#include "sentence.h"

class Shell
{
public:

	Sentence& appendSentence();
	Sentence& back();
	int exec();
private:
	list<Sentence> mSentenceList;
};

