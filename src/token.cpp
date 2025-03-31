
#include "token.h"

Token::Token()
{
}
Token::Token(const string& pStr, TokenType pType)
: string(pStr)
, mType(pType)
{
}
Token::Token(const char* pChar, TokenType pType)
: string(pChar)
, mType(pType)
{
}
void Token::setType(const TokenType& pType)
{
	mType = pType;
}

TokenType Token::type()
{
	return mType;
}

void Token::clear()
{
	string::clear();
	mType = TokenType::None;
}

