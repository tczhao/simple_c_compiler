#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#define int long long // work with 64bit target

int token;            // current token
char *src, *old_src;  // pointer to source code string;
int poolsize;         // default size of text/data/stack
int line;             // line number

// Memory
int *text,            // text segment
    *old_text,        // for dump text segment
    *stack;           // stack
char *data;           // data segment

// Register for virtual machine
int *pc,              // program counter 
    *bq,              // stack pointer
    *sp,              // base pointer
    ax,               // general register
    cycle;

// Instructions
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

// Lexical analyzer
void next() {
    token = *src++;
    return;
}

// Parser
void expression(int level) {
    // do nothing
}

// Parser entrance
void program() {
    next();                  // get next token
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}

// Interpret on custom vm
int eval() {
    int op, *tmp;
    while(1) {
        op = *pc++; // get next operation code
        if (op == IMM)       {ax = *pc++;}
        else if (op == LC)   {ax = *(char *)ax;}
        else if (op == LI)   {ax = *(int *)ax;}
        else if (op == SC)   {ax = *(int *)*sp++ = ax;}
        else if (op == SI)   {*(int *)*sp++ = ax;}
        else if (op == PUSH) {*--sp = ax;}
        else if (op == JMP)  {pc = (int *)*pc;}
    }
    return 0;
}

int main(int argc, char **argv)
{
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }

    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }

    src[i] = 0; // add EOF character
    close(fd);

    // Memory - allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("cloud not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    // Register
    bq = sp = (int *) ((int)stack + poolsize);
    ax = 0;

    program();
    return eval();
}