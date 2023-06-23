/* date = June 17th 2023 8:19 pm */

#ifndef PARSER_H
#define PARSER_H

#include "defines.h"
#include "lexer.h"

typedef u32 Expression_Node_Kind;
enum {
  NodeType_Error,
  NodeType_Number,
  NodeType_Positive,
  NodeType_Negative,
  NodeType_Add,
  NodeType_Sub,
  NodeType_Mul,
  NodeType_Div,
  NodeType_Pow,
};

typedef struct Expression_Node Expression_Node;
struct Expression_Node {
  Expression_Node_Kind type;
  
  union {
    f64 number;
    struct { Expression_Node* operand; } unary;
    struct { Expression_Node* left; Expression_Node* right; } binary;
  };
};

typedef u32 Precedence;
enum {
  Precedence_MIN,
  
  Precedence_Term,
  Precedence_Mult,
  Precedence_Div,
  Precedence_Power,
  
  Precedence_MAX,
};

typedef struct Parser {
  M_Pool node_pool;
  
  Token curr;
  Token next;
  Lexer lexer;
} Parser;

void             parser_init(Parser* parser, string expression);
Expression_Node* parser_parse_expression(Parser* parser, Precedence prec);
void             parser_free(Parser* parser);

void parser_debug_dump_tree(Expression_Node* node, u32 indent);

#endif //PARSER_H
