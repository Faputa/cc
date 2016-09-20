//Óï¾ä·ÖÎö

#include "cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stmt() {
	if(!strcmp(tks, "{")) {
		inblock();
		next();
		while(strcmp(tks, "}")) {
			if(tki == Int) declare(LOC);
			else stmt();
			next();
		}
		outblock();
	} else if(tki == If) {
		next(); if(strcmp(tks, "(")) { printf("error87!\n"); exit(-1); }
		next(); expr(")");
		if(strcmp(tks, ")")) { printf("error88!\n"); exit(-1); }
		next();
		*e++ = JZ; int *_e1 = e++;
		stmt();
		char *_p = p;
		next();
		if(tki == Else) {
			*e++ = JMP; int *_e2 = e++;
			*_e1 = e - emit;
			next();
			stmt();
			*_e2 = e - emit;
		} else {
			*_e1 = e - emit;
			p = _p;
		}
	} else if(tki == Do) {
		int *_e1 = e;
		next();
		stmt();
		next();
		if(tki == While) next(); else { printf("error89!\n"); exit(-1); }
		if(strcmp(tks, "(")) { printf("error90!\n"); exit(-1); }
		next();
		expr(")");
		if(strcmp(tks, ")")) { printf("error91!\n"); exit(-1); }
		next();
		if(strcmp(tks, ";")) { printf("error92!\n"); exit(-1); }
		*e++ = JZ; int *_e2 = e++;
		*e++ = JMP; *e++ = _e1 - emit;
		*_e2 = e - emit;
	} else if(tki == For) {
		inblock();
		next();
		if(!strcmp(tks, "(")) next(); else { printf("error93!\n"); exit(-1); }
		if(strcmp(tks, ";")) {
			if(tki == Int) declare(LOC);
			else expr("");
			if(strcmp(tks, ";")) { printf("error94!\n"); exit(-1); }
		}
		next();
		int *_e1 = e;
		int *_e4;
		if(strcmp(tks, ";")) {
			expr("");
			if(strcmp(tks, ";")) { printf("error95!\n"); exit(-1); }
			*e++ = JZ; _e4 = e++;
		}
		*e++ = JMP; int *_e3 = e++; 
		next();
		int *_e2 = e;
		if(strcmp(tks, ")")) {
			expr(")");
			if(strcmp(tks, ")")) { printf("error96!\n"); exit(-1); }
			*e++ = JMP; *e++ = _e1 - emit;
		}
		next();
		*_e3 = e - emit;
		stmt();
		*e++ = JMP; *e++ = _e2 - emit;
		*_e4 = e - emit;
		outblock();
	} else if(tki == While) {
		int *_e1 = e;
		next(); if(strcmp(tks, "(")) { printf("error97!\n"); exit(-1); }
		next(); expr(")");
		if(strcmp(tks, ")")) { printf("error98!\n"); exit(-1); }
		next();
		*e++ = JZ; int *_e2 = e++;
		stmt();
		*e++ = JMP; *e++ = _e1 - emit;
		*_e2 = e - emit;
	} else if(tki == Return) {
		next(); expr("");
		*e++ = MOV; *e++ = SP; *e++ = BP; //sp = bp
		*e++ = POP; *e++ = BP;
		*e++ = POP; *e++ = IP;
	} else {
		if(strcmp(tks, ";")) {
			expr("");
			if(strcmp(tks, ";")) { printf("error99!\n"); exit(-1); }
		}
	}
}
