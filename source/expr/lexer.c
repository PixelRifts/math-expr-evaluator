#include "lexer.h"

//~ A few helper functions, should be optimized out.... hopefully
static inline b8 is_digit(i8 c) { return c >= '0' && c <= '9'; }
static inline b8 is_whitespace(i8 c) { return c == ' ' || c == '\r' || c == '\n' || c == '\t'; }
static inline b8 is_alpha(i8 c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
static inline b8 lexer_bound(Lexer* lexer) { return *lexer->current == '\0'; }
static inline i8 lexer_peek(Lexer* lexer) { return *lexer->current; }
static inline i8 lexer_advance(Lexer* lexer) { lexer->current++; return lexer->current[-1]; }

static inline Token lexer_make_token(Lexer* lexer, TokenType type) {
  return (Token) {
    .type = type,
    .lexeme = (string) { .str = (u8*)lexer->start, .size = (u32) (lexer->current - lexer->start) },
  };
}
static inline void lexer_skip_whitespace(Lexer* lexer) {
  while (true) {
    i8 c = lexer_peek(lexer);
    switch (c) {
      case ' ': case '\r': case '\t': case '\n': lexer_advance(lexer); break;
      default: return;
    }
  }
}

//~ Actual stuff

Token lexer_number(Lexer* lexer) {
  while (is_digit(lexer_peek(lexer))) lexer_advance(lexer);
  if (lexer_peek(lexer) == '.') {
    lexer_advance(lexer);
    while (is_digit(lexer_peek(lexer))) lexer_advance(lexer);
  }
  return lexer_make_token(lexer, TokenType_Number);
}

Token lexer_identifier(Lexer* lexer) {
  while (is_alpha(lexer_peek(lexer)) || is_digit(lexer_peek(lexer))) {
    lexer_advance(lexer);
  }
  return lexer_make_token(lexer, TokenType_Ident);
}



void lexer_init(Lexer* lexer, string expression) {
  lexer->start = (char*) expression.str;
  lexer->current = (char*) expression.str;
}

Token lexer_next_token(Lexer* lexer) {
  lexer_skip_whitespace(lexer);
  lexer->start = lexer->current;
  if (lexer_bound(lexer)) return lexer_make_token(lexer, TokenType_EOF);
  i8 c = lexer_advance(lexer);
  
  switch (c) {
    case '(': return lexer_make_token(lexer, TokenType_OpenParenthesis);
    case ')': return lexer_make_token(lexer, TokenType_CloseParenthesis);
    case '+': return lexer_make_token(lexer, TokenType_Plus);
    case '-': return lexer_make_token(lexer, TokenType_Minus);
    case '*': return lexer_make_token(lexer, TokenType_Star);
    case '/': return lexer_make_token(lexer, TokenType_Slash);
    case '^': return lexer_make_token(lexer, TokenType_Caret);
    
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': return lexer_number(lexer);
    
    default: {
      if (is_alpha(c))
        return lexer_identifier(lexer);
      else {
        lexer_advance(lexer);
        return lexer_make_token(lexer, TokenType_Error);
      }
    }
  }
}

//~ Debug

string lexer_debug_get_token_type_name(TokenType type) {
  switch (type) {
    case TokenType_Error: return str_lit("Error");
    case TokenType_EOF: return str_lit("EOF");
    case TokenType_Number: return str_lit("Number Literal");
    case TokenType_Ident: return str_lit("Identifier");
    case TokenType_Plus: return str_lit("+");
    case TokenType_Minus: return str_lit("-");
    case TokenType_Star: return str_lit("*");
    case TokenType_Slash: return str_lit("/");
    case TokenType_Caret: return str_lit("^");
    case TokenType_OpenParenthesis: return str_lit("(");
    case TokenType_CloseParenthesis: return str_lit(")");
  }
  return str_lit("unreachable");
}

void lexer_debug_print_token(Token token) {
  printf("%.*s:  %.*s\n", str_expand(lexer_debug_get_token_type_name(token.type)),
         str_expand(token.lexeme));
}

