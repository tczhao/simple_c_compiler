# simple_c_compiler

- next() - lexical analyzer
- program() - entrance of parser
- expression(level) - parser
- eval() - interpret target instruction

# ideal compiler

- Scanner (Lexical Analyzer)
	- converts the source program's stream of characters into a stream of tokens
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