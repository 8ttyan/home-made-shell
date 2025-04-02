#include "parser.h"
#include "stringtree.h"
#include "lexicaltokenizer.h"

Parser::Parser()
{
}

void Parser::run(LexicalTokenizer& pTokenizer)
{
	++pTokenizer;
	StringTree shellST = "Parser";
	Shell(pTokenizer, &shellST);
	shellST.print();
}
void Parser::Shell(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Shell>");
	while (1) {
		Sentence(pTokenizer,&myST);
		TokenType type=pTokenizer->type();
		if ( type==TokenType::EoS || type==TokenType::BackGround ) {
			++pTokenizer;
		}
		if ( type==TokenType::Comment ) break;
		if ( type==TokenType::None ) break;
	}
}
void Parser::Sentence(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Sentence>");
	++pTokenizer;
}
