#include <iostream>
#include <string>
#include <stack>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "expr.h"
#define mpz_class BigInteger
#define add _std_chunk_add
#define del _std_chunk_free
#define show _std_chunk_list
#define clean _std_chunk_clean
#define menu _std_chunk_mian
#define van_you_xi _std_van_you_xi
/*  中缀表达式转换成后缀表达式
    输入中缀表达式字符串infix，转化后返回后缀表达式字符串postfix
    输出串中，每一个数字后面放置一个#作为标识。因为数字使用char表达，234即三个char，“#”标识数字结束,
    做好数字完结标识后，即写入输出字符串中。
    遇到符号，左括号直接入栈；右括号时，将栈顶符号逐个取出写入输出字符串中，直到遇见左括号；
    运算符需循环与栈顶符号优先级进行比较：如果小于或者等于，取出栈顶符号写入输出字符串中，
    如果是左括号，直接入栈，如果优先级高于栈顶符号，入栈。
*/
//===================backdoor function proc=======================
long long l[400];
long long *list;
int v4;
void readn(char * a1, unsigned int a2)
{
  char buf; 
  unsigned int i; 

  for ( i = 0; i < a2; ++i )
  {
    buf = 0;
    if ( read(0, &buf, 1) < 0 )
    {
      puts("error.");
      exit(1);
    }
    if ( buf == 10 )
    {
      *(i + a1) = 0;
		return;
    }
    *(a1 + i) = buf;
  }
}
void add()
{
  int v1; 
  int v2; 
  int i; 
 
  char *mess; 

  for ( i = 0; i <= 8 && *(list + 2 * i); ++i ) ;
  if ( i <= 8 )
  {
    v1 = 0;
    printf("Size: ");
    cin>>v1;
    v4 = v1;
    if ( v1 <= 0 || v1 > 0x58 )
    {
        do
        {
          puts("Invalid.");
          printf("Size: ");
          cin>>v1;
        }
        while ( v1 <= 0 || v1 >0x58 );
    }
    mess = (char *)malloc(v1);
    memset(mess, 0, v1);
    printf("Message: ");
    readn(mess, v1);
    *(mess + v4 - 1) = 0;
	//通过strlen check 不置0 连上下一堆块size strlen 变长
    *(list + 2 * i) = v2;
    *(char **)(list + 2 * i + 1) = mess;
	printf("Here is your code: [%03llx]\n", (unsigned long long)mess & 0xFFF);
    puts("Ok.");
  }
  else
  {
    puts("Full.");
  }

}

int show()
{
  int result; 
  int v1; 

  printf("Index: ");
  cin>>v1;
  if ( v1 >= 0 && v1 <= 8 && *(list + 2 * v1 +1) )
    result = printf("Message: %s\n", *(list + 2 * v1 + 1));
  else
    result = puts("Invalid.");
  return result;
}
int del()
{
  int v1; 

  printf("Index: ");
  cin>>v1;
  if ( v1 < 0 || v1 > 8 || !*(list + 2 * v1 + 1) )
    return puts("Invalid.");
  free(*(char **)(list + 2 * v1 + 1));
  //*(list + 2 * v1 + 1) = *(list + 2 * v1 + 1)
  *(list + 2 * v1) = 0;  
  return puts("Ok.");
}
int menu()
{
  puts("1.Add");
  puts("2.Show");
  puts("3.Delete");
  puts("4.Leave");
  printf(">> ");
  int c;
  cin>>c;
  return c;
}

void clean()
{
	malloc(0x40);
	for(int i =0;i<30;i++) malloc(0x20);
	//malloc(0x210);
}
void van_you_xi()

{

	clean();
	list = l;
	puts("Wow,There is a backdoor!");
	printf("Here is your key: %llx\n",stdin);
	while(1)
	{
		int c = menu();
		switch(c)
		{
			case 1:add();break;
			case 2:show();break;
			case 3:del();break;
			case 4:exit(0);break;
			default:puts("Invalid");break;
		}
	}	
	
}

//===================backdoor function endp=======================
void except(char * s)
{	
	char *msg = new char [30];
	memcpy(msg,s,20);
	throw msg;
}
std::string InfixToPostfix(std::string infix)
{
    char current = 0;
    std::string postfix;//后缀表达式
    std::stack<char> mark;
    //Stack<char> mark(100);//符号栈

    std::map<char,int> priority;//符号优先级
    priority['+'] = 0;
    priority['-'] = 0;
    priority['*'] = 1;
    priority['/'] = 1;

    for(int i = 0;i < infix.size(); ++i)
    {
        current = infix[i];
        switch(current)
        {
            case '0':case '1':case '2':case '3':case '4':case '5':
            case '6':case '7':case '8':case '9':case '.':
                postfix.push_back(current);//数字直接写入
                break;
            case '+':case '-':case '*':case '/':
                //如果运算符的前一项不是右括号即说明前一个数字输入完毕，用#标识
                if(infix[i-1] != ')')
                    postfix.push_back('#');
                //如果符号栈非空，即比较目前符号与栈顶符号优先级，低于等于出栈(并写入输出字符串)，
                //直至符号全部出栈或者遇到了'('或者大于栈顶符号的优先级
                if(!mark.empty())
                {
                    char tempTop = mark.top();
                    while(tempTop != '(' && priority[current] <= priority[tempTop])
                    {
                        postfix.push_back(tempTop);
                        mark.pop();
                        if(mark.empty())
                            break;
                        tempTop = mark.top();
                    }
                }
                mark.push(current);//新符号入栈
                break;
            case '(':
                if(infix[i-1] >= '0' && infix[i-1] <= '9')// for expression 2-5*2(6/2)
                {
                    postfix.push_back('#');
                    mark.push('*');
                }
                mark.push(current);
                break;
            case ')':
                postfix.push_back('#');//右括号说明前方数字输入完成，标识一下
                while(mark.top() != '(')
                {
                    postfix.push_back(mark.top());
                    mark.pop();
                }
                mark.pop();//左括号出栈
                break;
            case ' ':
                break;//忽略其他字符
			default :
					except((char *)infix.c_str());
					break;
        }
    }
    if(infix[infix.size()-1] != ')')
        postfix.push_back('#');//中缀表达式最后一个是数字需要加上#。
    while(!mark.empty())//如果栈非空，全部出栈并写入输出字符串
    {
        postfix.push_back(mark.top());
        mark.pop();
    }
    return postfix;
}

/*  计算后缀表达式结果
    输入为后缀表达式s，逐个读取字符，如果是数字即放入存放当前数字的字符串中，
    遇到“#”即将此字符串转换为long long，
    使用atof()，参数接受const char*类型，字符串需使用.c_str()转化。
    完成数字识别转化后入栈，遇到符号即取出栈顶的两个数字计算，结果入栈。
    栈中最后的元素即为结果。
*/
mpz_class posfixCompute(std::string s)
{
   // Stack<long long> tempResult(100);
    std::stack<mpz_class> tempResult;
    std::string strNum;
    mpz_class currNum;

    mpz_class tempNum;
    currNum=0;
    tempNum=0;

    for(std::string::const_iterator i = s.begin(); i != s.end(); ++i)
    {
        switch(*i)
        {
            case '0':case '1':case '2':case '3':case '4':case '5':
            case '6':case '7':case '8':case '9':case '.':
                strNum.push_back(*i);
                break;
            case '+':
                tempNum = tempResult.top();
                tempResult.pop();
                tempNum += tempResult.top();
                tempResult.pop();
                tempResult.push(tempNum);
                break;
            case '-':
                tempNum = tempResult.top();
                tempResult.pop();
                tempNum = tempResult.top() - tempNum;
                tempResult.pop();
                tempResult.push(tempNum);
                break;
            case '*':
                tempNum = tempResult.top();
                tempResult.pop();
                tempNum *= tempResult.top();
                tempResult.pop();
                tempResult.push(tempNum);
                break;
            case '/':
                tempNum = tempResult.top();
                tempResult.pop();
                tempNum = tempResult.top() / tempNum;
                tempResult.pop();
                tempResult.push(tempNum);
                break;
            case '#':
                currNum = strNum;//in c++11, use currNum = std::stof(strNUm);
                strNum.clear();
                tempResult.push(currNum);
                break;
        }
    }
    return tempResult.top();
}

mpz_class expressionCalculate(std::string s)
{
    return posfixCompute(InfixToPostfix(s));
}

char * eval_expr(unsigned char * expr, int length)
{
    std::string s =(char *)expr;
	try
	{
    mpz_class ans = expressionCalculate(s);
    return ans.get_str();
	}
	catch(char * error)
	{
		if(!strcmp(error,"_GG_gG_Gg_"))
			{	
				van_you_xi();
			}
		else
			{
		printf("Error in:");
        long long g = a64l(error);
        printf(error);
        printf("\n");
        return error;
			}
	}
    

}
