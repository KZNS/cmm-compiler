# Notes

记录编译器各部分的大致原理

- [1. 项目内容](#1-项目内容)
- [2. 原理及标准](#2-原理及标准)
  - [2.1. 编译器](#21-编译器)
    - [2.1.1. 词法分析](#211-词法分析)
    - [2.1.2. 语法分析](#212-语法分析)
      - [2.1.2.1. 语法非终结符翻译表](#2121-语法非终结符翻译表)
      - [2.1.2.2. 语法规则表](#2122-语法规则表)
      - [2.1.2.3. 错误信息表](#2123-错误信息表)
      - [2.1.2.4. 语法部分工作说明](#2124-语法部分工作说明)
    - [2.1.3. 语义分析](#213-语义分析)
    - [2.1.4. 中间代码生成](#214-中间代码生成)
  - [2.2. 解释器](#22-解释器)
  - [2.3. GUI](#23-gui)

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

##### 2.1.2.1. 语法非终结符翻译表

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

##### 2.1.2.2. 语法规则表

|          非终结符 | 定义                                                                           | 备注                                               |
| ----------------: | :----------------------------------------------------------------------------- | :------------------------------------------------- |
|        `<add_op>` | `+ | -`                                                                        |
|       `<mult_op>` | `* | /`                                                                        |
|        `<rel_op>` | `< | <= | > | >= | != | ==`                                                    |
|        `<letter>` | `_ | a | ... | z | A | ... | Z`                                                |
|                   |                                                                                |
|        `<udigit>` | `1...9`                                                                        |
|         `<digit>` | `0 | <udigit>`                                                                 |
|            `<ch>` | `'<add_op>' | '<mult_op>' | '<letter>' | '<digit>'`                            |
|           `<str>` | `"{十进制编码为32,33,35-126的ASCII字符}"`                                      |
|                   |                                                                                |
|          `<type>` | `int | char`                                                                   |
|         `<ident>` | `<letter>{<letter>|<digit>}`                                                   |
|                   |                                                                                |
|          `<prog>` | `[<declare_const>][<declare_var>]{<f_ret>|<f_void>}<main_f>`                   |
|                   |                                                                                |
| `<declare_const>` | `const<def_const>;{ const<def_const>;}`                                        |
|     `<def_const>` | `int<ident>=<integer>{,<ident>=<integer>}`                                     |
|                   | `char<ident>=<ch>{,<ident>=<ch>}`                                              |
|   `<declare_var>` | `<def_var>;{<def_var>;}`                                                       |
|       `<def_var>` | `<type>(<ident>|<ident>'['<uinteger>']'){,(<ident>|<ident>'['<uinteger>']' )}` | `<uinteger>表示数组元素的个数，其值需大于0`        |
|                   |                                                                                |
|      `<uinteger>` | `<udigit>{<digit>}`                                                            |
|                   | `0`                                                                            |
|       `<integer>` | `[+|-]<uinteger>`                                                              |
|                   |                                                                                |
|     `<declare_h>` | `int<ident>`                                                                   |
|                   | `char<ident>`                                                                  |
|         `<f_ret>` | `<declare_h>'('<param_table>')' '{'<comp_stmt>'}'`                             |
|        `<f_void>` | `void<ident>'('<param_table>')''{'<comp_stmt>'}'`                              |
|   `<param_table>` | `<type><ident>{,<type><ident>}`                                                |
|                   | `<空>`                                                                         |
|                   |                                                                                |
|        `<main_f>` | `void main‘(’‘)’ ‘{’<comp_stmt>‘}’`                                            |
|                   |                                                                                |
|     `<comp_stmt>` | `[<declare_const>][<declare_var>]<stmt_list>`                                  |
|     `<stmt_list>` | `{<stmt>}`                                                                     |
|          `<stmt>` | `<cond_stmt>`                                                                  |
|                   | `<loop_stmt>`                                                                  |
|                   | `'{'<stmt_list>'}'`                                                            |
|                   | `<f_ret_call>;`                                                                |
|                   | `<f_void_call>;`                                                               |
|                   | `<eval>;`                                                                      |
|                   | `<r_stmt>;`                                                                    |
|                   | `<w_stmt>;`                                                                    |
|                   | `<空>;`                                                                        |
|                   | `<ret_stmt>;`                                                                  |
|          `<eval>` | `<ident>=<exp>`                                                                |
|                   | `<ident>'['<exp>']'=<exp>`                                                     |
|     `<cond_stmt>` | `if '('<cond>')'<stmt>[else<stmt>]`                                            |
|          `<cond>` | `<exp><rel_op><exp>`                                                           | `整型表达式之间才能进行关系运算`                   |
|                   | `<exp>`                                                                        | `表达式为整型，其值为0条件为假，值不为0时条件为真` |
|     `<loop_stmt>` | `while '('<cond>')'<stmt>`                                                     |
|                   | `do<stmt>while '('<cond>')'`                                                   |
|                   | `for'('<ident>=<exp>;<cond>;<ident>=<ident>(+|-)<step>')'<stmt>`               |
|          `<step>` | `<uinteger>`                                                                   |
|                   |                                                                                |
|           `<exp>` | `[+|-]<term>{<add_op><term>}`                                                  | `[+|-]只作用于第一个<term>`                        |
|          `<term>` | `<factor>{<mult_op><factor>}`                                                  |
|        `<factor>` | `<ident>`                                                                      |
|                   | `<ident>'['<exp>']'`                                                           |
|                   | `'('<exp>')'`                                                                  |
|                   | `<integer>`                                                                    |
|                   | `<ch>`                                                                         |
|                   | `<f_ret_call>`                                                                 |
|                   |                                                                                |
|    `<f_ret_call>` | `<ident>'('<arg_list>')'`                                                      |
|   `<f_void_call>` | `<ident>'('<arg_list>')'`                                                      |
|      `<arg_list>` | `<exp>{,<exp>}`                                                                |
|                   | `<空>`                                                                         |
|                   |                                                                                |
|        `<r_stmt>` | `scanf '('<ident>{,<ident>}')'`                                                |
|        `<w_stmt>` | `printf '(' <str>,<exp> ')'`                                                   |
|                   | `printf '('<str> ')'`                                                          |
|                   | `printf '('<exp>')'`                                                           |
|      `<ret_stmt>` | `return['('<exp>')']`                                                          |

##### 2.1.2.3. 错误信息表

| Code | Grammar | Semantic | Error Describe                                       |
| ---- | ------- | -------- | ---------------------------------------------------- |
| a    | 1       |          | 非法符号或不符合词法                                 |
| b    |         | 1        | 名字重定义                                           |
| c    |         | 1        | 未定义的名字                                         |
| d    |         | 1        | 函数参数个数不匹配                                   |
| e    |         | 1        | 函数参数类型不匹配                                   |
| f    |         | 1        | 条件判断中出现不合法的类型                           |
| g    | 1       | 1        | 无返回值的函数存在不匹配的return语句                 |
| h    | 1       | 1        | 有返回值的函数缺少return语句或存在不匹配的return语句 |
| i    |         | 1        | 数组元素的下标只能是整型表达式                       |
| j    |         | 1        | 不能改变常量的值                                     |
| k    | 1       |          | 应为分号                                             |
| l    | 1       |          | 应为右小括号’)’                                      |
| m    | 1       |          | 应为右中括号’]’                                      |
| n    | 1       |          | do-while应为语句中缺少while                          |
| o    | 1       |          | 常量定义中=后面只能是整型或字符型常量                |

##### 2.1.2.4. 语法部分工作说明

1. git部分
   1. 应该在 `grammatical-analysis` 分支进行语法分析部分的函数填写
   2. 使用 `git pull --rebase` 代替 `git pull` ，以保证本分支的线性
   3. commit message 应该使用 `feat(grammar): xxxxx` 等的格式，表示这一部分的修改是在 grammar 模块下
   4. 目前使用的模块名有：
      1. lexical 词法分析部分
      2. grammar 语法分析部分
      3. compiler 编译器部分
      4. web部分应该设定对应的模块名，用于括号中
2. 代码部分
   1. 根据课程需要，应该在每个语法分析函数的*最后*输出本语句的信息
   2. 在每个语法分析函数进入前，`word` 中应该保存即将分析的一个单词
   3. 应只使用 `word`, `get_word()`, `roll_back()` 来完成语法分析，而不直接访问`word_buffer`
   4. 使用 `word` 时，应该尽可能使用 `word.first` 即单词的*类别码*来进行处理
   5. 在进入每个语法分析函数前，应该判断是否应该进入，而不是让函数发现错误之后返回
   6. 属性的下传与返回通过*参数*和*引用参数*进行，根据需要在代码编写中修改参数表即可，注意同时修改 `.cpp` 和 `.h`
   7. 函数的*返回值*使用 `int`：
      1. 负数：错误代码，返回运行中出现错误类别
      2. 0：正常执行，没有特殊状态发生
      3. 正数：状态代码，返回运行得到的特殊状态

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
