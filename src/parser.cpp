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

bool Parser::run(LexicalTokenizer& pTokenizer, Shell* pShell, bool pPrintTree/*=false*/)
{
	if ( ++pTokenizer==false ) return false;
	StringTree shellST = "Parser";
	if ( Line(pTokenizer,pShell,&shellST)==false ) return false;
	if (pPrintTree) shellST.print();
	return true;
}
bool Parser::Line(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Line>");
	if ( pTokenizer->type()!=TokenType::Comment ) {
		if ( _Shell(pTokenizer,pShell,&myST)==false ) return false;
	}
	if ( pTokenizer.valid()==false ) return true;
	if ( pTokenizer->type()==TokenType::Comment ) {
		myST.push_back("<Comment>").push_back(*pTokenizer);
		++pTokenizer;
		return true;
	} else {
		return false;
	}
	return true;
}
bool Parser::_Shell(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	StringTree& myST=pParentST->push_back("<Shell>");
	while (1) {
		if ( _Sentence(pTokenizer,pShell,&myST)==false ) return false;
		if ( pTokenizer.valid()==false ) break;
		if ( pTokenizer->type()==TokenType::SubShellEnd ) break;
		if ( pTokenizer->type()==TokenType::Comment ) break;
	}
	return true;
}
bool Parser::_Sentence(LexicalTokenizer& pTokenizer, Shell* pShell, StringTree* pParentST)
{
	Sentence& sent=pShell->appendSentence();
	StringTree& myST=pParentST->push_back("<Sentence>");
	while (1) {
		if ( _ProcessGroup(pTokenizer,&sent,&myST)==false ) return false;
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
	if ( pTokenizer->type()==TokenType::EoS ) {
		myST.push_back(";");
		++pTokenizer;
	} else if ( pTokenizer->type()==TokenType::BackGround ) {
		sent.setWaitType( Sentence::WaitType::BackGround );
		myST.push_back("&");
		++pTokenizer;
	}
	return true;
}
bool Parser::_ProcessGroup(LexicalTokenizer& pTokenizer, Sentence* pSentence, StringTree* pParentST)
{
	ProcessGroup& pg=pSentence->appendProcessGroup();
	StringTree& myST=pParentST->push_back("<ProcessGroup>");
	while (1) {
		if ( _Process(pTokenizer,&pg,&myST)==false ) return false;
		if ( pTokenizer->type()!=TokenType::Pipe ) break;
		myST.push_back("<pipe>");
		++pTokenizer;
	}
	return true;
}
bool Parser::_Process(LexicalTokenizer& pTokenizer, ProcessGroup* pPG, StringTree* pParentST)
{
	Process& proc = pPG->appendProcess();
	StringTree& myST=pParentST->push_back("<Process>");
	if ( pTokenizer->type()==TokenType::SubShellBegin ) {
		myST.push_back("<SubShellBegin>");
		++pTokenizer;
		if ( _Shell(pTokenizer,NULL,&myST)==false ) return false;
		if ( pTokenizer->type()!=TokenType::SubShellEnd ) {
			printf("error");
		} else {
			myST.push_back("<SubShellEnd>");
			++pTokenizer;
		}
	} else {
		if ( _Command(pTokenizer,&proc,&myST)==false ) return false;
	}
	while ( pTokenizer->type()==TokenType::Redirect || pTokenizer->type()==TokenType::Dup ) {
		if ( pTokenizer->type()==TokenType::Redirect ) {
			int fileNo = STDOUT_FILENO;
			bool appendFlg = false;
			if ( *pTokenizer=="<" )  { fileNo=STDIN_FILENO;  appendFlg=false; }
			if ( *pTokenizer==">" )  { fileNo=STDOUT_FILENO; appendFlg=false; }
			if ( *pTokenizer=="1>" ) { fileNo=STDOUT_FILENO; appendFlg=false; }
			if ( *pTokenizer=="2>" ) { fileNo=STDERR_FILENO; appendFlg=false; }
			//if ( *pTokenizer=="<" )  { fileNo=STDIN_FILENO;  appendFlg=false; } // here document
			if ( *pTokenizer==">>" )  { fileNo=STDOUT_FILENO; appendFlg=true; }
			if ( *pTokenizer=="1>>" ) { fileNo=STDOUT_FILENO; appendFlg=true; }
			if ( *pTokenizer=="2>>" ) { fileNo=STDERR_FILENO; appendFlg=true; }
			myST.push_back("<Redirect>").push_back(*pTokenizer);
			++pTokenizer;
			myST.push_back("<Word>").push_back(*pTokenizer);
			string filePath = *pTokenizer;
			++pTokenizer;
			proc.redirect(fileNo, filePath, appendFlg);
		} else {
			myST.push_back("<Dup>").push_back(*pTokenizer);
			DUPLICATE dup=DUPLICATE::NONE;
			if ( *pTokenizer==">&2" ) dup=DUPLICATE::STDERR_TO_STDOUT;
			if ( *pTokenizer=="1>&2" ) dup=DUPLICATE::STDERR_TO_STDOUT;
			if ( *pTokenizer=="2>&1" ) dup=DUPLICATE::STDOUT_TO_STDERR;
			proc.duplicate(dup);
			++pTokenizer;
		}
	}
	return true;
}
bool Parser::_Command(LexicalTokenizer& pTokenizer, Process* pProc, StringTree* pParentST)
{
	vector<string> args;
	StringTree& myST=pParentST->push_back("<Command>");
	if ( pTokenizer->type()!=TokenType::Word ) return false;
	do {
		args.push_back(*pTokenizer);
		StringTree& word = myST.push_back("<Word>");
		word.push_back(*pTokenizer);
		++pTokenizer;
	} while ( pTokenizer->type()==TokenType::Word );
	pProc->setArgs(args);
	return true;
}

