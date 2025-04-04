
#include "lexicaltokenizer.h"
#include "automatontransition.h"
#include "token.h"
#include "statetypemapping.h"

LexicalTokenizer::LexicalTokenizer(Prompter& pTarget)
: mTarget(pTarget)
, mValid(true)
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
	operator++();
	pToken = mCurrentToken;
	return mValid;
}

bool LexicalTokenizer::operator ++ ()
{
	if ( mValid==false ) return false;
	mCurrentToken.clear();
	if ( mCurrentChar=='\0' ) {
		mTarget >> mCurrentChar;
	}
	if ( mState==AutomatonState::Init && mCurrentChar=='\n' ) {
		mCurrentChar = '\0';
		mValid = false;
		return false;
	}
	char prevChar='\0';
	while ( mCurrentChar!='\0' ) {
		//printf("prev=%c cur=%c mCurrentToken=%s\n", prevChar, mCurrentChar, mCurrentToken.c_str());
		AutomatonTransition trans = getMatchedTransition(mState,mCurrentChar);
		if ( trans.currentState==AutomatonState::None ) {
			mValid = false;
			return false;
		}
		if ( prevChar!='\0' && trans.appendPrevChar ) {
			mCurrentToken += prevChar;
		}
		if ( trans.nextState==AutomatonState::Final ) {
			for (const StateTypeMapping& stm : StateTypeMap) {
				if ( stm.automatonState==mState ) {
					mCurrentToken.setType( stm.tokenType );
					break;
				}
			}
			mState = AutomatonState::Init;
			return true;
		}
		mState = trans.nextState;
		prevChar = mCurrentChar;
		mTarget >> mCurrentChar;
	}
	return true;
}

Token LexicalTokenizer::operator * () const
{
	return mCurrentToken;
}
Token* LexicalTokenizer::operator ->()
{
	return &mCurrentToken;
}
const Token* LexicalTokenizer::operator ->() const
{
	return &mCurrentToken;
}
bool LexicalTokenizer::valid() const
{
	return mValid;
}

