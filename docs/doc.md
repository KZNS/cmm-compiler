# Cmm 编译器文档

本文档包含本项目编译器从词法分析到中间代码生成与执行，和最后ui的全部技术内容。

## 目录 <!-- omit in toc -->

- [1. 项目概述](#1-项目概述)
  - [1.1. 功能概述](#11-功能概述)
  - [1.2. 代码结构](#12-代码结构)
- [2. 词法分析](#2-词法分析)
- [3. 语法分析](#3-语法分析)
- [4. 语义分析](#4-语义分析)
- [5. 中间代码生成](#5-中间代码生成)
- [6. 解释执行](#6-解释执行)
- [7. UI前端](#7-ui前端)

## 1. 项目概述

### 1.1. 功能概述

见 [README.md](../README.md)

### 1.2. 代码结构

项目主体分为两个部分，编译器/解释器部分和前端部分。  

```text
.
├── run.sh       启动脚本
├── compiler     编译器/解释器部分
└── webservice   前端UI部分
```

编译器部分：

```text
.
├── compiler.exe      编译器可执行文件
├── language.txt      词法规则
├── compiler.cpp      编译器入口
├── automaton.h       词法分析部分
├── automaton.cpp
├── translator.h      语法、语义部分
├── translator.cpp
├── symbol_table.h    符号表部分
├── symbol_table.cpp
└── cLogger           log模块
    ├── clogger.cpp
    └── clogger.h

```

前端UI部分：

```text
.
├── 1.cpp
├── app.py
├── notes.md
├── placeholder.png
├── requirements.txt
├── static
│   └── comp.js
└── templates
    └── index.html
```

## 2. 词法分析

## 3. 语法分析

## 4. 语义分析

## 5. 中间代码生成

## 6. 解释执行

## 7. UI前端
