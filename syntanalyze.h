#ifndef SYNT_H
#define SYNT_H
/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>*/
#include "lexanalyze.h"
#include "rpn.h"

struct Exception{
	buffer *lex;
	char *message;
	void ThrowEx(buffer *l, const char *str);
};

class SyntAnalyze{
	buffer *list;
	int ident;
	Exception ex;
	RpnItem *rpn;
	RpnItem *cur_elem;
	Context *context;
	void Next();
	void Start();
	void Into();
	void Comand();
	void Com1();
	void Func();
	void Arg2();
	void Var();
	void Mas();
	void Arg();
	void Part1();
	void Part2();
	void Midle1();
	void Midle2();
	void Base1();
	void Base2();
	void Fun0();
	void Fun2();
	void AddRpn (RpnElem *elem);
public:
	SyntAnalyze();
	~SyntAnalyze();
	void GetList(buffer *star);
	RpnItem *GetRpn();
	RpnItem *GetCurElem();
	void PrintRpn();
	void DelRpn();
	Context *GetCont();
};

#endif
