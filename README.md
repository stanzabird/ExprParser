# expr-parser

This program attempts to parse the normal mathematical expressions like:

```
1+1
2*3+1
2*(3+1)
```

---
## Parsing setup: BNF for the syntax.

```
  expr   := expr '+' term | term;
  term   := term '*' factor | factor;
  factor := '(' expr ')' | number;
```

Our lexer has one function, next_token(), that is the only one that matters. It
produces the next token, or the 'end' state if the end of input is reached.

```cpp
struct lexer_t {
  enum token_id { op_add, op_mul, paren_open, paren_close, number, empty };
  struct token_t { token_id id; bool end; }
  std::string input;

  lexer_t(const std::string& input) : input(input), id(empty) {}
  token_t next_token();
};
```

We will translate the BNF into member functions of the parser, but the
general parser skeleton would look something like this:

```cpp
struct parser_t {
  lexer_t lexer;
  struct value_t { bool end; number_t value; };

  parser_t(const std::string& input) : lexer(input) {}

  // Functions for each of the BNF variables.
  // ...
  
  value_t expr() { return value_t(); }
  value_t term() { return value_t(); }
  value_t factor() { return value_t(); }
  value_t number() { return value_t(); }
};

int main() { auto result = parser_t("1+1").expr(); return 0; }
```

Writing these member functions is what handwritten parsers are all about. Here I have
the example of how I did number():
```cpp
value_t number() {
  value_t v;
  auto token = lexer.next_token();
  if (token.end) { v.end = true; return v; }
  if (token.id != token_id::number) return v;
  
  v.accepted = true;
  v.value = token.value;
  return v;
}
```

As you can see, it just fetches the value of the number
from the lexer, which is where I would want that to happen anyway.
