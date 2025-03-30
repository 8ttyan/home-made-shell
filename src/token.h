
#pragma once
#include <string>
using namespace std;

enum class TokenType {
	None,
};

class Token : public string
{
public:
	Token();
	Token(const string& pStr, TokenType pType=TokenType::None);
	Token(const char* pChar, TokenType pType=TokenType::None);
	TokenType Type();

private:
	TokenType mType;
};
