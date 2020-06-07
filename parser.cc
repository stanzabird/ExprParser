#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


using number_t = int;


struct lexer_t {
  enum token_id
    {
     op_add, op_sub, op_mul, op_div,
     paren_open, paren_close,
     number, empty
    };
  std::string input;
  std::string::iterator pos;
  struct token_t { token_t() : end(false), id(token_id::empty), value() {} bool end; token_id id; number_t value; };
  lexer_t(const std::string& input) : input(input) { pos = this->input.begin(); }
  token_t next_token();
};


/*
  expr   := expr '+' term | term;
  term   := term '*' factor | factor;
  factor := '(' expr ')' | number;
 */


struct parser_t {
  lexer_t lexer;
  using token_id = lexer_t::token_id;
  struct value_t { value_t() : end(false), accepted(false) {} bool end; bool accepted; number_t value; };
  parser_t(const std::string& input) : lexer(input) {}

  value_t expr();

  value_t term() {
    value_t v;
    
    auto token = lexer.next_token();  
    if (token.end) { v.end = true; return v; }
    if (token.id == token_id::number) {
      v.accepted = true; v.value = token.value; return v;
    }
    else if (token.id == token_id::paren_open) {
      auto v2 = expr();
      if (v2.end) return v2;
      if (v2.accepted) {
	auto token2 = lexer.next_token();
	if (token2.id != token_id::paren_close) return v;
	return v2;
      }
    }
    return v;
  }
  
  value_t number() {
    value_t v;
    auto token = lexer.next_token();
    if (token.end) { v.end = true; return v; }
    if (token.id != token_id::number) return v;
    
    v.accepted = true;
    v.value = token.value;
    return v;
  }
};


int main(int argc, char* argv[]) {

  std::vector<std::string> args(argv+1,argv+argc);
  std::stringstream ss;
  for (auto arg : args ) { ss << " " << arg; } 
  parser_t parser(ss.str());
  
  parser_t::value_t v = parser.expr();
  
  if (v.accepted == true) {
    std::cout << v.value << "\n";
  }
  else {
    std::cout << "Parse error.\n";
  }
  
  return 0;
}






// the ugly part...
lexer_t::token_t lexer_t::next_token() {
  token_t t;

  // sanity checking...
  if (input == "") { t.end = true; return t; }
  if (pos == input.end()) { t.end = true; return t; }

  if (pos == input.begin()) {
    // first token
  }

  // eat whitespace
  while (isspace(*pos)) ++pos;

  // math operators..
  if (*pos == '+') { t.id = token_id::op_add; ++pos; return t; }
  if (*pos == '-') { t.id = token_id::op_sub; ++pos; return t; }
  if (*pos == '*') { t.id = token_id::op_mul; ++pos; return t; }
  if (*pos == '/') { t.id = token_id::op_div; ++pos; return t; }

  if (*pos == '(') { t.id = token_id::paren_open; ++pos; return t; }
  if (*pos == ')') { t.id = token_id::paren_close; ++pos; return t; }

  // now only we have number_t to parse..
  std::string s{pos,input.end()};
  auto p = s.find_first_not_of("0123456789");

  number_t n;
  
  if (p == std::string::npos) {
    n = std::stoi(std::string(begin(s), end(s)));
    pos = input.end();
  }
  else {
    n = std::stoi(std::string(begin(s), begin(s)+p));
    pos += p; // eww..
  }
  
  t.id = token_id::number;
  t.value = n;
  return t;
}






  parser_t::value_t parser_t::expr() {
    value_t v,v1,v2;
    
    auto token1 = lexer.next_token();
    if (token1.end) { v.end = true; return v; }
    if (token1.id != token_id::number) return v;
    
    auto token2 = lexer.next_token();
    if (token2.end) { v.end = true; return v; }
    if (token2.id != token_id::op_add
	&& token2.id != token_id::op_sub
	&& token2.id != token_id::op_mul
	&& token2.id != token_id::op_div) return v;

    auto token3 = lexer.next_token();
    if (token3.end) { v.end = true; return v; }
    if (token3.id != token_id::number) return v;

    // ok we're set for the actual calculation...
    if (token2.id == token_id::op_add) {
      v.accepted = true;
      v.value = token1.value + token3.value;
      return v;
    }
    else if (token2.id == token_id::op_sub) {
      v.accepted = true;
      v.value = token1.value - token3.value;
      return v;
    }
    else if (token2.id == token_id::op_mul) {
      v.accepted = true;
      v.value = token1.value * token3.value;
      return v;
    }
    else if (token2.id == token_id::op_div) {
      v.accepted = true;
      v.value = token1.value / token3.value;
      return v;
    }
    else {
      // this should not happen..
      v.end = true;
      return v;
    }
  }

