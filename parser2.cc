#include <iostream>

#include <string>
#include <stack>



namespace parser2 {

  
  using number_t = int;


  
  
  struct lexer_t {
    std::string input;
    std::string::iterator pos;
    
    lexer_t(const std::string& input) : input(input) { pos = this->input.begin(); }
    
    struct token_t {  
      enum token_id
	{
	  op_add, op_sub, op_mul, op_div,
	  paren_open, paren_close,
	  number, empty
	};
      
      bool end;
      token_id id;
      number_t value;
      
      token_t() : end(false), id(token_t::token_id::empty), value() {}
      
      bool operator==(char c) {
	switch(c)
	  {
            case '+': if (id == op_add) return true; break;
            case '-': if (id == op_sub) return true; break;
            case '*': if (id == op_mul) return true; break;
            case '/': if (id == op_div) return true; break;
            case '(': if (id == paren_open) return true; break;
            case ')': if (id == paren_close) return true; break;
	  }
	return false;
      }
    }; // struct token_t
    

    std::stack<token_t> tokens;

    token_t next_token() {
      // see if there are tokens available on the stack
      if (!tokens.empty()) {
	auto retval = tokens.top();
	tokens.pop();
	return retval;
      }
      return parse_token();
    }
    void reject_token(token_t token) {
      tokens.push(token);
    }
    
  private:
    token_t parse_token();
  };


  
  struct parser_t {
    lexer_t lexer;

    parser_t(const std::string& input) : lexer(input) {
    }

    
    struct value_t {
      bool accepted, end;
      number_t value;
      
      value_t() : value_t(false) {}
      value_t(bool accepted) : accepted(accepted), end(false), value() {}
      
      operator bool() { return accepted && !end; }
      
      value_t operator+(const value_t& val) { value_t retval(true); retval.value = this->value + val.value; return retval; }
      value_t operator-(const value_t& val) { value_t retval(true); retval.value = this->value - val.value; return retval; }
      value_t operator*(const value_t& val) { value_t retval(true); retval.value = this->value * val.value; return retval; }
      value_t operator/(const value_t& val) { value_t retval(true); retval.value = this->value / val.value; return retval; }
    };


    
    // entry point for the parser, returns the value of the expression.
    number_t parse() {
      auto token = lexer.next_token();
      auto val = expr(token);

      if (!val) {
        std::cout << "error: syntax error in input.\n";
        exit(1);
      }

      // check to see if we got the entire expression
      auto token2 = lexer.next_token();
      if (token2.end != true) {
        std::cout << "error: trailing garbage in input.\n";
        exit(1);
      }
      
      return val.value;
    }

    //
    // syntax...
    //
    
    value_t expr(lexer_t::token_t token) {
      auto value = term(token);
      if (value) {
	
        auto token2 = lexer.next_token();

	if (token2 == '+') {
	  auto token3 = lexer.next_token();
	  auto value2 = term(token);
	  if (value2) {
            return value + value2;
	  }
	  else {
	    lexer.reject_token(token3);
	    lexer.reject_token(token2);
	    return false;
	  }
	}
	
	else if (token2 == '-') {
	  auto token3 = lexer.next_token();
	  auto value2 = term(token);
	  if (value2) {
            return value - value2;
	  }
	  else {
	    lexer.reject_token(token3);
	    lexer.reject_token(token2);
	    return false;
	  }
	}
	
	else {
	  lexer.reject_token(token2);
          return value;
	}
      }

      return value;
    }

    value_t term(lexer_t::token_t token) {
      auto value = factor(token);
      if (value) {
	auto token2 = lexer.next_token();
	
	if (token2 == '*') {
	  auto token3 = lexer.next_token();
	  auto value2 = term(token);
	  if (value2) {
	    return value * value2;
	  }
	  else {
	    lexer.reject_token(token3);
	    lexer.reject_token(token2);
	    return false;
	  }
	}
	
	else if (token2 == '/') {
	  auto token3 = lexer.next_token();
	  auto value2 = term(token);
	  if (value2) {
	    return value / value2;
	  }
	  else {
	    lexer.reject_token(token3);
	    lexer.reject_token(token2);
	    return false;
	  }
	}
	
	else {
	  lexer.reject_token(token2);
          return value;
	}
      }
      return value;
    }


    value_t factor(lexer_t::token_t token) {
      auto value = number(token);
      if (value) {
	return value;
      }
      else if (token == '(') {
	auto token2 = lexer.next_token();
	auto value2 = expr(token2);
        if (value2) {
          auto token3 = lexer.next_token();
          if (token3 == ')') {
            return value2;
          }
          else {
            lexer.reject_token(token3);
            lexer.reject_token(token2);
            return false;
          }
        }
      }

      return false;
    }

    value_t number(lexer_t::token_t token) {
      value_t v;
      
      if (token.end) { v.end = true; return v; }
      if (token.id != lexer_t::token_t::token_id::number) return v;
    
      v.accepted = true;
      v.value = token.value;
      return v;
    }
    
  }; // struct parser_t
  
}







int main(int argc,char* argv[]) {
  
  if (argc == 1) {
    std::cout << "Use: " << argv[0] << " <expression> ...\n";
    return 1;
  }
  
  for (int i = 1; i < argc; i++) {
    std::cout << argv[i] << " -> ";
    std::cout << parser2::parser_t(argv[i]).parse() << "\n";
  }
  
  return 0;
}








// less interesting functionality... the tokenizer.
namespace parser2 {
  
  lexer_t::token_t lexer_t::parse_token() {
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
    if (*pos == '+') { t.id = token_t::token_id::op_add; ++pos; return t; }
    if (*pos == '-') { t.id = token_t::token_id::op_sub; ++pos; return t; }
    if (*pos == '*') { t.id = token_t::token_id::op_mul; ++pos; return t; }
    if (*pos == '/') { t.id = token_t::token_id::op_div; ++pos; return t; }
    
    if (*pos == '(') { t.id = token_t::token_id::paren_open; ++pos; return t; }
    if (*pos == ')') { t.id = token_t::token_id::paren_close; ++pos; return t; }

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
  
    t.id = token_t::token_id::number;
    t.value = n;
  
    return t;
  }
  
} // namespace parser2


