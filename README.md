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

The Deterministic Finite Automaton(DFA) Diagram is as follows:

```mermaid
flowchart LR
classDef final stroke-width:2pt, stroke:black;

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
          escape -- any --> cmdargs
start -- # --> comment:::final
          comment -- except '#92;n' --> comment
start -- ' --> squate:::final
          squate -- except ',\ --> squate
          squate -- \ --> sq_escape
                     sq_escape -- any --> squate
start -- #34; --> dquate:::final
               dquate -- except #34;,\ --> dquate
               dquate -- \ --> dq_escape
                          dq_escape -- any --> dquate
```

where

```mermaid
stateDiagram-v2
classDef final stroke-width:2pt, stroke:black;
mid_state
note right of mid_state: This cannot be final state. Error if there is no destination.

final_state:::final
note right of final_state: This can be final state. Return the token if there is no destination.
```


The s.d. (syntax delimiter) means following character set:

```
'>' '<' '&' '|' '\n' ' ' '(' ')' ''' '"'
```

This diagram is correspond to the class `LexicalTokenizer`.

## Recursive Descent Parsing

