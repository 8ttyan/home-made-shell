
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
	IgnoreNL,
	Comment,
	BeginSingleQuate,
	InSingleQuate,
	EndSingleQuate,
	EscapeSingleQuate,
	BeginDoubleQuate,
	InDoubleQuate,
	EndDoubleQuate,
	EscapeDoubleQuate,
};

enum class Match
{
	OneOf,
	Except,
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

