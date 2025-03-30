
#include "lexicaltokenizer.h"
#include "automatontransition.h"
#include "token.h"
#include "statetypemapping.h"

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
		return false;
	}
	char prevChar='\0';
	while ( mCurrentChar!='\0' ) {
		//printf("prev=%c cur=%c token=%s\n", prevChar, mCurrentChar, token.c_str());
		AutomatonTransition trans = getMatchedTransition(mState,mCurrentChar);
		if ( trans.currentState==AutomatonState::None ) {
			return false;
		}
		if ( prevChar!='\0' && trans.appendPrevChar ) {
			token += prevChar;
		}
		if ( trans.nextState==AutomatonState::Final ) {
			for (const StateTypeMapping& stm : StateTypeMap) {
				if ( stm.automatonState==mState ) {
					token.setType( stm.tokenType );
					break;
				}
			}
			pToken = token;
			mState = AutomatonState::Init;
			return true;
		}
		mState = trans.nextState;
		prevChar = mCurrentChar;
		mTarget >> mCurrentChar;
	}
	return true;
}

