#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


typedef enum{
  TK_RESERVED,      //reserved word
  TK_STR,           //instruction or label
  TK_NUM,           //register or imm or offset
  TK_EOF,           //end token
}TokenType;

typedef struct Token Token;
struct Token{
    TokenType   type;
    Token      *next;
    char        *str;     //instruction or label or reserved word
    int          len;     //instruction or label or reserved word
    int          val;     //number of register or immadiate value or offset
};

/*formula
    "DEPTH" "=" []
    ([label] ":") [instr] [reg] "," [reg] "," ([reg] | [imm] | [lablel])
    ([label] ":") [instr] [reg] "," [offset] "(" [reg] ")"
    ([label] ":") [instr] ([imm] | [label])
*/
Token *token;
char *user_input;
char *filename;

Token *tokenize(char *p);
bool consume_tokentype(TokenType tokentype);
bool consume(char *op);
void expect(char *op);
int  expect_number();
bool at_eof();

void error(char *fmt, ...);
