# home-made-shell

## Syntax

The available syntax are as follows:

```
command > file # stdout to file
command 1> file # stdout to file
command 2> file # stderr to file
command 1> fileA 2> fileB # stdout to fileA, stderr to fileB
command &> file # redirect stdout and stderr to file
cmd1 | cmd2 # connect stdout of cmd1 to stdin of cmd2.
```

The space can be ommitted:
```
command>file # stdout to file
command1>file # equal to "command1 > file". Different from "command 1> file"
```

## Lexical Tokenization

### Deterministic Finite Automaton

The lexical tokenizer is typically represented by the following Deterministic Finite Automaton(DFA) Diagram.

```mermaid
---
title : Deterministic Finite Automaton (DFA) Diagram
---
flowchart LR
classDef final stroke-width:2pt, stroke:black;

%% Define State
EoS(((EoS)))
pipe(((Pipe)))
OR(((Or)))
subshell(((SubShell)))
BackGround(((BackGround)))
AND(((And)))
write12(((Write12)))
read(((Read)))
append(((Append)))
digit(((Digit)))
write(((Write)))
specify((Specify))
dup(((Dup)))
cmdargs(((CmdArgs)))
escape((Escape))
SignleQuate(((SignleQuate)))
DoubleQuate(((DoubleQuate)))
EscapeSingleQuate((EscapeSingleQuate))
EscapeDoubleQuate((EscapeDoubleQuate))
comment(((Comment)))

%% Define Graph
start -- #59; --> EoS
start -- | --> pipe
          pipe -- | --> OR
start -- (,) --> subshell
start -- & --> BackGround
               BackGround -- & --> AND
               BackGround -- #62; --> write12
start -- < --> read
start -- #62; --> write
          write -- #62; --> append
          write -- & --> specify
                    specify -- 1,2 --> dup
start -- 1,2 --> digit
          digit -- #62; --> write
          digit -- except s.d. --> cmdargs
start -- exept s.d. --> cmdargs
          cmdargs -- except s.d. --> cmdargs
          cmdargs -- \ --> escape
start -- \ --> escape
          escape -- any --> cmdargs
start -- # --> comment
          comment -- except '#92;n' --> comment
start -- ' --> SignleQuate
          SignleQuate -- except '\ --> SignleQuate
          SignleQuate -- \ --> EscapeSingleQuate
                      EscapeSingleQuate -- any --> SignleQuate
start -- #34; --> DoubleQuate
               DoubleQuate -- except #34;\ --> DoubleQuate
               DoubleQuate -- \ --> EscapeDoubleQuate
                           EscapeDoubleQuate -- any --> DoubleQuate

```

where **double circle** means final state, and s.d. (syntax delimiter) means following character set.

```
'>' '<' '&' '|' ';' '\n' ' ' '(' ')' ''' '"'
```

### Extend DFM

But above diagram has some problem when you coding Lexical Tokenizer.

#### Ignorable character

For example, input `"abc\efg"` means `abc\efg` itsself, but `"abc\"efg"` means `abc"efg`. In the Double-Quatation, the escape character `\` is removed when the next character is `"`, but `\` is not removed when the next is the other.
Then, we introduce two transition type; the **solid line** means "append previous character", the dash line means "ignore previous character".

```mermaid
---
title: Transition Type
---
flowchart LR
DoubleQuate(((DoubleQuate)))
EscapeDoubleQuate((EscapeDoubleQuate))
DoubleQuate -- \ --> EscapeDoubleQuate
EscapeDoubleQuate -. #34; .-> DoubleQuate
EscapeDoubleQuate -- except #34; --> DoubleQuate

```

#### Independent final state

In the diagram, there is two types of final state that are distinguished by the token contains the final character or not.
For example, if the automaton ended at DoubleQuate state, the token does not contain the character `"`. But if the automaton ended at `;`, the token contains `;`.
These difference is represented as transiton type to the independent final state.

```mermaid
---
title: Independent Final State
---
flowchart LR

DoubleQuate((DoubleQuate))
Final(((Final)))
Start -- #34; --> DoubleQuate
DoubleQuate -. except #34; .-> InDoubleQuate
InDoubleQuate -- except #34; --> InDoubleQuate
InDoubleQuate -. #34; .-> Final

```

#### Extended DFM for Lexical Tokenizer

Finally, we use the following Extended Deterministic Finite Automaton Diagram to describe the Lexical Tokenizer insdted of the typical DFM.
Each arrows are correspond to `TransitionTable` defined in `src/lexicaltokenizer.cpp`. The arrow has no condition means match any characters.

```mermaid
---
title : Extended DFA
---
flowchart LR

%% Define State
Init((Init))
Final(((Final)))
EoS[EoS<BR>#60;EoS#62;]
Pipe[Pipe<BR>#60;Pipe#62;]
OR[Or<BR>#60;Or#62;]
SubShell[SubShell<BR>#60;SubShell#62;]
BackGround[BackGround<BR>#60;BackGround#62;]
And[And<BR>#60;And#62;]
Write12[Write12<BR>#60;Redirect#62;]
Read[Read<BR>#60;Redirect#62;]
Append[Append<BR>#60;Redirect#62;]
Digit[Digit<BR>#60;Word#62;]
Write[Write<BR>#60;Redirect#62;]
Specify[Specify]
Dup[Dup<BR>#60;Redirect#62;]
CmdArgs[CmdArgs<BR>#60;Word#62;]
Escape[Escape]
BeginSingleQuate[BeginSingleQuate]
InSingleQuate[InSingleQuate]
EndSingleQuate[EndSingleQuate<BR>#60;Word#62;]
EscapeSingleQuate[EscapeSingleQuate]
BeginDoubleQuate[BeginDoubleQuate]
InDoubleQuate[InDoubleQuate]
EndDoubleQuate[EndDoubleQuate<BR>#60;Word#62;]
EscapeDoubleQuate[EscapeDoubleQuate]
Comment[Comment<BR>#60;Comment#62;]

%% Define Graph
Init --\s\t--> Init
Init -- #59; --> EoS
  EoS --any--> Final
Init --|--> Pipe
  Pipe --> Final
  Pipe -- | --> OR
    OR --> Final
Init --()--> SubShell
  SubShell--> Final
Init --&--> BackGround
  BackGround --except &>--> Final
  BackGround --&--> And
    And --> Final
  BackGround -- #62; --> Write12
    Write12 --> Final
Init -- < --> Read
  Read --> Final
Init -- #62; --> Write
  Write --> Final
  Write -- #62; --> Append
    Append --> Final
  Write -- & --> Specify
    Specify -- 12 --> Dup
      Dup --> Final
Init --12--> Digit
  Digit -- #62; --> Write
  Digit -- except s.d. --> CmdArgs
Init -- exept s.d. \\ --> CmdArgs
  CmdArgs --s.d. --> Final
  CmdArgs -- except s.d. and \ --> CmdArgs
  CmdArgs -- \ --> Escape
Init -- \ --> Escape
  Escape -.except #92;n.-> CmdArgs
  Escape -.#92;n.-> IgnoreNL
    IgnoreNL -.\.-> Escape
    IgnoreNL -..->|except s.d.| CmdArgs
    IgnoreNL -..->|s.d.| Final
Init --#35;--> Comment
  Comment --#92;n--> Final
  Comment --except '#92;n'--> Comment
Init -- ' --> BeginSingleQuate
  BeginSingleQuate -.except '.-> InSingleQuate
    InSingleQuate --'--> EndSingleQuate
      EndSingleQuate -..-> Final
    InSingleQuate -- except '\ --> InSingleQuate
    InSingleQuate -- \ --> EscapeSingleQuate
      EscapeSingleQuate -.'.-> InSingleQuate
      EscapeSingleQuate --except '--> InSingleQuate
  BeginSingleQuate -.'.-> EndSingleQuate
Init -- #34; --> BeginDoubleQuate
  BeginDoubleQuate --except #34;--> InDoubleQuate
    InDoubleQuate --#34;--> EndDoubleQuate
      EndDoubleQuate -..-> Final
   InDoubleQuate -- except #34;\ --> InDoubleQuate
   InDoubleQuate -- \ --> EscapeDoubleQuate
     EscapeDoubleQuate -.#34;.-> InDoubleQuate
     EscapeDoubleQuate --except #34;--> InDoubleQuate
   BeginDoubleQuate -.#34;.-> EndDoubleQuate
```

where the expression `<foo>` is the token type correspond to EBNF described in the next section.

## Recursive Descent Parsing

The home-made-shell syntax is defined by following Extend Backus Naur From(EBNF).

```EBNF
<Shell> ::= (<Sentence>)+ [ <Comment> ]
<Sentence> ::= <ProcessGroup> [ ("&&" | "||" ) <ProcessGroup> ] [";"|"&"]
<ProcessGroup> ::= <Process> { "|" <Process> }
<Process> ::= (<Command> | "(" <Shell> ")") { (<Redirect> <Word>) | <Dup> }
<Command> ::= (<Word>)+
<Word> ::= <CmdArgs> | <SignleQuate> | <DoubleQuate>
```

* `{a}` : Repeat `a` 0 or more times.
* `(a)+` : Repeat `a` 1 or more times.
* `[a]` : `a` is optional.
* `a|b` means a OR b.

