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
	Line(pTokenizer, &shellST);
	shellST.print();
}
void Parser::Line(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Line>");
	if ( pTokenizer->type()!=TokenType::Comment ) {
		Shell(pTokenizer,&myST);
	}
	if ( pTokenizer->type()==TokenType::Comment ) {
		myST.push_back("<Comment>").push_back(*pTokenizer);
		++pTokenizer;
		return;
	}
}
void Parser::Shell(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Shell>");
	while (1) {
		Sentence(pTokenizer,&myST);
		if ( pTokenizer->type()==TokenType::EoS ) {
			myST.push_back(";");
			++pTokenizer;
		} else if ( pTokenizer->type()==TokenType::BackGround ) {
			myST.push_back("&");
			++pTokenizer;
		}
		if ( pTokenizer->type()==TokenType::Comment ) {
			break;
		}
		if ( pTokenizer->type()==TokenType::SubShellEnd ) {
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
}
void Parser::ProcessGroup(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<ProcessGroup>");
	while (1) {
		Process(pTokenizer, &myST);
		if ( pTokenizer->type()!=TokenType::Pipe ) break;
		myST.push_back("<pipe>");
		++pTokenizer;
	}
}
void Parser::Process(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Process>");
	if ( pTokenizer->type()==TokenType::SubShellBegin ) {
		myST.push_back("<SubShellBegin>");
		++pTokenizer;
		Shell(pTokenizer,&myST);
		if ( pTokenizer->type()!=TokenType::SubShellEnd ) {
			printf("error");
		} else {
			myST.push_back("<SubShellEnd>");
			++pTokenizer;
		}
	} else {
		Command(pTokenizer,&myST);
	}
	while ( pTokenizer->type()==TokenType::Redirect || pTokenizer->type()==TokenType::Dup ) {
		if ( pTokenizer->type()==TokenType::Redirect ) {
			myST.push_back("<Redirect>").push_back(*pTokenizer);
			++pTokenizer;
			myST.push_back("<Word>").push_back(*pTokenizer);
			++pTokenizer;
		} else {
			myST.push_back("<Dup>").push_back(*pTokenizer);
			++pTokenizer;
		}
	}
}
void Parser::Command(LexicalTokenizer& pTokenizer, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Command>");
	do {
		StringTree& word = myST.push_back("<Word>");
		word.push_back(*pTokenizer);
		++pTokenizer;
	} while ( pTokenizer->type()==TokenType::Word );
}

