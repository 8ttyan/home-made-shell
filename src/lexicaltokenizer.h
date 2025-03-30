
#pragma once

#include <string>
using namespace std;
#include "prompter.h"
#include "automatonstate.h"
class Token;

enum class Match
{
	OneOf,
	Except,
};

class LexicalTokenizer
{
public:
	LexicalTokenizer(Prompter& pTarget);
	bool operator >> (Token& pToken);

private:
	Prompter& mTarget;
	AutomatonState mState;
	char mCurrentChar;
};

