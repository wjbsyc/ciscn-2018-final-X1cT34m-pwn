
#include "hgmp.h"
#define mpz_class BigInteger

std::string InfixToPostfix(std::string infix);
mpz_class posfixCompute(std::string s);
mpz_class  expressionCalculate(std::string s);
char * eval_expr(unsigned char * expr, int length);
