# simple_c_compiler

Simple compiler that targets a vm with custom instruction

```
gcc compiler.c
./a.out hello.c
```

Topdown parser calculator
```
gcc topdown_parser_calculator.c
./a.out
3*2+1
```

```
init
- text (text segment)
- data (data segment)
- stack (stack)
- symbol (symbol table)

- add default keywords to symbol
- add default library to symbol

-> read source file
	-> get next token
		-> while token exist
			-> global_declaration()
				-> add enum to symbol
				-> determine char|int
				   - determine pointer (token Mul -> Type = basetype + PTR)
				   - validate global declaration
				   - check duplicate declaration
						-> function_declaration() (Class:Fun,Value:text+1)
							- function_parameter()
								- params=0
								- INT|CHAR|pointer
								- validate,duplicate checking
								- current_id:Bxxx=Class(Loc)/Type/Value(params++)
								- index_of_bp = params+1;
							- function_body()
								- local variable declaration
								- *++text = ENT
								- *++text = pos_local - index_of_bq (length of local variable address)
								- statement()
									-> token: If
										- within('()') expression(Assign)
										- *++text = JZ; b = ++text
										- statement()
										- token: Else
											- *b = (int)(text+3); *++text = JMP; b = ++text
											- statement()
										- *b = (int)(text+1)
									-> token: While
										- a = text+1
										- within('()') expression(Assign)
										- *++text = JZ; b = ++text
										- statement()
										- *++text = JMP; *++text = (int)a; b = (int)(text+1)
									-> token: '{'
										- within('{}') statement()
									-> token: Return
										- expression(Assign)
									-> token: ';'
									-> expression(Assign)
								- *++text = LEV
							- unwind local declaration, current_id:Class/Type/Value=Bxxx
						-> variable_declaration()
							- Class = Glo
							- Value = (int)data (memory address)
				-> get next token
	-> pc = Value: text
	-> setup stack
		- EXIT
		- PUSH
		- argc
		- argv
		- *PUSH
```

# ideal compiler

```
                   +-------+                      +--------+
-- source code --> | lexer | --> token stream --> | parser | --> assembly
                   +-------+                      +--------+
```

- Scanner (Lexical Analyzer)
	- converts the source program's stream of characters into a stream of tokens
	- to simplify the parser by converting the stream of source code into token stream
		- remove comments
		- expand macros
		- check indentation
		- remove whitespace
- Parser (Syntax Analyzer)
	- turns token sequence into an abstract syntax tree
- Semantic Analyzer
	- check legality rules and form a semantic graph
- Intermediate Code Generator
	- produce a flow graph
- Machine Independent Code Improver
- Target Code Generator
- Machine Dependent Code Improver

# memory

```
+------------------+
|    stack   |     |      high address
|    ...     v     |
|                  |
|                  |
|                  |
|                  |
|    ...     ^     |
|    heap    |     |
+------------------+
| bss  segment     |
+------------------+
| data segment     |
+------------------+
| text segment     |      low address
+------------------+
```

- `text` storing code(instructions)
- `data` storing initialized data `int i = 10`
- `bss` storing un-initialized data `int i[1000]`
- `stack` handling states of function calls
- `heap` allocate memory dynamically for program

# registers

store running states of computers
- `PC`: program counter, stores memory address that stores the **next** instruction to be run
- `SP`: stack pointer, which point to the top of the stack. Stack is stored from high address to low address, so when we push a new element, the `SP` decreases
- `BP`: base pointer, points to some elements on the stack, used in function calls
- `AX`: general register that store the result of an instruction

# instruction set

a set of instruction that CPU can understand

- `MOV`
	- move data into registers or memory
	- in `x86`, `MOV dest, source`
	- our implementation, the `MOV` is replace by
		- `IMM <num>` to put immediate `<num>` into register `AX`
		- `LC` to load a character into `AX` from a memory address which is stored in `AX` before execution
		- `LI` to load a integer into `AX` from a memory address which is stored in `AX` before execution
		- `SC` to store the character in `AX` into the memory whose address is stored on the top of the stack
		- `SI` to store the integer in `AX` into the memory whose address is stored on the top of the stack
- `PUSH`
	- in `x86` can push an immediate value or a register's value onto the stack
	- our implementation
		- `PUSH` push the value in `AX` onto the stack
- `JMP`
	- `JMP <addr>` will uncondtionally set the value `PC` register to `<addr>`
	- `*pc` stores the argument of `JMP` instructuion
- `JZ/JNZ`
	- conditional jump to implement `if` statement
	- jump when `AX` is `0` or not
- `CALL`
	- to call the function whose starting point is `<addr>` and fetch the bookkeeping information to resume previous execution
	- Our convention, returning value is store into AX regardless of value or memory address
	- C convention
		- It is caller's duty to push the arguments onto stack
		- After function call returns, caller need to pop out the argument
		- The arguments are pushed in reversed order
- `ENT`
	- called when about to enter the function call to "make a new calling frame"
	- store the current `PC` value onto the stack, and save some space(`<size>` bytes) to store the local variables for function
- `ADJ`
	- to adjust the stack, to "remove arguments from frame"
- `LEV`
	- to accomplish `MOV` `POP` `RET`
	- function call return
- `LEA`
	- fetch sub function argument

# lexer

- lexical analysis is the process of converting a sequence of characters (such as in a computer program or web page) into a sequence of tokens (strings with an identified "meaning").
- Lexer is used to pre-process the source code, so as to reduce the complexity of parser.
- Lexer is also a kind of compiler which consumes source code and output token stream.
- lookahead(k) is used to fully determine the meaning of current character/token.
- e.g. Normally we represent the token as a pair: (token type, token value). For example, if a program's source file contains string: "998", the lexer will treat it as token (Number, 998) meaning it is a number with value of 998.

Symbol table is an important data structure created and maintained by compilers in order to store information about the occurrence of various entities such as variable names, function names, objects, classes, interfaces

```
struct identifier {
    int token;
    int hash;
    char * name;
    int class;
    int type;
    int value;
    int Bclass;
    int Btype;
    int Bvalue;
}
```

- `token`: is the token type of an identifier. Theoretically it should be
   fixed to type `Id`. But it is not true because we will add keywords(e.g
   `if`, `while`) as special kinds of identifier.
- `hash`: the hash value of the name of the identifier, to speed up the
   comparision of table lookup.
- `name`: well, name of the identifier.
- `class`: Whether the identifier is global, local or constants.
- `type`: type of the identifier, `int`, `char` or pointer.
- `value`: the value of the variable that the identifier points to.
- `BXXX`: local variable can shadow global variable. It is used to store
   global ones if that happens.

our implementation

```
----+-----+----+----+----+-----+-----+-----+------+------+----
 .. |token|hash|name|type|class|value|btype|bclass|bvalue| ..
----+-----+----+----+----+-----+-----+-----+------+------+----
    |<---       one single identifier                --->|
```

# Top-down parsing

start with a non-terminator and recursively check the source code to replace the non-terminators with its alternatives until no non-terminator is left

# Terminator and Non-terminator

They are terms used in Backus-Naur Form (BNF, a language that describe grammars)

```
<expr> ::= <expr> + <term>
         | <expr> - <term>
         | <term>

<term> ::= <term> * <factor>
         | <term> / <factor>
         | <factor>

<factor> ::= ( <expr> )
           | Num
```

item enclosed by `<>` is called a `Non-terminator`
- The can get replaced with items on the right hand of `::=`
- `|` means alternatives

Terminator
- Those do not appear on the left side of `::=`, e.g. `+`, `(`, `Num`, often are the tokens we get from the lexer

calculator example `3 * (4 + 2)`
```
1. <expr> => <expr>
2.           => <term>        * <factor>
3.              => <factor>     |
4.                 => Num (3)   |
5.                              => ( <expr> )
6.                                   => <expr>           + <term>
7.                                      => <term>          |
8.                                         => <factor>     |
9.                                            => Num (4)   |
10.                                                        => <factor>
11.                                                           => Num (2)
```

left recursion
- translate into non left-recursive equivalent to prevent never exit
```
<expr> ::= <term> <expr_tail>

<expr_tail> ::= + <term> <expr_tail>
              | - <term> <expr_tail>
              | <empty>

<term> ::= <factor> <term_tail>

<term_tail> ::= * <factor> <term_tail>
              | / <factor> <term_tail>
              | <empty>

<factor> ::= ( <expr> )
           | Num
```

# EBNF

```
program ::= {global_declaration}+

global_declaration ::= enum_decl | variable_decl | function_decl

enum_decl ::= 'enum' [id] '{' id ['=' 'num'] {',' id ['=' 'num'] '}'

variable_decl ::= type {'*'} id { ',' {'*'} id } ';'

function_decl ::= type {'*'} id '(' parameter_decl ')' '{' body_decl '}'

parameter_decl ::= type {'*'} id {',' type {'*'} id}

body_decl ::= {variable_decl}, {statement}

statement ::= non_empty_statement | empty_statement

non_empty_statement ::= if_statement | while_statement | '{' statement '}'
                     | 'return' expression | expression ';'

if_statement ::= 'if' '(' expression ')' statement ['else' non_empty_statement]

while_statement ::= 'while' '(' expression ')' non_empty_statement
```
