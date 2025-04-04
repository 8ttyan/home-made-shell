#include "parser.h"
#include "stringtree.h"
#include "lexicaltokenizer.h"

Parser::Parser()
{
}

void Parser::run(LexicalTokenizer& pTokenizer)
{
	if ( ++pTokenizer==false ) return;
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
		if ( pTokenizer->type()==TokenType::Comment ) {
			myST.push_back("<Comment>");
			break;
		}
		if ( pTokenizer.valid()==false ) break;
	}
}
void Parser::Sentence(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Sentence>");
	while (1) {
		ProcessGroup(pTokenizer,&myST);
		if ( pTokenizer->type()==TokenType::And ) {
			myST.push_back("&&");
			++pTokenizer;
			continue;
		}
		if ( pTokenizer->type()==TokenType::Or ) {
			myST.push_back("||");
			++pTokenizer;
			continue;
		}
		break;
	}
	if ( pTokenizer->type()==TokenType::EoS ) {
		myST.push_back(";");
		++pTokenizer;
		return;
	}
	if ( pTokenizer->type()==TokenType::BackGround ) {
		myST.push_back("&");
		++pTokenizer;
		return;
	}
}
void Parser::ProcessGroup(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<ProcessGroup>");
	++pTokenizer;
}

