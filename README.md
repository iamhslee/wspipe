# wspipe
2024-1 Operating Systems (ITP30002) - HW #2

A homework assignment to implement pipe-based word search program, just like the `grep` command in Linux, but simpler using C language.

### Author
Hyunseo Lee (22100600) <hslee@handong.ac.kr>

## 1. How to build
This program is written in C language on Ubuntu 22.04.1 LTS. To build this program, you need to use Ubuntu 22.04.1 LTS with GCC and Make installed.

After unarchiving the folder, you should see the following files:
```bash
$ ls
Makefile  README.md  wspipe.c
```

To build the program, run the following command in the terminal:
```bash
$ make
```

To build the program with debug mode, run the following command in the terminal:
```bash
$ make debug
```

To cleanup the build files, run the following command in the terminal:
```bash
$ make clean
```

## 2. Usage
To run the program, run the following command in the terminal:
```bash
$ /wspipe <Command> <Keyword>
```

`Command`: Command to execute with arguments (wrap with single (or double) quotes if contains whitespace or any escape character)

`Keyword`: Keyword to search (wrap with single (or double) quotes if contains whitespace or any escape character)

You can also see the usage explained above by running the following command in the terminal:
```bash
$ ./wspipe -h
```