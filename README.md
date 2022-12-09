## 现代 C++ 实现简单 SQL 数据库

> iSTLsql（ i 谐音“爱”，STL 即 C++ 标准模板库）是一个简单的 SQL 数据库，基于 `std::multiset` 实现索引结构。目前支持 bzj 老师大作业要求的所有功能

### Introduction

环境：Ubuntu-22.04，g++ 11.2.0

数据暂时存储在 `./testdata.in` 中，以 SQL 语句存储，可以作为示例参考

### Used C++ Features环境

✅ auto 类型推导与函数返回值推导，基于区间的 for，结构化绑定，函数默认参数值

✅ C++ 强类型枚举类 enum class,，左值引用，using 定义新类型

✅ `std::string`, `std::unordered_map`, `std::multiset`

✅ `std::stringstream`, `std::vector`, `std::tuple`

✅ lambda 表达式（匿名函数）, `std::functional`

✅ 类模板 template，回调函数 callback，重载二元运算符

✅ 类 class（构造函数，析构函数，虚函数 virtual）

✅ 自定义异常与异常处理

✅ `noexcept`, `nullptr`

### Other Featrures

✅ 白老师的双向链表模板 `dlist.h`

✅ Linux 下终端颜色渲染（借用白老师的代码）

### SQL Grammar

1. create database 

   功能：创建数据库 

   语法： 

   ```sql
   create database  <dbname>
   ```

   注：尖括号<>标记括起了参数。在使用时，须给出具体的名字。例如： create database person 以下同此。

2. drop database

   功能：删除数据库 

   语法： 

   ```sql
   drop database
   ```

3. use

   功能：切换数据库 

   语法：

   ```sql
   use <dbname>
   ```

4. create table

   功能：创建表

   语法：

   ```sql
   create table <table-name> (<column> <type> [primary],…)
   ```

   数据类型 `<type>`仅支持 `int` 和 `string`

   如果指定列是 primary（主键），则为表建立索引

   **注：`<table-name>`后必须有空格，`(` 与 `<column>` 之间不能有空格，每个 `,` 后也需要有空格，`)` 前不能有空格**

待更新。。。