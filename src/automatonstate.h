#pragma once

enum class AutomatonState
{
	// spacial state
	None,
	Init,
	Final,

	// normal state
	EoS,
	Pipe,
	And,
	Or,
	SubShellBegin,
	SubShellEnd,
	BackGround,
	Write12,
	Read,
	Write,
	Append,
	Specify,
	Dup,
	Digit,
	CmdArgs,
	Escape,
	IgnoreNL,
	Comment,
	BeginSingleQuate,
	InSingleQuate,
	EndSingleQuate,
	EscapeSingleQuate,
	BeginDoubleQuate,
	InDoubleQuate,
	EndDoubleQuate,
	EscapeDoubleQuate,
};

