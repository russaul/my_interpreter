//#include "lexanalyze.h"
//#include "syntanalyze.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "lexanalyze.h"
#include "rpn.h"

void RpnElem::Push (RpnItem **stack, RpnElem *elem)
{
	RpnItem *tmp = new RpnItem;
	tmp->elem = elem;
	tmp->next = NULL;
	if ((*stack) == NULL)
		(*stack) = tmp;
	else{
		tmp->next = (*stack);
		(*stack) = tmp;
	}
}

RpnElem *RpnElem::Pop (RpnItem **stack)
{
	RpnElem *tmp;
	if ((*stack)->next == NULL){
		tmp = (*stack)->elem;
		(*stack) = NULL;
	}else{
		tmp = (*stack)->elem;
		(*stack) = (*stack)->next;
	}
	return tmp;
}

RpnExNotDouble::RpnExNotDouble(RpnElem *op)
{
	ex = op;
	const char mes[] = "This is not double, this:";
	op->Print();
	message = StrCopy(mes);
}

RpnExNotVar::RpnExNotVar(RpnElem *op)
{
	ex = op;
	const char mes[] = "This is not var, this:";
	op->Print();
	message = StrCopy(mes);
}

RpnExNoVar::RpnExNoVar()
{
	ex = NULL;
	const char mes[] = "I haven't found variable in the table\n";
	message = StrCopy(mes);
}

RpnExNotLab::RpnExNotLab(RpnElem *op)
{
	ex = op;
	const char mes[] = "Label don't found\n";
	message = StrCopy(mes);
}

/*Type's block */

RpnDouble::RpnDouble(double i)
{
	val = i;
}

RpnElem* RpnDouble::Clone() const
{
	return new RpnDouble(val);
}

double RpnDouble::Get() const
{
	return val;
}

RpnVar::RpnVar(const char *str)
{
	name = StrCopy(str);
}

RpnElem* RpnVar::Clone() const
{
	return new RpnVar(name);
}

char* RpnVar::Get() const
{
	return name;
}

RpnLab::RpnLab(RpnItem *item)
{
	adr = item;
}

RpnElem* RpnLab::Clone() const
{
	return new RpnLab(adr);
}

RpnItem* RpnLab::Get() const
{
	return adr;
}

void RpnLab::Set(RpnItem *tmp)
{
	adr = tmp;
}

RpnLabel::RpnLabel(char *n)
{
	name = StrCopy(n);
}

RpnElem* RpnLabel::Clone() const
{
	return new RpnLabel(name);
}

char* RpnLabel::Get() const
{
	return name;
}

void RpnLabel::Set(char *tmp)
{
	name = tmp;
}

RpnStr::RpnStr(char *t)
{
	str = StrCopy(t);
}

RpnElem* RpnStr::Clone() const
{
	return new RpnStr(str);
}

char* RpnStr::Get() const
{
	return str;
}

/*Function's block */

RpnElem* RpnFunPlus::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() + i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunMinus::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() - i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunMinusOne::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	double res = i1->Get()-1;
	delete op1;
	return new RpnDouble(res);
}

RpnElem* RpnFunMul::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() * i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunDiv::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() / i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunMod::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	int p1 = i1->Get(), p2 = i2->Get();
	int res = p1 % p2;
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunAnd::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() && i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunOr::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() || i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunMor::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() > i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunLow::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() < i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunEq::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *i2 = dynamic_cast<RpnDouble*>(op2);
	if (!i2) throw RpnExNotDouble(op2);
	double res = i1->Get() == i2->Get();
	delete op1;
	delete op2;
	return new RpnDouble(res);
}

RpnElem* RpnFunVar::EvaluateFun(RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnVar *i1 = dynamic_cast<RpnVar*>(op1);
	if (!i1) throw RpnExNotVar(op1);
	double res = ListGet(i1->Get(), con);
	delete[] op1;
	return new RpnDouble(res);
}

RpnElem* RpnFunNo::EvaluateFun (RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	double res = !(i1->Get());
	delete op1;
	return new RpnDouble(res);
}
		
void RpnFunPrint::Evaluate(RpnItem **stack, RpnItem **cur_cmd, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnStr *i1 = dynamic_cast<RpnStr*>(op1);
	if (!i1){
		RpnDouble *i2 = dynamic_cast<RpnDouble*>(op1);
		double res = i2->Get();
		printf ("%f\n",res);
	}else{
		printf ("%s\n",i1->Get());
	}
	delete op1;
	*cur_cmd = (*cur_cmd)->next;
}

RpnElem* RpnFunInd::EvaluateFun(RpnItem **stack, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnVar *i2 = dynamic_cast<RpnVar*> (op2);
	if (!i2) throw RpnExNotVar(op2);
	double ind = i1->Get();
	char *str = NumToStr(ind);
	char *ok = new char[StrLen(i2->Get())+StrLen(str)-1];
	ok = StrCopy(i2->Get());
	ok[StrLen(i2->Get())-1] = '.';
	int i=0;
	while(str[i] != 0){
		ok[StrLen(i2->Get())+i] = str[i];
		i++;
	}
	delete[] str;
	delete op1;
	delete op2;
	return new RpnVar(ok);
} 

RpnElem* RpnFunAss::EvaluateFun(RpnItem **stack, Context *con) const
{
//	Variable *list = con->table_v;
	RpnElem *op1 = Pop(stack);
	RpnDouble *i1 = dynamic_cast<RpnDouble*>(op1);
	if (!i1) throw RpnExNotDouble(op1);
	RpnElem *op2 = Pop(stack);
	RpnVar *i2 = dynamic_cast<RpnVar*>(op2);
	if (!i2) throw RpnExNotVar(op1);
	ListSet(i2->Get(),i1->Get(),con);
	delete op1;
	delete op2;
	return NULL;
}

double RpnElem::ListGet(const char *n, Context *con)
{
	Variable *list = con->table_v;
	while ((list != NULL) && (strcmp(n,list->name) != 0))
		list = list->next;
	if (list == NULL) throw RpnExNoVar();
	return list->val;
}

void RpnElem::ListSet(const char *n, double v, Context *con)
{
	Variable *list = con->table_v;
	while ((list != NULL) && (strcmp(n,list->name) != 0))
		list = list->next;
	if (list == NULL){
		list = new Variable;
		list->next = NULL;
		list->name = StrCopy(n);
		list->val = v;
	}else{
		list->val = v;
	}
}

void RpnFun::Evaluate(RpnItem **stack, RpnItem **cur_cmd, Context *con) const
{
	RpnElem *res = EvaluateFun(stack,con);
	if (res) Push(stack,res);
	*cur_cmd = (*cur_cmd)->next;
}

void RpnOpNo::Evaluate(RpnItem **stack, RpnItem **cur_cmd, Context *con) const
{
	*cur_cmd = (*cur_cmd)->next;
}

void RpnOpGo::Evaluate(RpnItem **stack, RpnItem **cur_cmd, Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnLab *l1 = dynamic_cast<RpnLab*>(op1);
	if (!l1) throw RpnExNotLab(op1);
	RpnItem *addr = l1->Get();
	*cur_cmd = addr;
	delete op1;
}

void RpnOpGoFalse::Evaluate (RpnItem **stack, RpnItem **cur_cmd,
				 Context *con) const
{
	RpnElem *op1 = Pop(stack);
	RpnLab *l1 = dynamic_cast<RpnLab*>(op1);
	if (!l1) throw RpnExNotLab(op1);
	RpnElem *op2 = Pop(stack);
	RpnDouble *l2 = dynamic_cast<RpnDouble*>(op2);
	if (!l2) throw RpnExNotDouble(op1);
	double fin = l2->Get();
	if (fin == 1){
		RpnItem *addr = l1->Get();
		*cur_cmd = addr;
		delete op1;
		delete op2;
	}else{
		*cur_cmd = (*cur_cmd)->next;
		delete op1;
		delete op2;
	}
}

void RpnDouble::Print() const
{
	printf("Double\n");
}

void RpnVar::Print() const
{
	printf("Variable\n");
}

void RpnLab::Print() const
{
	printf("Lable\n");
}

void RpnStr::Print() const
{
	printf("String\n");
}

void RpnFunPlus::Print() const
{
	printf("Plus\n");
}

void RpnFunMinus::Print() const
{
	printf("Minus\n");
}

void RpnFunMinusOne::Print() const
{
	printf("MinusOne\n");
}

void RpnFunMul::Print() const
{
	printf("Multipicate\n");
}

void RpnFunDiv::Print() const
{
	printf("Div\n");
}

void RpnFunMod::Print() const
{
	printf("Mod\n");
}

void RpnFunEq::Print() const
{
	printf("Equel\n");
}

void RpnFunMor::Print() const
{
	printf("More\n");
}

void RpnFunLow::Print() const
{
	printf("Low\n");
}

void RpnFunAnd::Print() const
{
	printf("And\n");
}

void RpnFunOr::Print() const
{
	printf("or\n");
}

void RpnFunAss::Print() const
{
	printf("Assignmet\n");
}

void RpnFunPrint::Print() const
{
	printf("Print\n");
}

void RpnFunTurn::Print() const
{
	printf("Turn\n");
}

void RpnFunInfo::Print() const
{
	printf("Info\n");
}

void RpnFunMark::Print() const
{
	printf("Market\n");
}

void RpnFunBuild::Print() const
{
	printf("Build\n");
}

void RpnFunProd::Print() const
{
	printf("Prod\n");
}

void RpnFunBuy::Print() const
{
	printf("Buy\n");
}

void RpnFunSell::Print() const
{
	printf("Sell\n");
}

void RpnOpNo::Print() const
{
	printf("NoOperation\n");
}

void RpnOpGo::Print() const
{
	printf("Go\n");
}

void RpnOpGoFalse::Print() const
{
	printf("GoFalse\n");
}

void RpnFunNo::Print() const
{
	printf("No\n");
}

void RpnFunInd::Print() const
{
	printf("Index\n");
}

void RpnLabel::Print() const
{
	printf("Labeled\n");
}

