//维护符号表

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

Id *id;
static Id *ids;

void id_init() {
	static int is_init = 0;
	if(!is_init) {
		ids = id = (Id*)malloc(MAXSIZE * sizeof(Id));
		id++ -> csmk = GLO;
		is_init = 1;
	}
}

void print_ids(Id *this_id) {
	printf("- %s -\n", this_id -> name);
	for(Id *i = ids; i < id; i++){
		if(i -> csmk == GLO) printf("GLO");
		else if(i -> csmk == FUN) printf("FUN");
		else if(i -> csmk == LOC) printf("LOC");
		else if(i -> csmk == ID) {
			printf("%s ", i -> name);
			printf("%d ", i -> offset);
			if(i -> class == GLO) printf("GLO");
			else if(i -> class == FUN) printf("FUN");
			else if(i -> class == LOC) printf("LOC");
		}
		printf("\n");
	}
	printf("\n");
}

int typesize(Type *type) {
	if(type -> base == INT) return 1;
	else if(type -> base == PTR) return 1;
	else if(type -> base == ARR) return typesize(type -> rely) * type -> count;
	return 0;
}

void setid(Id *this_id, Type* type) {
	for(Id *i = this_id - 1; i -> csmk == ID; i--) {
		if(!strcmp(tks, i -> name)) { printf("error83!\n"); exit(-1); }
	}
	
	//this_id -> name = tks;
	this_id -> type = type;
	this_id -> csmk = ID;
	
	Id *last_id = this_id - 1;
	if(last_id -> csmk != ID) {
		this_id -> class = last_id -> csmk;
	} else {
		this_id -> class = last_id -> class;
	}
	
	while(last_id -> csmk == LOC ||
	      (last_id -> csmk == ID &&
	       (last_id -> type -> base == FUN ||
	        last_id -> type -> base == API))) last_id--; //可以证明：type等于FUN或API时csmk一定等于ID
	
	if(this_id -> class == GLO) {
		if(last_id -> csmk == GLO) {
			this_id -> offset = MAXSIZE - typesize(type);
			//if(this_id -> type -> base == INT) this_id -> offset = MAXSIZE - 1;
			//else if(this_id -> type -> base == PTR) this_id -> offset = MAXSIZE - 1;
			//else if(this_id -> type -> base == ARR) this_id -> offset = MAXSIZE - type -> count;
		} else {
			this_id -> offset = last_id -> offset - typesize(type);
			//if(last_id -> type -> base == INT) this_id -> offset = last_id -> offset - 1;
			//else if(last_id -> type -> base == PTR) this_id -> offset = last_id -> offset - 1;
			//else if(last_id -> type -> base == ARR) this_id -> offset = last_id -> offset - type -> count;
		}
	} else {
		if(last_id -> csmk == ARG || last_id -> csmk == FUN || last_id -> offset < 0) { //offset小于0为参数，且此时csmk不可能为LOC
			this_id -> offset = 0;
		} else {
			this_id -> offset = last_id -> offset + typesize(last_id -> type);
			//if(last_id -> type -> base == INT) this_id -> offset = last_id -> offset + 1;
			//else if(last_id -> type -> base == PTR) this_id -> offset = last_id -> offset + 1;
			//else if(last_id -> type -> base == ARR) this_id -> offset = last_id -> offset + type -> count;
		}
	}
}

Id* getid(char *tks) {
	for(Id *i = id - 1; i >= ids; i--) {
		if(i -> csmk == ID && !strcmp(tks, i -> name)) return i;
	}
	printf("error84!"); exit(-1);
}

void inblock() {
	(id++) -> csmk = LOC;
}

void outblock() {
	while(id -> csmk != LOC) {
		id--;
	}
	//id -> csmk = 0;
}

void inparam() {
	(id++) -> csmk = ARG;
}

void infunc() {
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

void outfunc() {
	while(id -> csmk != FUN) {
		id--;
	}
	//id -> csmk = 0;
}
