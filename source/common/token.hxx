#ifndef TOKEN_HXX
#define TOKEN_HXX

namespace bamboo {

class Token {
public:
	Token() {};
	Token(const Token &rhs);
	Token& operator=(const Token &rhs);
	virtual const char *get_orig_token() const = 0; 
	virtual unsigned short get_pos() const = 0;
	virtual ~Token() {};
};

}; /* namespace bamboo */

#endif
