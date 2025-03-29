
#pragma once
#include <string>
using namespace std;

class Tokenizer
{
public:
	Tokenizer(const string& pTarget, const string& pDelimiters);
	bool operator >> (string& pToken);

private:
	bool isDelimiter(const char pChar);
private:
	string mTarget;
	size_t mPos;
	string mDelimiters;

};

