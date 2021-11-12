## 待办清单

### 功能部分

- [ ] 变量栈空间管理
- [ ] 函数定义与调用

### 表达式解析部分

- [ ] ```python
  file_input: (NEWLINE | stmt)* EOF;
  ```
  
- [ ] ```python
  funcdef: 'def' NAME parameters ':' suite;
  ```

- [ ] ```python
  parameters: '(' typedargslist? ')';
  ```
  
- [ ] ```python
  typedargslist: (tfpdef ('=' test)? (',' tfpdef ('=' test)?)*);
  ```
  
- [ ] ```python
  tfpdef: NAME ;
  ```

- [ ] ```python
  stmt: simple_stmt | compound_stmt;
  ```
  
- [ ] ```python
  simple_stmt: small_stmt  NEWLINE;
  ```
  
- [ ] ```python
  small_stmt: expr_stmt | flow_stmt;
  ```
  
- [x] ```python
  expr_stmt: testlist ( (augassign testlist) | ('=' testlist)*);  # 连等 加等/减等/...
  ```
  
- [x] ```python
  augassign: ('+=' | '-=' | '*=' | '/=' | '//=' | '%=' );
  ```
  
- [ ] ```python
  flow_stmt: break_stmt | continue_stmt | return_stmt;
  ```
  
- [ ] ```python
  break_stmt: 'break';
  ```
- [ ] ```python
  continue_stmt: 'continue';
  ```
  
- [ ] ```python
  return_stmt: 'return' (testlist)?;
  ```
  
- [ ] ```python
  compound_stmt: if_stmt | while_stmt | funcdef ;
  ```
  
- [ ] ```python
  if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ('else' ':' suite)?;
  ```
  
- [ ] ```python
  while_stmt: 'while' test ':' suite;
  ```

- [ ] ```python
  suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT;
  ```
  
- [x] ```python
  test: or_test ;
  ```
  
- [x] ```python
  or_test: and_test ('or' and_test)*;
  ```
  
- [x] ```python
  and_test: not_test ('and' not_test)*;
  ```
  
- [x] ```python
  not_test: 'not' not_test | comparison;
  ```
  
- [x] ```python
  comparison: arith_expr (comp_op arith_expr)*;
  ```
  
- [x] ```python
  comp_op: '<'|'>'|'=='|'>='|'<=' | '!=';
  ```
  
- [x] ```python
  arith_expr: term (addorsub_op term)*;
  ```
  
- [x] ```python
  addorsub_op: '+'|'-';
  ```
  
- [x] ```python
  term: factor (muldivmod_op factor)*;
  ```
  
- [x] ```python
  muldivmod_op: '*'|'/'|'//'|'%';
  ```
  
- [x] ```python
  factor: ('+'|'-') factor | atom_expr;
  ```
  
- [x] ```python
  atom_expr: atom trailer?;
  ```

- [x] ```python
  trailer: '(' (arglist)? ')' ;
  ```
  
- [x] ```python
  atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')'));
  ```
  
- [x] ```python
  testlist: test (',' test)* (',')?;  # 算式  eg： a,b  a  a+b
  ```
  
- [x] ```python
  arglist: argument (',' argument)*  (',')?;
  ```
  
- [ ] ```python
  argument: ( test | test '=' test );  # 缺 Keyword argument
  ```

### 报错部分

- [ ] 未定义的变量
- [ ] 运算时的非法类型