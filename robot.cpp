#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include "lexanalyze.h"
#include "syntanalyze.h"
#include "rpn.h"

#define BUFSIZE 1024
#define MAXARGC 6
#define MAXARGJ 5
	
struct InfPlayer{
	char *name;
	int factory;
	int product;
	int material;
	int score;
	int status;
};

int StrToInt(char *str)
{
	int i = 0, p = StrLen(str) - 1;
	while (p >= 0){
		i = i*10 + (str[p] - 48);
		p--;
	}
	return i;
}

char* ReadBot(char *buf, const double fd)
{
	delete[] buf;
	buf = new char[BUFSIZE];
	read(fd, buf, BUFSIZE);
	printf("%s\n",buf);
	return buf;
}

void WaitOther(double fd, int numpl)
{	
	int n = 0;
	char* buf = new char[BUFSIZE]; 
	printf("%d\n",numpl);
	while (n < numpl){
		printf("ok\n");
		buf = ReadBot(buf,fd);
		n++;
	}
	char *st = StrCopy("start\n");
	write(fd, st, StrLen(st));
	buf = ReadBot(buf,fd);
}

int Check(char **argv, int argc)
{
	if ((strcmp(argv[4],".create") != 0) && (strcmp(argv[4],".join") != 0)){
		printf ("Invalid action for start\n");
		return 1;
	}
	if (strcmp(argv[4],".create") == 0){
		if (argc != MAXARGC){
			printf ("Unvalid number of parametres create\n");
			return 1;
		}
	}else if (argc != MAXARGC){
		printf("Unvalid number of parametres join\n");
		return 1;
	}
	return 0;
}
		
int StartGame(double fd, char **argv, double argc)
{
	char *buf = new char[BUFSIZE];
	char *name = new char[BUFSIZE];
	char *type = new char[BUFSIZE];
	sprintf(name, "%s\n",argv[3]);
	buf = ReadBot(buf, fd);
	write(fd, name, StrLen(name));
	buf = ReadBot(buf, fd);
	if (strcmp(argv[4],".create") == 0){
		sprintf(type, "%s\n",argv[4]);
		write(fd, type, StrLen(type));
		buf = ReadBot(buf, fd);
		int numpl = StrToNum(argv[5]);
		WaitOther(fd, numpl);
	}else{
		sprintf(type, "%s %s\n", argv[4], argv[5]);
		write(fd, type, StrLen(type));
		buf = ReadBot(buf, fd);
	}
	return fd;
}

int main(int argc, char **argv)
{
	char *p = argv[2];
	double port = StrToNum(p), fd = socket(AF_INET, SOCK_STREAM, 0);
	struct in_addr *inp;
	struct sockaddr_in addr;
	int o = Check(argv, argc);
	if (o == 1)
		return 1;
	if (0 == inet_aton(argv[1], &(addr.sin_addr))){
		printf ("Unvalid IP!\n");
		return 1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (0 != connect (fd, (struct sockaddr *) &addr,sizeof(addr))){
		perror ("Connect: ");
		return 1;
	}
	int res = StartGame(fd, argv, argc);
	if (res != fd)
		return 0;
	for(;;);
	return 0;
}	
