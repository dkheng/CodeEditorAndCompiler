# CodeEditorAndCompiler
基于Qt实现的代码编辑器与编译器，编译原理课程设计。可进行代码编辑，具有关键字高亮，需给定文法进行编译，输出四元式中间代码、目标代码等。
Code editor and compiler based on Qt implementation, and compilation principle course design.  Code editing can be performed, with keyword highlighting, a given grammar is required to be compiled, and quaternary intermediate code, object code, etc. are output.

使用提示：
1. 项目最初使用VS C++开发，后移植到Qt开发可视化界面。多数代码未使用Qt类，可自行修改。
2. 进行编译之前需自行给定文法，命名为grammar.txt放于TextFile文件夹下，编译各类输出文件也在TextFile文件夹下。
3. 编译器包括分析文法自动生成LL(1)分析表，词法分析单词扫描器，LL(1)语法分析，符号表，四元式生成与优化，目标代码生成等部分。
4. 文件读取使用fstream流，第一次运行前需将所有文件路径改为所用电脑路径。
   如
   fstream File("D:\\ProgramData\\QtFiles\\CodeEditor\\CodeEditor\\TextFile\\grammar.txt", fstream::in);
   需改为
   fstream File("...\\TextFile\\grammar.txt", fstream::in);
