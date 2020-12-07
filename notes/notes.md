# Notes

记录编译器各部分的大致原理

## 1. 项目内容

语法制导的翻译器、中间代码解释器GUI

1. 编译器部分
   1. 词法分析
   2. 语法分析
   3. 语义分析
   4. 中间代码生成
2. 解释器部分
   1. 执行中间代码
3. GUI部分

## 2. 原理及标准

### 2.1. 编译器

#### 2.1.1. 词法分析

~~通过正则表达式规定词法，然后处理成最小DFA的状态转移表~~

~~处理流程为：~~  
~~正则表达式 -> NFA -> DFA -> 最小DFA~~

通过代码直接从文件读出一个单词。  

- 保留字:int、if、then、else、while、do。
- 标识符:用助记符(或称为单词记号) ID 表示。
- 常数: 用助记符 NUM 表示。
- 算术运算符:+、- 、*、/。
- 逻辑运算符:and、or。
- 关系运算符:<、>、<=、>=、!= 、== 用助记符relop 表示。
- 分隔符:{、}、 ;、(、 )、=。

关键词在`language.txt`中，格式为：

```text
define keywords
<标识符1> 关键字1
<标识符2> 关键字2
```

功能相似的关键字应使用相同的标识符，如`+`和`-`  
这里`<op4>`的数字为c语言运算符优先级对应数字*待定，根据语法分析方式*  
![a](./运算符优先级.jpeg)

如：

```test
define keywords
<type_name> int
<if> if
<else> else
<while> while
<op4> +
<op4> -
<op3> *
<op3> /
<op1(> (
<op1)> )
<;> ;
```

#### 2.1.2. 语法分析

~~通过LL(1)文法描述文法，计算得到LL(1)分析表~~  
~~处理流程为：~~  
~~LL(1)文法 -> First集、Follow集 -> Select集 -> LL(1)分析表 -> 语法树~~

使用递归子程序，代码中直接实现对不同语句翻译，但是使用类似的架构。
语法分析以word为元素，每次调用词法分析获得一个单词，应该在词法分析开始前添加keyword的定义。
句子的定义见language文档。

表达式`expression`除外，递归子程序不能简单的处理表达式内部的优先关系，尤其是从左到右的运算。  
这里使用栈的方式读入并处理表达式，并输出合适的中间代码。

经过递归子程序中子程序的抽象，子程序间可以不传递信息，只有调用关系。  
对于`if`等分支，通过`line_number`获得需要跳到的地方。

老师提供的语法：

翻译表：

|                 中文 | 英文          |
| -------------------: | :------------ |
|           加法运算符 | add_op        |
|           乘法运算符 | mult_op       |
|           关系运算符 | rel_op        |
|                 字母 | letter        |
|                      |               |
|             非零数字 | udigit        |
|                 数字 | digit         |
|                 字符 | ch            |
|               字符串 | str           |
|                      |               |
|           类型标识符 | type          |
|               标识符 | ident         |
|                      |               |
|                      |               |
|                 程序 | prog          |
|                      |               |
|             常量说明 | declare_const |
|             常量定义 | def_const     |
|             变量说明 | declare_var   |
|             变量定义 | def_var       |
|                      |               |
|           无符号整数 | uinteger      |
|                 整数 | integer       |
|                      |               |
|             声明头部 | declare_h     |
|     有返回值函数定义 | f_ret         |
|     无返回值函数定义 | f_void        |
|               参数表 | param_table   |
|                      |               |
|               主函数 | main_f        |
|                      |               |
|             复合语句 | comp_stmt     |
|               语句列 | stmt_list     |
|                 语句 | stmt          |
|             赋值语句 | eval          |
|             条件语句 | cond_stmt     |
|                 条件 | cond          |
|             循环语句 | loop_stmt     |
|                 步长 | step          |
|                      |               |
|               表达式 | exp           |
|                   项 | term          |
|                 因子 | factor        |
|                      |               |
| 有返回值函数调用语句 | f_ret_call    |
| 无返回值函数调用语句 | f_void_call   |
|             值参数表 | arg_list      |
|                      |               |
|               读语句 | r_stmt        |
|               写语句 | w_stmt        |
|             返回语句 | ret_stmt      |

```text
<add_op>        ::=  + | -
<mult_op>       ::=  * | /
<rel_op>        ::=  < | <= | > | >= | != | ==
<letter>        ::=  _ | a | ... | z | A | ... | Z

<udigit>        ::=  1 | ... | 9
<digit>         ::=  0 | <udigit>
<ch>            ::=  '<add_op>' | '<mult_op>' | '<letter>' | '<digit>'
<str>           ::=  "{十进制编码为32,33,35-126的ASCII字符}"

<type>          ::=  int | char
<ident>         ::=  <letter>{<letter> | <digit>}

<prog>          ::=  [<declare_const>][<declare_var>]{<f_ret> | <f_void>}<main_f>

<declare_const> ::=  const<def_const>;{ const<def_const>;}
<def_const>     ::=  int<ident>=<integer>{,<ident>=<integer>} | char<ident>=<ch>{,<ident>=<ch>}
<declare_var>   ::=  <def_var>;{<def_var>;}
<def_var>       ::=  <type>(<ident> | <ident>'['<uinteger>']'){,(<ident> | <ident>'['<uinteger>']' )} //<uinteger>表示数组元素的个数，其值需大于0

<uinteger>      ::=  <udigit>{<digit>} | 0
<integer>       ::=  [+ | -]<uinteger>

<declare_h>     ::=  int<ident> | char<ident>
<f_ret>         ::=  <declare_h>'('<param_table>')' '{'<comp_stmt>'}'
<f_void>        ::=  void<ident>'('<param_table>')''{'<comp_stmt>'}'
<param_table>   ::=  <type><ident>{,<type><ident>} | <空>

<main_f>        ::=  void main‘(’‘)’ ‘{’<comp_stmt>‘}’

<comp_stmt>     ::=  [<declare_const>][<declare_var>]<stmt_list>
<stmt_list>     ::=  {<stmt>}
<stmt>          ::=  <cond_stmt> | <loop_stmt> | '{'<stmt_list>'}' | <f_ret_call>; | <f_void_call>; | <eval>; | <r_stmt>; | <w_stmt>; | <空>; | <ret_stmt>;
<eval>          ::=  <ident>=<exp> | <ident>'['<exp>']'=<exp>
<cond_stmt>     ::=  if '('<cond>')'<stmt>[else<stmt>]
<cond>          ::=  <exp><rel_op><exp> //整型表达式之间才能进行关系运算 | <exp> //表达式为整型，其值为0条件为假，值不为0时条件为真
<loop_stmt>     ::=  while '('<cond>')'<stmt> | do<stmt>while '('<cond>')' | for'('<ident>=<exp>;<cond>;<ident>=<ident>(+ | -)<step>')'<stmt>
<step>          ::=  <uinteger>

<exp>           ::=  [+ | -]<term>{<add_op><term>} //[+ | -]只作用于第一个<term>
<term>          ::=  <factor>{<mult_op><factor>}
<factor>        ::=  <ident> | <ident>'['<exp>']' | '('<exp>')' | <integer> | <ch> | <f_ret_call>

<f_ret_call>    ::=  <ident>'('<arg_list>')'
<f_void_call>   ::=  <ident>'('<arg_list>')'
<arg_list>      ::=  <exp>{,<exp>} | <空>

<r_stmt>        ::=  scanf '('<ident>{,<ident>}')'
<w_stmt>        ::=  printf '(' <str>,<exp> ')' | printf '('<str> ')' | printf '('<exp>')'
<ret_stmt>      ::=  return['('<exp>')']]
```

```text
<加法运算符>   ::= +|-
<乘法运算符>  ::= *|/
<关系运算符>  ::=  <|<=|>|>=|!=|==
<字母>   ::= ＿|a|...|z|A|...|Z

<非零数字>  ::= 1|...|9
<数字>   ::= 0|<非零数字>
<字符>    ::=  '<加法运算符>'|'<乘法运算符>'|'<字母>'|'<数字>'
<字符串>   ::=  "{十进制编码为32,33,35-126的ASCII字符}"

<类型标识符>      ::=  int | char
<标识符>    ::=  <字母>{<字母>|<数字>}

<程序>    ::= [<常量说明>][<变量说明>]{<有返回值函数定义>|<无返回值函数定义>}<主函数>

<常量说明> ::=  const<常量定义>;{ const<常量定义>;}
<常量定义>   ::=   int<标识符>=<整数>{,<标识符>=<整数>}
                  | char<标识符>=<字符>{,<标识符>=<字符>}
<变量说明>  ::= <变量定义>;{<变量定义>;}
<变量定义>  ::= <类型标识符>(<标识符>|<标识符>'['<无符号整数>']'){,(<标识符>|<标识符>'['<无符号整数>']' )}
               //<无符号整数>表示数组元素的个数，其值需大于0


<无符号整数>  ::= <非零数字>{<数字>}|0
<整数>        ::= [+|-]<无符号整数>

<声明头部>   ::=  int<标识符> |char<标识符>
<有返回值函数定义>  ::=  <声明头部>'('<参数表>')' '{'<复合语句>'}'
<无返回值函数定义>  ::= void<标识符>'('<参数表>')''{'<复合语句>'}'
<参数表>    ::=  <类型标识符><标识符>{,<类型标识符><标识符>}| <空>

<主函数>    ::= void main‘(’‘)’ ‘{’<复合语句>‘}’

<复合语句>   ::=  [<常量说明>][<变量说明>]<语句列>
<语句列>   ::= {<语句>}
<语句>    ::= <条件语句>|<循环语句>| '{'<语句列>'}'| <有返回值函数调用语句>; 
               |<无返回值函数调用语句>;|<赋值语句>;|<读语句>;|<写语句>;|<空>;|<返回语句>;
<赋值语句>   ::=  <标识符>=<表达式>|<标识符>'['<表达式>']'=<表达式>
<条件语句>  ::= if '('<条件>')'<语句>[else<语句>]
<条件>    ::=  <表达式><关系运算符><表达式> //整型表达式之间才能进行关系运算
               |<表达式>               //表达式为整型，其值为0条件为假，值不为0时条件为真
<循环语句>   ::=  while '('<条件>')'<语句>| do<语句>while '('<条件>')'
                  |for'('<标识符>=<表达式>;<条件>;<标识符>=<标识符>(+|-)<步长>')'<语句>
<步长>::= <无符号整数>  

<表达式>    ::= [+|-]<项>{<加法运算符><项>}   //[+|-]只作用于第一个<项>
<项>     ::= <因子>{<乘法运算符><因子>}
<因子>    ::= <标识符>|<标识符>'['<表达式>']'|'('<表达式>')'|<整数>|<字符>|<有返回值函数调用语句>         

<有返回值函数调用语句> ::= <标识符>'('<值参数表>')'
<无返回值函数调用语句> ::= <标识符>'('<值参数表>')'
<值参数表>   ::= <表达式>{,<表达式>}|<空>

<读语句>    ::=  scanf '('<标识符>{,<标识符>}')'
<写语句>    ::= printf '(' <字符串>,<表达式> ')'| printf '('<字符串> ')'| printf '('<表达式>')'
<返回语句>   ::=  return['('<表达式>')']   
```

#### 2.1.3. 语义分析

语义分析在语法分析的同时进行，对语义进行静态检查

#### 2.1.4. 中间代码生成

中间代码使用三地址码（四元组）。*待定*

### 2.2. 解释器

### 2.3. GUI

编译器参数(后端python调用)  

```text
./compiler.exe <input> [-l <level>]
<level>: debug|info|warn|error|fatal
```

中间代码输出到./middle.txt (运行结果？)  
样例调用：`./compiler.exe input.txt -l info`
