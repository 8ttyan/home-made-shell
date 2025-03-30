
#pragma once
#include <string>
using namespace std;
#include "tokentype.h"

class Token : public string
{
public:
	Token();
	Token(const string& pStr, TokenType pType=TokenType::None);
	Token(const char* pChar, TokenType pType=TokenType::None);
	void setType(const TokenType&);
	TokenType type();

private:
	TokenType mType;
};
