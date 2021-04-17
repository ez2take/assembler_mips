#include "assembler.h"

bool is_alphanum(char c){
  return ('a'<= c && c <='z')||
         ('A'<= c && c <='Z')||
         ('0'<= c && c <='9')||
         (c == '_');
}
bool consume(char *op){
  if(token->type != TK_RESERVED || 
     strlen(op) != token->len   ||
     memcmp(op,token->str,token->len)){
    return false;
  }
  token = token->next;
  return true;
}

void expect(char *op){
  if(token->type != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(op,token->str,token->len)){
    error("not '%c'",op);
  }
  token = token->next;
}

int expect_number(){
  if(token->type != TK_NUM){
    error("not number");
  }
  int val = token->val;
  token = token->next;
  return val;
}
bool consume_tokentype(TokenType tokentype){
  if(token->type == tokentype){
    token = token->next;
    return true;
  }
  return false;
}

bool at_eof(){
  return token->type == TK_EOF;
}

Token *new_token(TokenType type, Token *cur,char *str,int len){
  Token *tok = calloc(1,sizeof(Token));
  tok->type = type;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}
Token *new_token_str(Token *cur,char *p){
  Token *tok = calloc(1,sizeof(Token));
  tok->type = TK_STR;
  tok->str = p++;
  int len = 1;
  while(is_alphanum(*p)){
    p++;
    len++;
  }
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p){
  Token head;
  head.next = NULL;
  Token *cur = &head;
  while(*p){
    if(isspace(*p)){
      p++;
      continue;
    }
    if(strncmp(p,"#DEPTH",7)==0){
      cur = new_token(TK_RESERVED,cur,p,7);
      p+=2;
      continue;
    }
    //comment out
    if (strncmp(p, "//", 2) == 0) {
      p += 2;
      while (*p != '\n')
        p++;
      continue;
    }
    if(*p == '('||*p == ')'||*p == '='||
       *p == ':'||*p == ','){
      cur = new_token(TK_RESERVED,cur,p++,1);
      continue;
    }
    if('a'<=*p && *p<='z' || 'A'<=*p && *p<='Z' ){
      cur = new_token_str(cur,p);
      p += cur->len;
      continue;
    }
    if(isdigit(*p)){
      cur = new_token(TK_NUM,cur,p,0);
      cur->val = strtol(p,&p,10);
      continue;
    }
    error("invalid word is used");
  }
  new_token(TK_EOF,cur,p,0);
  return head.next;
}



