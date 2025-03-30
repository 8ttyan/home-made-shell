
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
	MatchCondition condition;
	bool appendPrevChar;

	bool isMatch(const char c) const
	{
		for (int i=0; ; i++) {
			if ( conditionCharList[i]=='\0' ) break;
			if ( conditionCharList[i]==c ) {
				if ( condition==MatchCondition::Include ) {
					return true;
				} else {
					return false;
				}
			}
		}
		if ( condition==MatchCondition::Include ) {
			return false;
		} else {
			return true;
		}
	};
};

#define __sd "><&|;\n ()'\"#"

Transition TransitionTable[] = {
	{AutomatonState::Init, AutomatonState::Init, " \t", MatchCondition::Include, false},
	{AutomatonState::Init, AutomatonState::EoS,  ";",   MatchCondition::Include, false},
		{AutomatonState::EoS, AutomatonState::Final, "", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::Pipe, "|", MatchCondition::Include, false},
		{AutomatonState::Pipe, AutomatonState::Final, "", MatchCondition::Except, true},
		{AutomatonState::Pipe, AutomatonState::Or,   "|", MatchCondition::Include, true},
			{AutomatonState::Or, AutomatonState::Final,   "", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::Subshell, "()", MatchCondition::Include, false},
		{AutomatonState::Subshell, AutomatonState::Final, "", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::BackGround, "&", MatchCondition::Include, false},
		{AutomatonState::BackGround, AutomatonState::Final, "&>", MatchCondition::Except, true},
		{AutomatonState::BackGround, AutomatonState::And, "&", MatchCondition::Include, true},
			{AutomatonState::And, AutomatonState::Final, "", MatchCondition::Except, true},
		{AutomatonState::BackGround, AutomatonState::Write12, ">", MatchCondition::Include, true},
			{AutomatonState::Write12, AutomatonState::Final, "", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::Read, "<", MatchCondition::Include, false},
		{AutomatonState::Read, AutomatonState::Final, "", MatchCondition::Except, false},
	{AutomatonState::Init, AutomatonState::Write, ">", MatchCondition::Include, false},
		{AutomatonState::Write, AutomatonState::Final, "", MatchCondition::Except, false},
		{AutomatonState::Write, AutomatonState::Append, ">", MatchCondition::Include, true},
			{AutomatonState::Append, AutomatonState::Final, "", MatchCondition::Except, true},
		{AutomatonState::Write, AutomatonState::Specify, "&", MatchCondition::Include, true},
			{AutomatonState::Specify, AutomatonState::Dup, "12", MatchCondition::Include, true},
				{AutomatonState::Dup, AutomatonState::Final, "", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::Digit, "12", MatchCondition::Include, false},
		{AutomatonState::Digit, AutomatonState::Write, ">", MatchCondition::Include, true},
		{AutomatonState::Digit, AutomatonState::CmdArgs, __sd, MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::CmdArgs, __sd, MatchCondition::Except, false},
		{AutomatonState::CmdArgs, AutomatonState::Final, __sd "\\", MatchCondition::Include, true},
		{AutomatonState::CmdArgs, AutomatonState::CmdArgs, __sd "\\", MatchCondition::Except, true},
		{AutomatonState::CmdArgs, AutomatonState::Escape, "\\", MatchCondition::Include, true},
	{AutomatonState::Init, AutomatonState::Escape, "\\", MatchCondition::Include, false},
		{AutomatonState::Escape, AutomatonState::CmdArgs, "\n", MatchCondition::Except, true},
		{AutomatonState::Escape, AutomatonState::IgnoreNL, "\n", MatchCondition::Include, true},
			{AutomatonState::IgnoreNL, AutomatonState::Escape, "\\", MatchCondition::Include, true},
			{AutomatonState::IgnoreNL, AutomatonState::CmdArgs, __sd, MatchCondition::Except, true},
			{AutomatonState::IgnoreNL, AutomatonState::Final, __sd, MatchCondition::Include, false},
	{AutomatonState::Init, AutomatonState::Comment, "#", MatchCondition::Include, false},
		{AutomatonState::Comment, AutomatonState::Final, "\n", MatchCondition::Include, true},
		{AutomatonState::Comment, AutomatonState::Comment, "\n", MatchCondition::Except, true},
	{AutomatonState::Init, AutomatonState::BeginSingleQuate, "'", MatchCondition::Include, false},
		{AutomatonState::BeginSingleQuate, AutomatonState::InSingleQuate, "'", MatchCondition::Except, false},
		{AutomatonState::InSingleQuate, AutomatonState::EndSingleQuate, "'", MatchCondition::Include, true},
			{AutomatonState::EndSingleQuate, AutomatonState::Final, "", MatchCondition::Except, false},
		{AutomatonState::InSingleQuate, AutomatonState::InSingleQuate, "'\\", MatchCondition::Except, true},
		{AutomatonState::InSingleQuate, AutomatonState::EscapeSingleQuate, "\\", MatchCondition::Include, true},
			{AutomatonState::EscapeSingleQuate, AutomatonState::InSingleQuate, "'", MatchCondition::Include, false},
			{AutomatonState::EscapeSingleQuate, AutomatonState::InSingleQuate, "'", MatchCondition::Except, true},
		{AutomatonState::BeginSingleQuate, AutomatonState::EndSingleQuate, "'", MatchCondition::Include, false},
	{AutomatonState::Init, AutomatonState::BeginDoubleQuate, "\"", MatchCondition::Include, false},
		{AutomatonState::BeginDoubleQuate, AutomatonState::InDoubleQuate, "\"", MatchCondition::Except, false},
		{AutomatonState::InDoubleQuate, AutomatonState::EndDoubleQuate, "\"", MatchCondition::Include, true},
			{AutomatonState::EndDoubleQuate, AutomatonState::Final, "", MatchCondition::Except, false},
		{AutomatonState::InDoubleQuate, AutomatonState::InDoubleQuate, "\"\\", MatchCondition::Except, true},
		{AutomatonState::InDoubleQuate, AutomatonState::EscapeDoubleQuate, "\\", MatchCondition::Include, true},
			{AutomatonState::EscapeDoubleQuate, AutomatonState::InDoubleQuate, "\"", MatchCondition::Include, false},
			{AutomatonState::EscapeDoubleQuate, AutomatonState::InDoubleQuate, "\"", MatchCondition::Except, true},
		{AutomatonState::BeginDoubleQuate, AutomatonState::EndDoubleQuate, "\"", MatchCondition::Include, false},
};

Transition __transitionNullObject = { AutomatonState::None, AutomatonState::None, "", MatchCondition::Except, false };

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

