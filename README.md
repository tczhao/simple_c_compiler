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