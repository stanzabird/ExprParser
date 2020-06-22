#include <string>
#include <vector>


namespace parser2 {

  using number_t = int;
  
  
  struct lexer_t {
    std::string input;
    std::string::iterator pos;
    
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
      
      token_t() : end(false), id(empty), value() {}
      
      bool operator==(char c) {
	switch(c)
	  {
	  case '+': if (id == op_add) return true;
	  case '-': if (id == op_sub) return true;
	  case '*': if (id == op_mul) return true;
	  case '/': if (id == op_div) return true;
	  case '(': if (id == paren_open) return true;
	  case ')': if (id == paren_close) return true;
	  }
	return false;
      }
    };

    lexer_t(const std::string& input) : input(input) { pos = this->input.begin(); }
    token_t next_token();
    void reject_token(token_t);
  };

  struct parser_t {
    lexer_t lexer;
    struct value_t {
      bool accepted;
      value_t() : value_t(false) {}
      value_t(bool accepted) : accepted(accepted) {}
      operator bool() { return accepted; }//todo
      
      value_t operator+(const value_t& value) { return value; }//todo
      value_t operator-(const value_t& value) { return value; }//todo
      value_t operator*(const value_t& value) { return value; }//todo
      value_t operator/(const value_t& value) { return value; }//todo
    };


    
    value_t expr() {
      auto token = lexer.next_token();
      auto value = expr(token);
      if (!value) {
	lexer.reject_token(token);
	return false;
      }
      return value;
    }
    
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
      else
	return false;
    }

    value_t number(lexer_t::token_t token) { return false; }
    
  }; // struct parser_t
  
}

int main(int argc,char* argv[]) {
  return 0;
}
