#include "hgmp.h"
using namespace std;
BigInteger& BigInteger::clean(){while(!s.back()&&s.size()>1)s.pop_back(); return *this;}

BigInteger& BigInteger::operator = (const string& str) {
        s.clear();
        int x, len = (str.length() - 1) / WIDTH + 1;
        for (int i = 0; i < len; i++) {
            int end = str.length() - i*WIDTH;
            int start = max(0, end - WIDTH);
            sscanf(str.substr(start,end-start).c_str(), "%d", &x);
            s.push_back(x);
        }
        return (*this).clean();
    }
BigInteger& BigInteger::operator = (long long num) {
        s.clear();
        do {
            s.push_back(num % BASE);
            num /= BASE;
        } while (num > 0);
        return *this;
    }
BigInteger BigInteger::operator + (const BigInteger& b) const {
	if(symbol == b.symbol){
        BigInteger c; c.s.clear();
        c.symbol=symbol;
        for (int i = 0, g = 0; ; i++) {
            if (g == 0 && i >= s.size() && i >= b.s.size()) break;
            int x = g;
            if (i < s.size()) x += s[i];
            if (i < b.s.size()) x += b.s[i];
            c.s.push_back(x % BASE);
            g = x / BASE;
        }
        return c;
    }
    else
    {
    	if(symbol)
    	{
    		BigInteger c; c.s.clear();
    		c=*this;
    		c.symbol = 0;
    		BigInteger d = b-c;
    		return d;
    	}
    	else
    	{
    		BigInteger c; c.s.clear();
    		c = b;
    		c.symbol = 0;
    		BigInteger d = *this - c ;
    		return d;
    	}
    }
    }
BigInteger BigInteger::operator - (const BigInteger& b) const {
        if(b <= *this && !symbol && !b.symbol) {
        BigInteger c; c.s.clear();
        for (int i = 0, g = 0; ; i++) {
            if (g == 0 && i >= s.size() && i >= b.s.size()) break;
            int x = s[i] + g;
            if (i < b.s.size()) x -= b.s[i];
            if (x < 0) {g = -1; x += BASE;} else g = 0;
            c.s.push_back(x);
        }
        return c;
    	}
    	if(b > *this && !symbol && !b.symbol)
    	{   
    		BigInteger c; c.s.clear();
    		c = b - *this;
    		c.symbol = 1;
    		
    		return c;
    	}
    	if(symbol && b.symbol)
    	{
    		BigInteger c = b;
    		c.symbol = 0;
    		return *this + c;
    	}
       if(symbol != b.symbol)
       {
       		BigInteger x = *this, y = b ,c;
       		c.s.clear();
       		x.symbol=0;
       		y.symbol=0;
       		c = x+y;
       		c.symbol = symbol;
       		return c;
       }

    }
BigInteger BigInteger::operator * (const BigInteger& b) const {
        int i, j; LL g;
        vector<LL> v(s.size()+b.s.size(), 0);
        BigInteger c; c.s.clear();
        for(i=0;i<s.size();i++) for(j=0;j<b.s.size();j++) v[i+j]+=LL(s[i])*b.s[j];
        for (i = 0, g = 0; ; i++) {
            if (g ==0 && i >= v.size()) break;
            LL x = v[i] + g;
            c.s.push_back(x % BASE);
            g = x / BASE;
        }
        c.symbol = (symbol != b.symbol);
        return c.clean();
    }
BigInteger BigInteger::operator / (const BigInteger& b) const {
        // 除数必须大于0
        BigInteger c = *this;       // 商:主要是让c.s和(*this).s的vector一样大
        BigInteger m;               // 余数:初始化为0
        for (int i = s.size()-1; i >= 0; i--) {
            m = m*BASE + s[i];
            c.s[i] = bsearch(b, m);
			m -= b*c.s[i];
        }
        c.symbol = (symbol != b.symbol);
        return c.clean();
    }
BigInteger BigInteger::operator % (const BigInteger& b) const { //方法与除法相同
        BigInteger c = *this;
        BigInteger m;
        for (int i = s.size()-1; i >= 0; i--) {
            m = m*BASE + s[i];
            c.s[i] = bsearch(b, m);
            m -= b*c.s[i];
        }
        return m;
    }
int BigInteger::bsearch(const BigInteger& b, const BigInteger& m) const{
        int L = 0, R = BASE-1, x;
        while (1) {
            x = (L+R)>>1;
            if (b*x<=m) {if (b*(x+1)>m) return x; else L = x;}
            else R = x;
        }
    }

BigInteger& BigInteger::operator += (const BigInteger& b) {*this = *this + b; return *this;}
BigInteger& BigInteger::operator -= (const BigInteger& b) {*this = *this - b; return *this;}
BigInteger& BigInteger::operator *= (const BigInteger& b) {*this = *this * b; return *this;}
BigInteger& BigInteger::operator /= (const BigInteger& b) {*this = *this / b; return *this;}
BigInteger& BigInteger::operator %= (const BigInteger& b) {*this = *this % b; return *this;}

bool BigInteger::operator < (const BigInteger& b) const {
		if(symbol!=b.symbol) return symbol; 
        if (s.size() != b.s.size()) return symbol?(s.size() > b.s.size()):(s.size() < b.s.size());
        for (int i = s.size()-1; i >= 0; i--)
            if (s[i] != b.s[i]) return symbol? (s[i] > b.s[i]):(s[i] < b.s[i]);
        return symbol?true:false;
    }
bool BigInteger::operator >(const BigInteger& b) const{return b < *this;}
bool BigInteger::operator<=(const BigInteger& b) const{return !(b < *this);}
bool BigInteger::operator>=(const BigInteger& b) const{return !(*this < b);}
bool BigInteger::operator!=(const BigInteger& b) const{return b < *this || *this < b;}
bool BigInteger::operator==(const BigInteger& b) const{return !(b < *this) && !(b > *this);}
char * BigInteger::get_str()
    {
    char * tostr = new char [200];
    memset(tostr,0,200);
    if(symbol) sprintf(tostr,"-%d",this -> s.back());
    else sprintf(tostr,"%d",this -> s.back());
    for (int i = this -> s.size()-2; i >= 0; i--) {
        char buf[20];
        sprintf(buf, "%08d", this -> s[i]);
        strcat(tostr,buf);
    }
    return tostr;
    }
istream& operator >> (istream& in, BigInteger& x) {
    string s;
    if (!(in >> s)) return in;
    x = s;
    return in;
}
ostream& operator << (ostream& out, const BigInteger& x) {
	if(x.symbol) out<<"-";
    out << x.s.back();
    for (int i = x.s.size()-2; i >= 0; i--) {
        char buf[20];
        sprintf(buf, "%08d", x.s[i]);
        for (int j = 0; j < strlen(buf); j++) out << buf[j];
    }
    return out;
}
