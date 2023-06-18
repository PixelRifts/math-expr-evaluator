#include "defines.h"
#include "os/os.h"
#include "os/window.h"
#include "os/input.h"
#include "base/tctx.h"

#include "expr/parser.h"
#include "expr/eval.h"

int main(int argc, char **argv) {
  if (argc != 2) LogFatal("Expected a math expression as the only input argument");
  
  OS_Init();
  ThreadContext context = {0};
  tctx_init(&context);
  
  
  
  Parser parser;
  parser_init(&parser, str_make(argv[1]));
  Expression_Node* tree = parser_parse_expression(&parser, Precedence_MIN);
  parser_debug_dump_tree(tree, 0);
  f64 answer = evaluate(tree);
  printf("%f\n", answer);
  parser_free(&parser);
  
  
  
  tctx_free(&context);
}
