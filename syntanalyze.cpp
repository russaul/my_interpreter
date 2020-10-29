#include "lexanalyze.h"
#include "syntanalyze.h"
#include "rpn.h"

SyntAnalyze::SyntAnalyze()
{
	context = new Context;
	context->table_v = NULL;
	context->table_l = NULL;
	rpn = NULL;
	cur_elem = NULL;
}

SyntAnalyze::~SyntAnalyze()
{
	delete list;
}

void SyntAnalyze::Next()
{
	list = list->next;
	if (list == NULL)
		ex.ThrowEx(list,"Unexpected end of file");

}
void Exception::ThrowEx(buffer *l, const char *str)
{
	lex = l;
	message = StrCopy(str);
	throw(this);
}
	
void SyntAnalyze::Start()
{
	if (strcmp(list->lexeme, "begin") != 0)
		ex.ThrowEx(list, "expected 'begin'");
	Next();
	Into();
	if (strcmp(list->lexeme,"end") != 0)
		ex.ThrowEx(list, "expected 'end'");
}

void SyntAnalyze::Into()
{
	while (strcmp(list->lexeme, "end") != 0){
		Comand();
		Next();
	}
}		

void SyntAnalyze::Comand()
{
	if (list->type == lx_label){
		AddRpn(new RpnOpNo());
		Label *l = context->table_l;
		if (l == NULL){
			l = new Label;
			l->next = NULL;
			l->name = StrCopy(list->lexeme);
			l->adr = GetCurElem();
			context->table_l = l;
		}else{
			while (l->next != NULL){
				if (strcmp(l->name,list->lexeme) == 0){
					l->adr = GetCurElem();
					break;
				}else{
					l = l->next;
					if (l->next == NULL){
						l->next = new Label;
						l->next->next = NULL;
						l->next->name = StrCopy(
								list->lexeme);
						l->next->adr = GetCurElem();
						break;
					}
				}
			}
		}
		Next();
	//	l = NULL;
	//	delete[] l;
	}	
	Com1();
	if (strcmp(list->lexeme,";") != 0)
		ex.ThrowEx(list,"expected ';'");
}

void SyntAnalyze::Com1()
{
	if (strcmp(list->lexeme,"if") == 0){
		Next();
		Arg();
		RpnLab *l1 = new RpnLab(NULL);
		AddRpn(l1);
		AddRpn(new RpnOpGoFalse());
		if (strcmp(list->lexeme,"then") != 0)
			ex.ThrowEx(list,"expected 'then'");
		Next();
		if (strcmp(list->lexeme,"begin") == 0){
			Next();
			Into();
			if (strcmp(list->lexeme,"end") != 0)
				ex.ThrowEx(list, "expected 'end'");
			Next();
			AddRpn(new RpnOpNo());
			l1->Set(cur_elem);
		}else{
			Com1();
			AddRpn(new RpnOpNo());
			l1->Set(cur_elem);
		}
	}else if (strcmp(list->lexeme,"go") == 0){
		Next();
		if (list->type != lx_label)
			ex.ThrowEx(list,"expected label");
		AddRpn(new RpnLabel(list->lexeme));
		Label *l = context->table_l;
		if (l == NULL){
			l = new Label;
			l->next = NULL;
			l->name = StrCopy(list->lexeme);
			context->table_l;
		}else{
			while (l->next != NULL){
				if (strcmp(l->name,list->lexeme) == 0){
					break;
				}else{
					l = l->next;
					if (l->next == NULL){
						l = l->next;
						l->next = NULL;
						l->name = StrCopy(list->lexeme);
						context->table_l = l;
						break;
					}
				}
			}
		}
		l = NULL;
		delete[] l;
		AddRpn(new RpnOpGo());
		Next();
	}else if (list->type == lx_function)
		Func();
	else if (list->type == lx_variable){
		Var();
		if (strcmp(list->lexeme,":=") != 0)
			ex.ThrowEx(list,"expected ':='");
		Next();
		Arg();
		AddRpn(new RpnFunAss());
	}
}

void SyntAnalyze::Func()
{
	if ((strcmp(list->lexeme,"?turn") == 0) ||
		(strcmp(list->lexeme,"?info") == 0) ||
		(strcmp(list->lexeme,"?market") == 0) ||
		(strcmp(list->lexeme,"?build") == 0)){
		Fun0();
		Next();
	}else if (strcmp(list->lexeme,"?prod") == 0){
		Next();
		Arg();
		AddRpn(new RpnFunProd());
	}else if ((strcmp(list->lexeme,"?buy") == 0) ||
		(strcmp(list->lexeme,"?sell") == 0)){
		Fun2();
	}else if (strcmp(list->lexeme,"?print") == 0){
		Next();
		Arg2();
		AddRpn(new RpnFunPrint());
	}else
		ex.ThrowEx(list,"Bad function");
}

void SyntAnalyze::Fun0()
{
	if (strcmp(list->lexeme,"?turn") == 0)
		AddRpn(new RpnFunTurn());
	else if (strcmp(list->lexeme,"?info") == 0)
		AddRpn(new RpnFunInfo());
	else if (strcmp(list->lexeme,"?market") == 0)
		AddRpn(new RpnFunMark());
	else if (strcmp(list->lexeme,"?build") == 0)
		AddRpn(new RpnFunBuild());
}

void SyntAnalyze::Fun2()
{
	if (strcmp(list->lexeme,"?buy") == 0){
		Next();
		Arg();
		Arg();
		AddRpn(new RpnFunBuy());
	}else if (strcmp(list->lexeme,"?sell") == 0){
		Next();
		Arg();
		Arg();
		AddRpn(new RpnFunSell());
	}
}

void SyntAnalyze::Arg2()
{
	if (list->type == lx_string){
		AddRpn(new RpnStr(list->lexeme));
		Next();
		return;
	}
	if (list->type == lx_variable){
		Var();
		AddRpn(new RpnFunVar());
	}else
		ex.ThrowEx(list,"Wrong parametr for ?print");
}

void SyntAnalyze::Var()
{
	if (list->type != lx_variable)
		ex.ThrowEx(list,"expected variable");
	AddRpn(new RpnVar(list->lexeme));
	Next();
	Mas();
}

void SyntAnalyze::Mas()
{
	if (strcmp(list->lexeme,"[") == 0){
		Next();
		Arg();
		if (strcmp(list->lexeme,"]") != 0)
			ex.ThrowEx(list,"Expected ']'");
		AddRpn(new RpnFunInd());
		Next();
	}
}

void SyntAnalyze::Arg()
{
	Part1();
	Part2();
}

void SyntAnalyze::Part1()
{
	Midle1();
	Midle2();
}

void SyntAnalyze::Part2()
{
	if ((strcmp(list->lexeme,"<") == 0) ||
		(strcmp(list->lexeme,">") == 0) ||
		(strcmp(list->lexeme,"=") == 0))
	{
		if (strcmp(list->lexeme,">") == 0){
			Next();
			Arg();
			AddRpn(new RpnFunMor());
		}else if (strcmp(list->lexeme,"<") == 0){
			Next();
			Arg();
			AddRpn(new RpnFunLow());
		}else if (strcmp(list->lexeme,"=") == 0){
			Next();
			Arg();
			AddRpn(new RpnFunEq());
		}
	}
}

void SyntAnalyze::Midle1()
{
	Base1();
	Base2();
}

void SyntAnalyze::Midle2()
{
	if ((strcmp(list->lexeme,"+") == 0) ||
		(strcmp(list->lexeme,"-") == 0) ||
		(strcmp(list->lexeme,"|") == 0))
	{
		if (strcmp(list->lexeme,"+") == 0){
			Next();
			Part1();
			AddRpn(new RpnFunPlus);
		}else if (strcmp(list->lexeme,"-") == 0){
			Next();
			Part1();
			AddRpn(new RpnFunMinus);
		}else if (strcmp(list->lexeme,"|") == 0){
			Next();
			Part1();
			AddRpn(new RpnFunOr);
		}
	}
}

void SyntAnalyze::Base1()
{
	if (list->type == lx_variable){
		Var();
		AddRpn(new RpnFunVar());
	}else if (list->type == lx_number){
		AddRpn(new RpnDouble(StrToNum(list->lexeme)));
		Next();
	}else if (strcmp(list->lexeme,"!") == 0){
		Next();
		Arg();
		AddRpn(new RpnFunNo());
	}else if (strcmp(list->lexeme,"-") == 0){
		Next();
		Arg();
		AddRpn(new RpnFunMinusOne());
	}else if (strcmp(list->lexeme,"(") == 0){
		Next();
		Arg();
		if (strcmp(list->lexeme,")") != 0)
			ex.ThrowEx(list,"expected ')'");
		Next();
	}else
		ex.ThrowEx(list,"bad expression");
}
		
void SyntAnalyze::Base2()
{
	if ((strcmp(list->lexeme,"*") == 0) ||
		(strcmp(list->lexeme,"/") == 0) ||
		(strcmp(list->lexeme,"%") == 0) ||
		(strcmp(list->lexeme,"&") == 0))
	{
		if (strcmp(list->lexeme,"*") == 0){
			Next();
			Midle1();
			AddRpn(new RpnFunMul);
		}else if (strcmp(list->lexeme,"/") == 0){
			Next();
			Midle1();
			AddRpn(new RpnFunDiv);
		}else if (strcmp(list->lexeme,"&") == 0){
			Next();
			Midle1();
			AddRpn(new RpnFunAnd);
		}else if (strcmp(list->lexeme,"%") == 0){
			Next();
			Midle1();
			AddRpn(new RpnFunMod);
		}
	}
}

void SyntAnalyze::GetList(buffer *star)
{
	list = star;
	Start();
}

RpnItem* SyntAnalyze::GetRpn()
{
	return rpn;
}

RpnItem* SyntAnalyze::GetCurElem()
{
	return cur_elem;
}

void SyntAnalyze::AddRpn (RpnElem *elem)
{
	RpnItem *tmp = rpn;
	if (rpn == NULL){
		rpn = new RpnItem;
		tmp = rpn;
	}else{
		while (rpn->next != NULL)
			rpn = rpn->next;
		rpn->next = new RpnItem;
		rpn = rpn->next;
	}
	rpn->elem = elem;
	rpn->next = NULL;
	cur_elem = rpn;
	rpn = tmp;
}

void SyntAnalyze::DelRpn()
{
	RpnItem *tmp = rpn;
	while(rpn){
		delete rpn->elem;
		rpn = rpn->next;
		delete tmp;
		tmp = rpn;
	}
}	

void SyntAnalyze::PrintRpn()
{
	RpnItem *tmp = rpn;
	while(tmp != NULL){
		tmp->elem->Print();
		tmp = tmp->next;
	}
}

Context* SyntAnalyze::GetCont()
{
	return context;
}
