## 现代 C++ 实现简单 SQL 数据库

> iSTLsql（i谐音“爱”，STL即C++标准膜拜库）是一个简单的 SQL 数据库，基于 `std::multiset` 实现索引结构。目前支持 bzj 老师大作业要求的所有功能

1. create database 

   功能：创建数据库 

   语法： 

   ```sql
   create database  <dbname>
   ```

   注：尖括号<>标记括起了参数。在使用时，须给出具体的名字。例如： create database person 以下同此。

2.  drop database

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