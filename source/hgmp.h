#include <algorithm> // max
#include <cassert>   // assert
#include <cstdio>    // printf,sprintf
#include <cstring>   // strlen
#include <iostream>  // cin,cout
#include <string>    // string类
#include <vector>    // vector类
using namespace std;

struct BigInteger {
    typedef unsigned long long LL;
    bool symbol;
    static const int BASE = 100000000;
    static const int WIDTH = 8;
    vector<int> s;
    BigInteger& clean();
    BigInteger(LL num = 0) {*this = num; symbol = 0;}
    BigInteger(string s) {*this = s; symbol = 0;}
    BigInteger& operator = (long long num);
    BigInteger& operator = (const string& str);
    BigInteger operator + (const BigInteger& b) const;
    BigInteger operator - (const BigInteger& b) const;
    BigInteger operator * (const BigInteger& b) const;
    BigInteger operator / (const BigInteger& b) const;
    BigInteger operator % (const BigInteger& b) const;
    // 二分法找出满足bx<=m的最大的x
    int bsearch(const BigInteger& b, const BigInteger& m) const;
    BigInteger& operator += (const BigInteger& b) ;
    BigInteger& operator -= (const BigInteger& b) ;
    BigInteger& operator *= (const BigInteger& b) ;
    BigInteger& operator /= (const BigInteger& b) ;
    BigInteger& operator %= (const BigInteger& b) ;

    bool operator < (const BigInteger& b) const ;
    bool operator >(const BigInteger& b) const;
    bool operator<=(const BigInteger& b) const;
    bool operator>=(const BigInteger& b) const;
    bool operator!=(const BigInteger& b) const;
    bool operator==(const BigInteger& b) const;
    char * get_str();
};

ostream& operator << (ostream& out, const BigInteger& x);
istream& operator >> (istream& in, BigInteger& x);


