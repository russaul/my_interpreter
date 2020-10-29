#include "syntanalyze.h"
#include "lexanalyze.h"
#include "rpn.h"

int main(int argc, char **argv)
{
	int c = 0, er = 0, num = 0;
	FILE *f = fopen(argv[1],"r");
	Machine GreatAuto;
	if (!f){
		perror ("open: ");
		exit(1);
	}
	while (c != EOF){
		c = getc(f);
		er = GreatAuto.TakeChar(c);
		if (er > 0){
			num = GreatAuto.GetLine();
			AnalyzeError(er,num);
			return 1;
		}
	}
	SyntAnalyze Inter;
	try{
		Inter.GetList(GreatAuto.GiveList());
	}catch(Exception *exc){
		printf("Error: %s in line %d but met %s\n",
			exc->message, exc->lex->num_str, exc->lex->lexeme);
		return 1;
	}
//	printf ("Compilation success\n");
	Context *cont = Inter.GetCont();
	RpnItem *com = Inter.GetRpn(),*stack = NULL;
	try{
		while (com != NULL)
			com->elem->Evaluate(&stack,&com,cont);
	}
	catch(RpnExec *ex){
		printf("%s\n",ex->GetMsg());
	}
	return 0;
}
