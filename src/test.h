#pragma once

#include <string>
using namespace std;

class Test
{
public:
	Test(const string& pMode);
private:
	void TestPrompter();
	void TestLexicalTokenizer();
	void TestParser();
};

