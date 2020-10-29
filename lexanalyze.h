#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

enum types{
	lx_number, lx_string, lx_variable, lx_function, lx_label, lx_keyword
};

enum error_types{
	er_ok, er_number, er_string, er_key, er_end, er_variable
};

struct buffer{
	char *lexeme;
	enum types type;
	int num_str;
	struct buffer *next;
};

class Machine{
	int status, num, bignum, first_er,point;
	char symb, def_symb, *buf;
	buffer *ListLex, *ListLast;
	enum types type;
	enum states{
		start, number, string, variable, equel, key, separator
	};
	enum error_types error;
	void AddList();
	void AnalyzeSymb();
	void AnalyzeVariable();
	void AnalyzeNumber();
	void AnalyzeString();
	void AnalyzeEquel();
	void AnalyzeSeparator();
	void AnalyzeStates();
	void AnalyzeKey();
	void AnalyzeLexKey();
	void AddBuf();
public:
	Machine();
	~Machine();
	int TakeChar(char s);
	buffer *GiveList();
	void GiveList1();
	int GetLine(){return num;}
};

void AnalyzeError(int er, int num);
char *NewBuf(char *s);
char *NumToStr(int num);
int StrLen(const char *str);
char *StrCopy(const char *str_start);
double StrToNum(const char *str);

#endif
