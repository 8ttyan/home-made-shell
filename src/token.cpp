
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

TokenType Token::Type()
{
	return mType;
}

