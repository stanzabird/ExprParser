# expr-parser

This program attempts to parse the normal mathematical expressions like 
1+1
2*3+1
2*(3+1)

---
## Parsing setup: BNF

'''
  expr   := expr '+' term | term;
  term   := term '*' factor | factor;
  factor := '(' expr ')' | number;
'''