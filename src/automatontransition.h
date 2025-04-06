#pragma once

#include "automatonstate.h"

struct AutomatonTransition
{
	AutomatonState currentState;
	AutomatonState nextState;
	Match condition;
	const char *conditionCharList;
	bool appendPrevChar;

	bool isMatch(const char c) const
	{
		for (int i=0; ; i++) {
			if ( conditionCharList[i]=='\0' ) break;
			if ( conditionCharList[i]==c ) {
				if ( condition==Match::OneOf ) {
					return true;
				} else {
					return false;
				}
			}
		}
		if ( condition==Match::OneOf ) {
			return false;
		} else {
			return true;
		}
	};
};

#define __sd "><&|;\n ()'\"#"

AutomatonTransition TransitionTable[] = {
	{AutomatonState::Init, AutomatonState::Init, Match::OneOf, " \t", false},
	{AutomatonState::Init, AutomatonState::EoS,   Match::OneOf,  ";", false},
		{AutomatonState::EoS, AutomatonState::Final, Match::Except, "", true},
	{AutomatonState::Init, AutomatonState::Pipe, Match::OneOf, "|", false},
		{AutomatonState::Pipe, AutomatonState::Final, Match::Except, "|", true},
		{AutomatonState::Pipe, AutomatonState::Or, Match::OneOf,   "|", true},
			{AutomatonState::Or, AutomatonState::Final, Match::Except,   "", true},
	{AutomatonState::Init, AutomatonState::SubShellBegin, Match::OneOf, "(", false},
		{AutomatonState::SubShellBegin, AutomatonState::Final, Match::Except, "", true},
	{AutomatonState::Init, AutomatonState::SubShellEnd, Match::OneOf, ")", false},
		{AutomatonState::SubShellEnd, AutomatonState::Final, Match::Except, "", true},
	{AutomatonState::Init, AutomatonState::BackGround, Match::OneOf, "&", false},
		{AutomatonState::BackGround, AutomatonState::Final, Match::Except, "&>", true},
		{AutomatonState::BackGround, AutomatonState::And, Match::OneOf, "&", true},
			{AutomatonState::And, AutomatonState::Final, Match::Except, "", true},
		{AutomatonState::BackGround, AutomatonState::Write12, Match::OneOf, ">", true},
			{AutomatonState::Write12, AutomatonState::Final, Match::Except, "", true},
	{AutomatonState::Init, AutomatonState::Read, Match::OneOf, "<", false},
		{AutomatonState::Read, AutomatonState::Final, Match::Except, "", false},
	{AutomatonState::Init, AutomatonState::Write, Match::OneOf, ">", false},
		{AutomatonState::Write, AutomatonState::Final, Match::Except, ">&", true},
		{AutomatonState::Write, AutomatonState::Append, Match::OneOf, ">", true},
			{AutomatonState::Append, AutomatonState::Final, Match::Except, "", true},
		{AutomatonState::Write, AutomatonState::Specify, Match::OneOf, "&", true},
			{AutomatonState::Specify, AutomatonState::Dup, Match::OneOf, "12", true},
				{AutomatonState::Dup, AutomatonState::Final, Match::Except, "", true},
	{AutomatonState::Init, AutomatonState::Digit, Match::OneOf, "12", false},
		{AutomatonState::Digit, AutomatonState::Write, Match::OneOf, ">", true},
		{AutomatonState::Digit, AutomatonState::CmdArgs, Match::Except, __sd, true},
	{AutomatonState::Init, AutomatonState::CmdArgs, Match::Except, __sd "\\", false},
		{AutomatonState::CmdArgs, AutomatonState::Final, Match::OneOf, __sd, true},
		{AutomatonState::CmdArgs, AutomatonState::CmdArgs, Match::Except, __sd "\\", true},
		{AutomatonState::CmdArgs, AutomatonState::Escape, Match::OneOf, "\\", true},
	{AutomatonState::Init, AutomatonState::Escape, Match::OneOf, "\\", false},
		{AutomatonState::Escape, AutomatonState::CmdArgs, Match::Except, "\n", false},
		{AutomatonState::Escape, AutomatonState::IgnoreNL, Match::OneOf, "\n", false},
			{AutomatonState::IgnoreNL, AutomatonState::Escape, Match::OneOf, "\\", false},
			{AutomatonState::IgnoreNL, AutomatonState::CmdArgs, Match::Except, __sd, false},
			{AutomatonState::IgnoreNL, AutomatonState::Final, Match::OneOf, __sd, false},
	{AutomatonState::Init, AutomatonState::Comment, Match::OneOf, "#", false},
		{AutomatonState::Comment, AutomatonState::Final, Match::OneOf, "\n", true},
		{AutomatonState::Comment, AutomatonState::Comment, Match::Except, "\n", true},
	{AutomatonState::Init, AutomatonState::BeginSingleQuate, Match::OneOf, "'", false},
		{AutomatonState::BeginSingleQuate, AutomatonState::InSingleQuate, Match::Except, "'", false},
		{AutomatonState::InSingleQuate, AutomatonState::EndSingleQuate, Match::OneOf, "'", true},
			{AutomatonState::EndSingleQuate, AutomatonState::Final, Match::Except, "", false},
		{AutomatonState::InSingleQuate, AutomatonState::InSingleQuate, Match::Except, "'\\", true},
		{AutomatonState::InSingleQuate, AutomatonState::EscapeSingleQuate, Match::OneOf, "\\", true},
			{AutomatonState::EscapeSingleQuate, AutomatonState::InSingleQuate, Match::OneOf, "'", false},
			{AutomatonState::EscapeSingleQuate, AutomatonState::InSingleQuate, Match::Except, "'", true},
		{AutomatonState::BeginSingleQuate, AutomatonState::EndSingleQuate, Match::OneOf, "'", false},
	{AutomatonState::Init, AutomatonState::BeginDoubleQuate, Match::OneOf, "\"", false},
		{AutomatonState::BeginDoubleQuate, AutomatonState::InDoubleQuate, Match::Except, "\"", false},
		{AutomatonState::InDoubleQuate, AutomatonState::EndDoubleQuate, Match::OneOf, "\"", true},
			{AutomatonState::EndDoubleQuate, AutomatonState::Final, Match::Except, "", false},
		{AutomatonState::InDoubleQuate, AutomatonState::InDoubleQuate, Match::Except, "\"\\", true},
		{AutomatonState::InDoubleQuate, AutomatonState::EscapeDoubleQuate, Match::OneOf, "\\", true},
			{AutomatonState::EscapeDoubleQuate, AutomatonState::InDoubleQuate, Match::OneOf, "\"", false},
			{AutomatonState::EscapeDoubleQuate, AutomatonState::InDoubleQuate, Match::Except, "\"", true},
		{AutomatonState::BeginDoubleQuate, AutomatonState::EndDoubleQuate, Match::OneOf, "\"", false},
};

AutomatonTransition __transitionNullObject = { AutomatonState::None, AutomatonState::None, Match::Except, "", false };

