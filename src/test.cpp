
#include <algorithm>	// std::transform
#include "test.h"
#include "prompter.h"
#include "lexicaltokenizer.h"

Test::Test(const string& pMode)
{
	string mode=pMode;	// prepare same character size
	std::transform(pMode.begin(), pMode.end(), mode.begin(), [](char c){return tolower(c);});
	printf("mode=%s\n",mode.c_str());
	if ( mode=="prompter" || mode=="prompt" ) {
		TestPrompter();
		return;
	}
	if ( mode=="token" || mode=="tokenizer" ) {
		TestLexicalTokenizer();
		return;
	}
	printf("Unknown test mode '%s'\n", pMode.c_str());
}

void Test::TestPrompter()
{
	Prompter prompter(stdin,stdout);
	char c;
	while ( prompter>>c ) {
		printf("char: %c\n",c);
	}
}
void Test::TestLexicalTokenizer()
{
	while (1) {
		Prompter prompter(stdin,stdout);
		LexicalTokenizer lt(prompter);
		string token;
		while ( lt>>token ) {
			printf("token: %s\n", token.c_str());
		}
	}
}

