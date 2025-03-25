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

The class `LexicalTokenizer` is lexical tokenizer that represented by the following Deterministic Finite Automaton(DFA) Diagram.

```mermaid
flowchart LR
classDef final stroke-width:2pt, stroke:black;

start -- #59; --> EoC:::final
start -- | --> pipe:::final
          pipe -- | --> OR:::final
start -- (,) --> subshell:::final
start -- & --> bg:::final
          bg -- & --> AND:::final
          bg -- > --> write12:::final
start -- < --> read:::final
start -- > --> write:::final
          write -- > --> append:::final
          write -- & --> specify
                    specify -- 1,2 --> dup:::final
start -- 1,2 --> digit:::final
          digit -- > --> write:::final
          digit -- except s.d. --> cmdargs:::final
start -- exept s.d. --> cmdargs:::final
          cmdargs -- except s.d. --> cmdargs
          cmdargs -- \ --> escape
          cmdargs -- # --> comment
start -- \ --> escape
          escape -. any .-> cmdargs
start -- # --> comment:::final
          comment -- except '#92;n' --> comment
start -- ' --> squate:::final
          squate -- except ',\ --> squate
          squate -- \ --> sq_escape
                     sq_escape -. ' .-> squate
                     sq_escape -- any --> squate
start -- #34; --> dquate:::final
               dquate -- except #34;,\ --> dquate
               dquate -- \ --> dq_escape
                          dq_escape -.#34;.-> dquate
                          dq_escape -- any --> dquate
```

where s.d. (syntax delimiter) means following character set.

```
'>' '<' '&' '|' ';' '\n' ' ' '(' ')' ''' '"'
```
The **bold border square** can be the final state. If you are at final state and there is no destination, the function retuns a token string.
The **normal square** cannot be the final state. Tokenization finished with error if there is no destination.

A solid line means transition to other state and append previous character to the token string. A dash line also means transition to other state, but it ignore previous character and does not push_back string array.

```mermaid
flowchart LR
classDef final stroke-width:2pt, stroke:black;
	id1(normal square)
	id2(final state):::final
	id1 -- solid --> id2
	id1 -. dash  .-> id2
```

## Recursive Descent Parsing

