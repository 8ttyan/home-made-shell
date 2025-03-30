#include "tokentype.h"

string TokenTypeToString(TokenType pToken)
{
	switch (pToken) {
		case TokenType::None: return "None";
		case TokenType::EoS: return "EoS";
		case TokenType::Pipe: return "Pipe";
		case TokenType::Or: return "Or";
		case TokenType::SubShell: return "SubShell";
		case TokenType::BackGround: return "BackGround";
		case TokenType::And: return "And";
		case TokenType::Redirect: return "Redirect";
		case TokenType::Word: return "Word";
		case TokenType::Comment: return "Comment";
	}
	return "Unkown_TokenType";
}

