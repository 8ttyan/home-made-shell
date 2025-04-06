
#include <algorithm>	// std::transform
#include "test.h"
#include "prompter.h"
#include "lexicaltokenizer.h"
#include "token.h"
#include "parser.h"
#include "shell.h"

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
	if ( mode=="parser" ) {
		TestParser();
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
		Token token;
		while ( lt>>token ) {
			string type=TokenTypeToString( token.type() );
			printf("(TokenType:%s) %s\n", type.c_str(), token.c_str());
		}
	}
}
void Test::TestParser()
{
	while (1) {
		Prompter prompter(stdin,stdout);
		LexicalTokenizer lt(prompter);
		Shell shell;
		Parser parser;
		parser.run(lt, &shell, true);
	}
}

