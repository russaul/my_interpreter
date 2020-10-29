#ifndef RPN_H
#define RPN_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lexanalyze.h"
//#include "syntanalyze.h"

struct RpnItem{
	class RpnElem *elem;
	RpnItem *next;
};

struct Variable{
	char *name;
	double val;
	Variable *next;
};

struct Label{
	char *name;
	struct RpnItem *adr;
	Label *next;
};

class RpnExec{
protected:
	char *message;
	class RpnElem *ex;
public:
	char *GetMsg(){return message;}
};

class RpnExNotDouble:public RpnExec{
public:
	RpnExNotDouble (RpnElem *op);
};

class RpnExNotVar:public RpnExec{
public:
	RpnExNotVar (RpnElem *op);
};

class RpnExNoVar:public RpnExec{
public:
	RpnExNoVar ();
};

class RpnExNotLab:public RpnExec{
public:
	RpnExNotLab (RpnElem *op);
};

class RpnElem{
public:
	virtual ~RpnElem(){}
	virtual void Print() const = 0;
	virtual void Evaluate (RpnItem **stack, RpnItem **cur_cmd, 
				struct Context *con) const = 0;
protected:
	static void Push (RpnItem **stack, RpnElem *elem);
	static RpnElem *Pop (RpnItem **stack);
	static double ListGet(const char *n, Context *con);
	static void ListSet(const char *n, double v, Context *con);
};

struct Context{
	Variable *table_v;
	Label *table_l;
//	Market *market;
//	Players *players;
	Context(){table_v = new Variable; table_l = new Label;}
};

class RpnConst: public RpnElem {
public:
	virtual RpnElem* Clone() const = 0;
	virtual void Print() const = 0;
	void Evaluate (RpnItem **stack, RpnItem **cur_cmd, Context *con) const
	{
		Push (stack, Clone());
		*cur_cmd = (*cur_cmd)->next;
	}	
};

class RpnDouble:public RpnConst{
	double val;
public:
	RpnDouble (double i);
	virtual ~RpnDouble(){};
	RpnElem* Clone() const;
	double Get() const;
	void Print() const;
};

class RpnVar:public RpnConst{
	char *name;
public:
	RpnVar(const char *str);
	virtual ~RpnVar(){delete[] name;}
	RpnElem* Clone() const;
	char* Get() const;
	void Print() const;
};

class RpnLab:public RpnConst{
	RpnItem *adr;
public:
	RpnLab (RpnItem *item);
	virtual ~RpnLab();
	RpnElem* Clone() const;
	RpnItem* Get() const;
	void Set(RpnItem *tmp);
	void Print() const;
};

class RpnLabel:public RpnConst{
	char *name;
public:
	RpnLabel (char *n);
	virtual ~RpnLabel();
	RpnElem* Clone() const;
	char* Get() const;
	void Set(char *tmp);
	void Print() const;
};

class RpnStr:public RpnConst{
	char *str;
public:
	RpnStr(char *t);
	~RpnStr(){delete[] str;}
	RpnElem* Clone() const;
	char* Get() const;
	void Print() const;
};
 
/* Function's block */

class RpnFun:public RpnElem{
public:
	virtual RpnElem *EvaluateFun (RpnItem **stack, Context *con) const = 0;
	void Evaluate (RpnItem **stack, RpnItem **cur_cmd, Context *con) const;
	virtual void Print() const = 0;
};

class RpnFunPlus:public RpnFun{
public:
	RpnFunPlus(){}
	~RpnFunPlus(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunMinus:public RpnFun{
public:
	RpnFunMinus(){}
	~RpnFunMinus(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunMinusOne:public RpnFun{
public:
	RpnFunMinusOne(){}
	~RpnFunMinusOne(){}
	RpnElem *EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunMul:public RpnFun{
public:
	RpnFunMul(){}
	~RpnFunMul(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};
	
class RpnFunDiv:public RpnFun{
public:
	RpnFunDiv(){}
	~RpnFunDiv(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunMod:public RpnFun{
public:
	RpnFunMod(){}
	~RpnFunMod(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunEq:public RpnFun{
public:
	RpnFunEq(){}
	~RpnFunEq(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunMor:public RpnFun{
public:
	RpnFunMor(){}
	~RpnFunMor(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunLow:public RpnFun{
public:
	RpnFunLow(){}
	~RpnFunLow(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunAnd:public RpnFun{
public:
	RpnFunAnd(){}
	~RpnFunAnd(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunOr:public RpnFun{
public:
	RpnFunOr(){}
	~RpnFunOr(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunAss:public RpnFun{
public:
	RpnFunAss(){}
	~RpnFunAss(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunPrint:public RpnElem{
public:
	RpnFunPrint(){}
	~RpnFunPrint(){}
	void Evaluate(RpnItem **stack, RpnItem **cur_cmd, Context *con) const;
	void Print() const;
};

class RpnFunNo:public RpnFun{
public:
	RpnFunNo(){};
	~RpnFunNo(){};
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunInd:public RpnFun{
public:
	RpnFunInd(){};
	~RpnFunInd(){};
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunVar:public RpnFun{
public:
	RpnFunVar(){};
	~RpnFunVar(){};
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const;
	void Print() const;
};

class RpnFunTurn:public RpnFun{
public:
	RpnFunTurn(){}
	~RpnFunTurn(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunInfo:public RpnFun{
public:
	RpnFunInfo(){}
	~RpnFunInfo(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunMark:public RpnFun{
public:
	RpnFunMark(){}
	~RpnFunMark(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunBuild:public RpnFun{
public:
	RpnFunBuild(){}
	~RpnFunBuild(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunBuy:public RpnFun{
public:
	RpnFunBuy(){}
	~RpnFunBuy(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunSell:public RpnFun{
public:
	RpnFunSell(){}
	~RpnFunSell(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnFunProd:public RpnFun{
public:
	RpnFunProd(){}
	~RpnFunProd(){}
	RpnElem* EvaluateFun (RpnItem **stack, Context *con) const {};
	void Print() const;
};

class RpnOpNo:public RpnElem{
public:
	RpnOpNo(){};
	~RpnOpNo(){};
	void Evaluate (RpnItem **stack, RpnItem **cur_cmd, Context *con) const;
	void Print() const;
};

class RpnOpGo:public RpnElem{
public:
	RpnOpGo(){};
	~RpnOpGo(){};
	void Evaluate (RpnItem **stack, RpnItem **cur_cmd, Context *con) const;
	void Print() const;
};

class RpnOpGoFalse:public RpnElem{
public:
	RpnOpGoFalse(){};
	~RpnOpGoFalse(){};
	void Evaluate (RpnItem **stack, RpnItem **cur_cmd, Context *con) const;
	void Print() const;
};

#endif
