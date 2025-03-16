
#include "tokenizer.h"

Tokenizer::Tokenizer(const string& pTarget, const string& pDelimiters)
: mTarget(pTarget)
, mPos(0)
, mDelimiters(pDelimiters)
{
}
bool Tokenizer::operator >> (string& pToken)
{
	pToken.clear();
	while ( mPos<mTarget.size() ) {
		char c = mTarget[mPos++];
		if ( isDelimiter(c) ) {
			if ( pToken.empty() ) continue;
			return true;
		}
		pToken += c;
	}
	if ( pToken.empty() ) return false;
	return true;
}
bool Tokenizer::isDelimiter(const char pChar)
{
	if ( mDelimiters.find_first_of(pChar)==string::npos ) return false;
	return true;
}

