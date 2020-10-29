#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "lexanalyze.h"

Machine::Machine()
{
	status = start;
	def_symb = 0;
	point = 0;
	num = 1;
	bignum = 0;
	buf = new char[2];
	buf[0] = 0;
	buf[1] = 0;
	ListLex = NULL;
	ListLast = NULL;
	error = er_ok;
}

Machine::~Machine(){
	status = start;
	while (ListLex){
		delete[] ListLex->lexeme;
		delete ListLex;
		ListLex = ListLex->next;
	}
	delete[] buf;
}	

void AnalyzeError(int er, int num)
{
	if (er == er_end){
		printf("Error input in %d\n",num);
		return;
	}
	if (er == er_string){
		printf("Error input string in %d\n",num);
		return;
	}
	if (er == er_number){
		printf("Error input number in %d\n",num);
		return;
	}
	if (er == er_key){
		printf("Error input keyword in %d\n",num);
		return;
	}
	if (er == er_variable){
		printf("Error input variable/function/label in %d\n",num);
		return;
	}
}

char *NewBuf(char *s)
{
	delete[] s;
	s = new char[2];
	s[0] = 0;
	s[1] = 0;
	return s;
}

char *NumToStr(int num)
{
	int i = 0, work = num;
	char *good;
	if (num == 0){
		good = new char[2];
		good[0] = '0';
		good[1] = 0;
		return good;
	}
	while (work != 0){
		work = work/10;
		i++;
	}
	i++;
	good = new char[i];
	good[i-1] = 0;
	i = i - 2;
	if (num < 0)
		good[0] = '-';
	num = abs(num);
	while (num != 0){
		good[i] = num%10 + 48;
		i--;
		num = num/10;
	}
	return good;
}

double StrToNum(const char *str)
{
	double num = 0;
	int i = StrLen(str), p = 0, tmp = 0, k = 0;
	while ((str[tmp] != '.') && (str[tmp] != 0))
		tmp++;
	k = tmp;
	tmp--;
	while (p != i){
		if (p == k){
			p++;
			continue;
		}
		num = (str[p] - 48)*(pow(10,tmp)) + num;
		p++;
		tmp--;
	}
	return num;
}

int StrLen(const char *str)
{
	int i = 0;
	while (str[i] != 0){
		i++;
	}
	return i;
}

char *StrCopy(const char *str_start)
{	
	char *str_end;
	int i = StrLen(str_start);
	str_end = new char[i];
	while (i>=0){
		str_end[i] = str_start[i];
		i--;
	}
	return str_end;
}

void Machine::AddBuf()
{
	int i = StrLen(buf);
	char *temp = new char[i + 2];
	temp[i] = symb;
	temp[i+1] = 0;
	i--;
	while (i>=0){
		temp[i] = buf[i];
		i--;
	}
	delete[] buf;
	buf = temp;
}

void Machine::AddList()
{
	buffer *tmp;
	tmp = new buffer;
	if (status == number){
		tmp->lexeme = StrCopy(buf);
		tmp->type = lx_number;
	}
	if (status == string){
		tmp->lexeme = StrCopy(buf);
		tmp->type = lx_string;
	}
	if (status == variable){
		tmp->lexeme = StrCopy(buf);
		tmp->type = type;
	}
	if (status == equel){
		tmp->lexeme = StrCopy(buf);
	}
	if (status == separator){
		tmp->lexeme = StrCopy(buf);
		tmp->type = type;
	}
	if (status == key){
		tmp->lexeme = StrCopy(buf);
		tmp->type = lx_keyword;
	}
	tmp->num_str = num;
	tmp->next = NULL;
	if (ListLast){
		ListLast->next = tmp;
		ListLast = ListLast->next;
	}else{
		ListLex = ListLast = tmp;
	}
	buf = NewBuf(buf);
	status = start;
}

buffer *Machine::GiveList()
{
	buffer *Temp = ListLex;
	return Temp;
}

void Machine::GiveList1()
{
	buffer *List = ListLex;
	while (List != NULL){
		printf("[%s]",List->lexeme);
		switch(List->type){
			case lx_variable:
			       printf("[variable]");
			       break;
			case lx_function:
			       printf("[function]");
			       break;
			case lx_label:
			       printf("[label]");
			       break;
			case lx_keyword:
			       printf("[keyword]");
			       break;
			case lx_string:
			       printf("[string]");
			       break;
			case lx_number:
			       printf("[number]");
			       break;
		}
		printf("[%d]\n",List->num_str);
		List = List->next;
	}
}

void Machine::AnalyzeStates()
{
	switch(status){
		case variable:
			AnalyzeVariable();
			break;
		case number:
			AnalyzeNumber();
			break;
		case string:
			AnalyzeString();
			break;
		case equel:
			AnalyzeEquel();
			break;
		case key:
			AnalyzeKey();
			break;
		default:
			AnalyzeSymb();
	}
}

void Machine::AnalyzeSymb ()
{
	if (symb == '$'){
		type = lx_variable;
		AnalyzeVariable();
		return;
	}
	if (symb == '?'){
		type = lx_function;
		AnalyzeVariable();
		return;
	}
	if (symb == '@'){
		type = lx_label;
		AnalyzeVariable();
		return;
	}
	if ((symb >= '0') && (symb <= '9')){
		status = number;
		AnalyzeNumber();
		return;
	}
	if (symb == '"'){
		AnalyzeString();
		return;
	}
	if (symb == ':'){
		AnalyzeEquel();
		return;
	}
	if (((symb >= 'a') && (symb <= 'z')) || 
		((symb >= 'A') && (symb <= 'Z'))){
		AnalyzeKey();
		return;
	}
	if ((symb != ' ') && (symb != '\t') && (symb != '\n')){
		status = separator;
		AnalyzeSeparator();
		return;
	}
	if (symb == '\n'){
		num++;
		return;
	}
	return;
}

void Machine::AnalyzeSeparator()
{
	type = lx_keyword;
	buf = new char[2];
	buf[0] = symb;
	buf[1] = 0;
	AddList();
	return;
}

void Machine::AnalyzeNumber()
{
	if ((symb >= '0') && (symb <= '9')){
		AddBuf();
		return;
	}
	if (symb == '.'){
		if (point == 0){
			AddBuf();
			point++;
			return;
		}else{
			error = er_number;
			return;
		}
	}
	if (((symb >= 'a') && (symb <= 'z')) || 
		((symb >= 'A') && (symb <= 'Z'))){
		error = er_number;
		return;
	}else{
		AddList();
		def_symb = symb;
	}
}

void Machine::AnalyzeVariable()
{
	if (status == start){
		buf[0] = symb;
		status = variable;
		return;
	}
	if (((symb >= 'a') && (symb <= 'z')) || 
		((symb >= 'A') && (symb <= 'Z')) ||
		((symb >= '0') && (symb <= '9'))){
		AddBuf();
		return;
	}else{
		if (buf[1] == 0){
			error = er_variable;
			return;
		}
		AddList();
		def_symb = symb;
	}
}

void Machine::AnalyzeString()
{
	if (status == start){
		status = string;
		return;
	}
	if (buf[0] == 0){
		buf[0] = symb;
		return;
	}
	if (symb == '\n'){
		error = er_string;
		return;
	}
	if (symb != '"'){
		AddBuf();
		return;
	}else{
		AddList();
		return;
	}
}

void Machine::AnalyzeEquel()
{
	if (status == start){
		buf[0] = symb;
		status = equel;
		return;
	}
	if (symb == '='){
		AddBuf();
		AddList();
		return;
	}else{
		AddList();
	}

}

void Machine::AnalyzeKey()
{
	if (status == start){
		buf[0] = symb;
		status = key;
		return;
	}
	if (((symb >= 'a') && (symb <= 'z')) || 
		((symb >= 'A') && (symb <= 'Z'))){
		AddBuf();
	}else{
		if ((symb >= '0') && (symb <= '9'))
			error = er_key;
		AnalyzeLexKey();
		def_symb = symb;
		return;
	}
}

void Machine::AnalyzeLexKey()
{
	if ((strcmp(buf, "if") == 0) || 
		(strcmp(buf,"then") == 0) ||
		(strcmp(buf,"go") == 0) ||
		(strcmp(buf,"begin") == 0) ||
		(strcmp(buf,"end") == 0)){
		AddList();
	}else{
		error = er_key;
		buf = NewBuf(buf);
	}
}	

int Machine::TakeChar(char s)
{
	symb = s;
	if (symb == EOF){
		if (status != start){
			error = er_end;
			return error;
		}
		return 0;
	}
	AnalyzeStates();
	if (def_symb != 0){
		symb = def_symb;
		def_symb = 0;
		AnalyzeStates();
	}
	if (error != er_ok){
		return error;
	}
	return 0;
}
