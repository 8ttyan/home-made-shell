#pragma once

#include <string>
using namespace std;

enum class TokenType {
	None,
	EoS,
	Pipe,
	Or,
	SubShell,
	BackGround,
	And,
	Redirect,
	Dup,
	Word,
	Comment,
};

string TokenTypeToString(TokenType pToken);

