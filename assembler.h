#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// this assembler is inspired by intel syntax and casl ii syntax

typedef struct string string;
struct string{
    char *str;
    int   len;
};

typedef struct Label Label;
struct Label{
    Label *next;
    string *name;
    int address;
};
/*formula
    ("[label]:") [instr] [reg] "," [reg] "," ([reg] | [imm] | [lablel])
    ("[label]:") [instr] [reg] "," [offset] "(" [reg] ")"
    ("[label]:") [instr] ([imm] | [label])
*/

//MIPS ISA
//OPCODE [31:26]
typedef enum{
    RTYPE =  0 , //"000000"
    J     =  2 , //"000010"
    JAL   =  3 , //"000011"
    BEQ   =  4 , //"000100"
    BNE   =  5 , //"000101"
    ADDI  =  8 , //"001000"
    LW    =  35, //"100011"
    SW    =  43, //"101011"
    INONE  =  -1,
}InstrType;
//R-TYPE FUNCT [5:0]
typedef enum{
    ADD   =  32, //"100000"
    SUB   =  34, //"100010"
    AND   =  36, //"100100"
    OR    =  37, //"100101"
    SLT   =  42, //"101010"
    FNONE  =  -1,
}FunctType;

typedef struct Instr Instr;
struct Instr{
    InstrType instr;
    int          rs;
    int          rt;
    int          rd;
    FunctType funct;
    int         imm;
    string   *label;
    int    jaddress;
};

//define
#define OP_RADIX 10         //operand radix of the input file
//#define DEBUG

//global variables
extern int depth;
extern char *user_input;    //user_input => memory&label_table
extern Label *label_table;

//functions

void error(char *fmt, ...);
void error_at(char *loc,char *fmt, ...);

void parse(char *p,Instr *memory[]);  //recognize labels and parse line into label,mnemonic,and operands
void codegen(Instr *memory[]);


