namespace parser2 {

  
  struct lexer_t {
    struct token_t {
      bool operator==(char c) { return true; }//todo
    };

    token_t next_token();
    void reject_token(token_t);
  };

  struct parser_t {
    lexer_t lexer;
    struct value_t {
      bool accepted;
      value_t() : value_t(false) {}
      value_t(bool accepted) : accepted(accepted) {}
      value_t operator+(const value_t& value) { return value; }//todo
      value_t operator*(const value_t& value) { return value; }//todo
      operator bool() { return accepted; }//todo
    };


    
    value_t expr() {
      auto token = lexer.next_token();
      auto value = expr(token);
      if (!value)
	lexer.reject_token(token);
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
	else {
	  lexer.reject_token(token2);
	}
      }
      return value;
    }


    value_t factor(lexer_t::token_t token) { return false; }


    
  };
  
}

int main(int argc,char* argv[]) {
  return 0;
}
