#include "eval.h"

#include <math.h>

f64 evaluate(Expression_Node* expr) {
  switch (expr->type) {
    case NodeType_Number: return expr->number;
    case NodeType_Positive: return +evaluate(expr->unary.operand);
    case NodeType_Negative: return -evaluate(expr->unary.operand);
    case NodeType_Add: return evaluate(expr->binary.left) + evaluate(expr->binary.right);
    case NodeType_Sub: return evaluate(expr->binary.left) - evaluate(expr->binary.right);
    case NodeType_Mul: return evaluate(expr->binary.left) * evaluate(expr->binary.right);
    case NodeType_Div: return evaluate(expr->binary.left) / evaluate(expr->binary.right);
    case NodeType_Pow: return pow(evaluate(expr->binary.left), evaluate(expr->binary.right));
  }
  return 0;
}
