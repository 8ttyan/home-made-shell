#pragma once

#include <list>
using namespace std;
#include "processgroup.h"

class Sentence
{
public:
	enum class Condition
	{
		AND,
		OR,
	};
	enum class WaitType
	{
		ForeGround,
		BackGround,
	};

	Sentence();
	ProcessGroup& appendProcessGroup();
	void appendCondition(Condition);
	void setWaitType(WaitType);
	int exec();

private:
	WaitType mWaitType;
	list<ProcessGroup> mPGList;
	list<Condition> mCondList;
};

