#include "parser.h"


static Precedence precedence_lookup[TokenType_MAX] = {
  [TokenType_Plus]  = Precedence_Term,
  [TokenType_Minus] = Precedence_Term,
  [TokenType_Star]  = Precedence_Mult,
  [TokenType_Slash] = Precedence_Div,
  [TokenType_Caret] = Precedence_Power,
};


//~ Helpers

static void parser_advance(Parser* parser) {
  parser->curr = parser->next;
  parser->next = lexer_next_token(&parser->lexer);
}

static Expression_Node* parser_parse_number(Parser* parser) {
  f64 value = strtod((const char*) parser->curr.lexeme.str, nullptr);
  parser_advance(parser);
  
  Expression_Node* ret = pool_alloc(&parser->node_pool);
  MemoryZeroStruct(ret, Expression_Node);
  ret->type = NodeType_Number;
  ret->number = value;
  return ret;
}

static Expression_Node* parser_parse_prefix_expr(Parser* parser) {
  Expression_Node* ret = nullptr;
  
  if (parser->curr.type == TokenType_Number) {
    ret = parser_parse_number(parser);
  } else if (parser->curr.type == TokenType_OpenParenthesis) {
    parser_advance(parser);
    ret = parser_parse_expression(parser, Precedence_MIN);
    if (parser->curr.type == TokenType_CloseParenthesis) {
      parser_advance(parser);
    }
  } else if (parser->curr.type == TokenType_Plus) {
    parser_advance(parser);
    ret = pool_alloc(&parser->node_pool);
    ret->type = NodeType_Positive;
    ret->unary.operand = parser_parse_prefix_expr(parser);
  } else if (parser->curr.type == TokenType_Minus) {
    parser_advance(parser);
    ret = pool_alloc(&parser->node_pool);
    ret->type = NodeType_Negative;
    ret->unary.operand = parser_parse_prefix_expr(parser);
  }
  
  if (!ret) {
    ret = pool_alloc(&parser->node_pool);
    MemoryZeroStruct(ret, Expression_Node);
    ret->type = NodeType_Error;
  }
  
  if (parser->curr.type == TokenType_Number ||
      parser->curr.type == TokenType_OpenParenthesis) {
    Expression_Node* new_ret = pool_alloc(&parser->node_pool);
    new_ret->type = NodeType_Mul;
    new_ret->binary.left = ret;
    new_ret->binary.right = parser_parse_expression(parser, Precedence_Div);
    ret = new_ret;
  }
  
  return ret;
}

static Expression_Node* parser_parse_infix_expr(Parser* parser, Token operator,
                                                Expression_Node* left) {
  Expression_Node* ret = pool_alloc(&parser->node_pool);
  MemoryZeroStruct(ret, Expression_Node);
  
  switch (operator.type) {
    case TokenType_Plus:  ret->type = NodeType_Add; break;
    case TokenType_Minus: ret->type = NodeType_Sub; break;
    case TokenType_Star:  ret->type = NodeType_Mul; break;
    case TokenType_Slash: ret->type = NodeType_Div; break;
    case TokenType_Caret: ret->type = NodeType_Pow; break;
  }
  ret->binary.left = left;
  ret->binary.right = parser_parse_expression(parser, precedence_lookup[operator.type]);
  return ret;
}

//~ Main things

void parser_init(Parser* parser, string expression) {
  pool_init(&parser->node_pool, sizeof(Expression_Node));
  parser->curr = (Token) {0};
  parser->next = (Token) {0};
  lexer_init(&parser->lexer, expression);
  parser_advance(parser);
  parser_advance(parser);
}

void parser_free(Parser* parser) {
  pool_free(&parser->node_pool);
}

Expression_Node* parser_parse_expression(Parser* parser, Precedence curr_operator_prec) {
  Expression_Node* left = parser_parse_prefix_expr(parser);
  Token next_operator = parser->curr;
  Precedence next_operator_prec = precedence_lookup[parser->curr.type];
  
  while (next_operator_prec != Precedence_MIN) {
    
    if (curr_operator_prec >= next_operator_prec) {
      break;
    } else /* (curr_operator_prec < next_operator_prec) */ {
      parser_advance(parser); // Advance the operator
      
      left = parser_parse_infix_expr(parser, next_operator, left);
      next_operator = parser->curr;
      next_operator_prec = precedence_lookup[parser->curr.type];
    }
  }
  
  return left;
}

void parser_debug_dump_tree(Expression_Node* node, u32 indent) {
  for (u32 i = 0; i < indent; i++) printf("  ");
  
  switch (node->type) {
    case NodeType_Error: printf("Error\n"); break;
    
    case NodeType_Number: {
      printf("%f\n", node->number);
    } break;
    
    case NodeType_Positive: {
      printf("Unary +:\n");
      parser_debug_dump_tree(node->unary.operand, indent + 1);
    } break;
    
    case NodeType_Negative: {
      printf("Unary -:\n");
      parser_debug_dump_tree(node->unary.operand, indent + 1);
    } break;
    
    case NodeType_Add: {
      printf("+:\n");
      parser_debug_dump_tree(node->binary.left, indent + 1);
      parser_debug_dump_tree(node->binary.right, indent + 1);
    } break;
    
    case NodeType_Sub: {
      printf("-:\n");
      parser_debug_dump_tree(node->binary.left, indent + 1);
      parser_debug_dump_tree(node->binary.right, indent + 1);
    } break;
    
    case NodeType_Mul: {
      printf("*:\n");
      parser_debug_dump_tree(node->binary.left, indent + 1);
      parser_debug_dump_tree(node->binary.right, indent + 1);
    } break;
    
    case NodeType_Div: {
      printf("/:\n");
      parser_debug_dump_tree(node->binary.left, indent + 1);
      parser_debug_dump_tree(node->binary.right, indent + 1);
    } break;
    
    case NodeType_Pow: {
      printf("^:\n");
      parser_debug_dump_tree(node->binary.left, indent + 1);
      parser_debug_dump_tree(node->binary.right, indent + 1);
    } break;
  }
}
