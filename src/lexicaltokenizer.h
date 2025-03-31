
#pragma once

#include <string>
using namespace std;
#include "prompter.h"
#include "automatonstate.h"
#include "token.h"

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
	bool operator ++ ();
//	bool operator ++ (int);
	Token operator * () const;

private:
	Prompter& mTarget;
	AutomatonState mState;
	char mCurrentChar;
	Token mCurrentToken;
};

