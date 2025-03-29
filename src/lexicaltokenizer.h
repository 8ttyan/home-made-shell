
#pragma once

#include <string>
using namespace std;
#include "prompter.h"

enum class AutomatonState
{
	// spacial state
	None,
	Init,
	Final,

	// normal state
	EoS,
	Pipe,
	And,
	Or,
	Subshell,
	BackGround,
	Write12,
	Read,
	Write,
	Append,
	Specify,
	Dup,
	Digit,
	CmdArgs,
	Escape,
	Comment,
	SQ,
	SqEsc,
	DQ,
	DqEsc,
};

class LexicalTokenizer
{
public:
	LexicalTokenizer(Prompter& pTarget);
	bool operator >> (string& pToken);

private:
	Prompter& mTarget;
	AutomatonState mState;
	char mCurrentChar;
};

