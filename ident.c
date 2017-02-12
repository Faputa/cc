//维护符号表

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

Id *id;

static Id *ids;

void ident_init(void) {
	ids = id = (Id*)malloc(MAXSIZE * sizeof(Id));
	id++ -> csmk = GLO;
}

void print_ids(void) {
	for(Id *i = ids; i < id; i++){
		if(i -> csmk == GLO) printf("GLO");
		else if(i -> csmk == ARG) printf("ARG");
		else if(i -> csmk == FUN) printf("FUN");
		else if(i -> csmk == LOC) printf("LOC");
		else if(i -> csmk == ID) {
			printf("%s ", i -> name);
			printf("%d ", i -> offset);
			if(i -> class == GLO) printf("GLO ");
			else if(i -> class == ARG) printf("ARG ");
			else if(i -> class == FUN) printf("FUN ");
			else if(i -> class == LOC) printf("LOC ");
			print_type(i);
		}
		printf("\n");
	}
	printf("\n");
}

void setid(Id *this_id, Type* type) {
	for(Id *i = this_id - 1; i -> csmk == ID; i--) {
		if(!strcmp(this_id -> name, i -> name)) { printf("error68!\n"); exit(-1); }
	}
	
	this_id -> type = type;
	this_id -> csmk = ID;
	
	Id *last_id = this_id - 1;
	while(last_id -> csmk == LOC ||
	      (last_id -> csmk == ID &&
	       (last_id -> type -> base == FUN ||
	        last_id -> type -> base == API))) last_id--; //可以证明：type等于FUN或API时csmk一定等于ID
	
	if(this_id -> class == GLO) {
		if(last_id -> csmk == GLO) this_id -> offset = MAXSIZE - typesize(type);
		else this_id -> offset = last_id -> offset - typesize(type);
	} else {
		if(last_id -> csmk == ARG || last_id -> csmk == FUN || last_id -> offset < 0) this_id -> offset = 0; //offset小于0为参数，且此时csmk不可能为LOC
		else this_id -> offset = last_id -> offset + typesize(last_id -> type);
	}
}

Id* getid(char *tks) {
	for(Id *i = id - 1; i >= ids; i--) {
		if(i -> csmk == ID && !strcmp(tks, i -> name)) return i;
	}
	printf("error69!"); exit(-1);
}

void inblock(void) {
	(id++) -> csmk = LOC;
}

void outblock(void) {
	do {
		id--;
	} while(id -> csmk != LOC);
	//id -> csmk = 0;
}

void inparam(void) {
	(id++) -> csmk = ARG;
}

void infunc(void) {
	Id *i;
	int argc = 0;
	for(i = id - 1; i -> csmk != ARG; i--) {
		argc++;
	}
	for(i = id - 1; i -> csmk != ARG; i--) {
		i -> offset -= argc + 2;
		i -> class = FUN;
	}
	i -> csmk = FUN;
}

void outfunc(void) {
	do {
		id--;
	} while(id -> csmk != FUN);
	//id -> csmk = 0;
}
