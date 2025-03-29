
#include "lexicaltokenizer.h"

LexicalTokenizer::LexicalTokenizer(Prompter& pTarget)
: mTarget(pTarget)
{
	mState = AutomatonState::Init;
	mCurrentChar = '\0';
}

struct Transition
{
	AutomatonState currentState;
	AutomatonState nextState;
	const char *conditionCharList;
	bool exceptFlag;
	bool appendPrevChar;

	bool isMatch(const char c) const
	{
		for (int i=0; ; i++) {
			if ( conditionCharList[i]=='\0' ) break;
			if ( conditionCharList[i]==c ) {
				if ( exceptFlag ) {
					return false;
				} else {
					return true;
				}
			}
		}
		if ( exceptFlag ) {
			return true;
		} else {
			return false;
		}
	};
};

#define __sd "><&|;\n ()'\"#"

Transition TransitionTable[] = {
	{AutomatonState::Init, AutomatonState::Init, " \t", false, false},
	{AutomatonState::Init, AutomatonState::EoS, ";", false, false},
	{AutomatonState::Init, AutomatonState::Pipe, "|", false, false},
	{AutomatonState::Pipe, AutomatonState::Or, "|", false, true},
	{AutomatonState::Init, AutomatonState::Subshell, "()", false, false},
	{AutomatonState::Init, AutomatonState::BackGround, "&", false, false},
	{AutomatonState::BackGround, AutomatonState::And, "&", false, true},
	{AutomatonState::BackGround, AutomatonState::Write12, ">", false, true},
	{AutomatonState::Init, AutomatonState::Read, "<", false, false},
	{AutomatonState::Init, AutomatonState::Write, ">", false, false},
	{AutomatonState::Write, AutomatonState::Append, ">", false, true},
	{AutomatonState::Write, AutomatonState::Specify, "&", false, true},
	{AutomatonState::Specify, AutomatonState::Dup, "12", false, true},
	{AutomatonState::Init, AutomatonState::Digit, "12", false, false},
	{AutomatonState::Digit, AutomatonState::Write, ">", false, true},
	{AutomatonState::Digit, AutomatonState::CmdArgs, __sd, true, true},
	{AutomatonState::Init, AutomatonState::CmdArgs, __sd, true, false},
	{AutomatonState::CmdArgs, AutomatonState::CmdArgs, __sd, true, true},
	{AutomatonState::CmdArgs, AutomatonState::Escape, "\\", false, true},
	{AutomatonState::Escape, AutomatonState::CmdArgs, "", true, false},
	{AutomatonState::Init, AutomatonState::Escape, "\\", false, false},
	{AutomatonState::Init, AutomatonState::Comment, "#", false, false},
	{AutomatonState::Comment, AutomatonState::Comment, "\n", true, true},
	{AutomatonState::Init, AutomatonState::SQ, "'", false, false},
	{AutomatonState::SQ, AutomatonState::SQ, "\\", true, false},
	{AutomatonState::SQ, AutomatonState::SqEsc, "\\", false, true},
	{AutomatonState::Init, AutomatonState::DQ, "\"", false, false},
	{AutomatonState::DQ, AutomatonState::DQ, "\\", true, false},
	{AutomatonState::DQ, AutomatonState::DqEsc, "\\", false, true},

	// Define Final-State as transition
	{AutomatonState::EoS, AutomatonState::Final, ";", true, true},
	{AutomatonState::Pipe, AutomatonState::Final, "|", true, true},
	{AutomatonState::Subshell, AutomatonState::Final, "", true, true},
	{AutomatonState::BackGround, AutomatonState::Final, "&>", true, true},
	{AutomatonState::And, AutomatonState::Final, "", true, true},
	{AutomatonState::Write12, AutomatonState::Final, "", true, true},
	{AutomatonState::Read, AutomatonState::Final, "", true, true},
	{AutomatonState::Write, AutomatonState::Final, ">&", true, true},
	{AutomatonState::Append, AutomatonState::Final, "", true, true},
	{AutomatonState::Dup, AutomatonState::Final, "", true, true},
	{AutomatonState::Digit, AutomatonState::Final, ">" __sd, true, true},
	{AutomatonState::CmdArgs, AutomatonState::Final, "\\" __sd, false, true},
	{AutomatonState::Comment, AutomatonState::Final, "\n", false, true},
	{AutomatonState::SQ, AutomatonState::Final, __sd, true, true},
	{AutomatonState::DQ, AutomatonState::Final, __sd, true, true},
};

Transition __transitionNullObject = { AutomatonState::None, AutomatonState::None, "", false, false };

const Transition& getMatchedTransition(AutomatonState pState, char pC)
{
	for (const Transition& trans : TransitionTable) {
		if ( pState!=trans.currentState ) continue;
		if ( trans.isMatch(pC) ) return trans;
	}
	return __transitionNullObject;
}

bool LexicalTokenizer::operator >> (string& pToken)
{
	string token;
	if ( mCurrentChar=='\0' ) {
		mTarget >> mCurrentChar;
	}
	if ( mState==AutomatonState::Init && mCurrentChar=='\n' ) {
		mCurrentChar = '\0';
		return false;
	}
	char prevChar='\0';
	while ( mCurrentChar!='\0' ) {
		//printf("prev=%c cur=%c token=%s\n", prevChar, mCurrentChar, token.c_str());
		Transition trans = getMatchedTransition(mState,mCurrentChar);
		if ( trans.currentState==AutomatonState::None ) {
			printf("None\n");
			return false;
		}
		mState = trans.nextState;
		if ( prevChar!='\0' && trans.appendPrevChar ) {
			//printf("append %c to %s", prevChar, token.c_str());
			token += prevChar;
			//printf(" results=%s\n",token.c_str());
		}
		if ( mState==AutomatonState::Final ) {
			// Do not ignore mCurrentChar because the final state is equal to next token's init state.
			pToken = token;
			mState = AutomatonState::Init;
			return true;
		}
		prevChar = mCurrentChar;
		mTarget >> mCurrentChar;
	}

	return true;
}

