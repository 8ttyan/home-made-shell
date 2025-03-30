
#include "lexicaltokenizer.h"
#include "automatontransition.h"
#include "token.h"

LexicalTokenizer::LexicalTokenizer(Prompter& pTarget)
: mTarget(pTarget)
{
	mState = AutomatonState::Init;
	mCurrentChar = '\0';
}

const AutomatonTransition& getMatchedTransition(AutomatonState pState, char pC)
{
	for (const AutomatonTransition& trans : TransitionTable) {
		if ( pState!=trans.currentState ) continue;
		if ( trans.isMatch(pC) ) return trans;
	}
	return __transitionNullObject;
}

bool LexicalTokenizer::operator >> (Token& pToken)
{
	Token token;
	if ( mCurrentChar=='\0' ) {
		mTarget >> mCurrentChar;
	}
	if ( mState==AutomatonState::Init && mCurrentChar=='\n' ) {
		mCurrentChar = '\0';
		//puts("end 1");
		return false;
	}
	char prevChar='\0';
	while ( mCurrentChar!='\0' ) {
		//printf("prev=%c cur=%c token=%s\n", prevChar, mCurrentChar, token.c_str());
		AutomatonTransition trans = getMatchedTransition(mState,mCurrentChar);
		if ( trans.currentState==AutomatonState::None ) {
			//puts("end 2");
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
			//puts("end 3");
			return true;
		}
		prevChar = mCurrentChar;
		mTarget >> mCurrentChar;
	}

	//puts("end 4");
	return true;
}

