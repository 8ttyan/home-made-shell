#pragma once

#include "automatonstate.h"
#include "token.h"

struct StateTypeMapping
{
	AutomatonState automatonState;
	TokenType tokenType;
};

StateTypeMapping StateTypeMap[] = {
	{AutomatonState::EoS, TokenType::EoS},
	{AutomatonState::Pipe, TokenType::Pipe},
	{AutomatonState::Or, TokenType::Or},
	{AutomatonState::SubShellBegin, TokenType::SubShellBegin},
	{AutomatonState::SubShellEnd, TokenType::SubShellEnd},
	{AutomatonState::BackGround, TokenType::BackGround},
	{AutomatonState::And, TokenType::And},
	{AutomatonState::Write12, TokenType::Redirect},
	{AutomatonState::Read, TokenType::Redirect},
	{AutomatonState::Append, TokenType::Redirect},
	{AutomatonState::Digit, TokenType::Word},
	{AutomatonState::Write, TokenType::Redirect},
	{AutomatonState::Dup, TokenType::Dup},
	{AutomatonState::CmdArgs, TokenType::Word},
	{AutomatonState::EndSingleQuate, TokenType::Word},
	{AutomatonState::EndDoubleQuate, TokenType::Word},
	{AutomatonState::Comment, TokenType::Comment},
};

