#include "parser.h"
#include "stringtree.h"
#include "lexicaltokenizer.h"
#include "shell.h"
#include "sentence.h"
#include "processgroup.h"
#include "process.h"

Parser::Parser()
{
}

void Parser::run(LexicalTokenizer& pTokenizer, Shell* pShell, bool pPrintTree/*=false*/)
{
	if ( ++pTokenizer==false ) return;
	StringTree shellST = "Parser";
	Line(pTokenizer, pShell, &shellST);
	if (pPrintTree) shellST.print();
}
void Parser::Line(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Line>");
	if ( pTokenizer->type()!=TokenType::Comment ) {
		_Shell(pTokenizer, pShell, &myST);
	}
	if ( pTokenizer->type()==TokenType::Comment ) {
		myST.push_back("<Comment>").push_back(*pTokenizer);
		++pTokenizer;
		return;
	}
}
void Parser::_Shell(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Shell>");
	while (1) {
		_Sentence(pTokenizer, pShell ,&myST);
		if ( pTokenizer->type()==TokenType::EoS ) {
			myST.push_back(";");
			++pTokenizer;
		} else if ( pTokenizer->type()==TokenType::BackGround ) {
			pShell->back().setWaitType( Sentence::WaitType::BackGround );
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
void Parser::_Sentence(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	Sentence& sent=pShell->appendSentence();
	StringTree& myST=pParentST->push_back("<Sentence>");
	while (1) {
		_ProcessGroup(pTokenizer, &sent, &myST);
		if ( pTokenizer->type()==TokenType::And ) {
			sent.appendCondition( Sentence::Condition::AND );
			myST.push_back("&&");
			++pTokenizer;
			continue;
		}
		if ( pTokenizer->type()==TokenType::Or ) {
			sent.appendCondition( Sentence::Condition::OR );
			myST.push_back("||");
			++pTokenizer;
			continue;
		}
		break;
	}
}
void Parser::_ProcessGroup(LexicalTokenizer& pTokenizer, Sentence* pSentence, StringTree* pParentST)
{
	ProcessGroup& pg=pSentence->appendProcessGroup();
	StringTree& myST=pParentST->push_back("<ProcessGroup>");
	while (1) {
		_Process(pTokenizer, &pg, &myST);
		if ( pTokenizer->type()!=TokenType::Pipe ) break;
		myST.push_back("<pipe>");
		++pTokenizer;
	}
}
void Parser::_Process(LexicalTokenizer& pTokenizer, ProcessGroup* pPG, StringTree* pParentST)
{
	Process& proc = pPG->appendProcess();
	StringTree& myST=pParentST->push_back("<Process>");
	if ( pTokenizer->type()==TokenType::SubShellBegin ) {
		myST.push_back("<SubShellBegin>");
		++pTokenizer;
		_Shell(pTokenizer,NULL,&myST);
		if ( pTokenizer->type()!=TokenType::SubShellEnd ) {
			printf("error");
		} else {
			myST.push_back("<SubShellEnd>");
			++pTokenizer;
		}
	} else {
		_Command(pTokenizer, &proc, &myST);
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
void Parser::_Command(LexicalTokenizer& pTokenizer, Process* pProc, StringTree* pParentST)
{
	vector<string> args;
	StringTree& myST=pParentST->push_back("<Command>");
	do {
		args.push_back(*pTokenizer);
		StringTree& word = myST.push_back("<Word>");
		word.push_back(*pTokenizer);
		++pTokenizer;
	} while ( pTokenizer->type()==TokenType::Word );
	pProc->setArgs(args);
}

