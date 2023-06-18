/* date = June 17th 2023 2:49 pm */

#ifndef LEXER_H
#define LEXER_H

#include "defines.h"
#include "base/base.h"

typedef u32 TokenType;
enum {
  TokenType_Error, TokenType_EOF,
  
  TokenType_Ident,
  
  TokenType_Number,
  TokenType_Plus, TokenType_Minus, TokenType_Star, TokenType_Slash, TokenType_Caret,
  TokenType_OpenParenthesis, TokenType_CloseParenthesis, TokenType_Comma,
  
  TokenType_MAX,
};

typedef struct Token {
  TokenType type;
  string lexeme;
} Token;

typedef struct Lexer {
  char* start;
  char* current;
} Lexer;


void  lexer_init(Lexer* lexer, string expression);
Token lexer_next_token(Lexer* lexer);

string lexer_debug_get_token_type_name(TokenType type);
void   lexer_debug_print_token(Token token);

#endif //LEXER_H
