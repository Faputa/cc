//外部接口

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

static Api *api, *apis;

void api_init(void) {
	apis = api = (Api*)malloc(MAXSIZE * sizeof(Api));
}

void api_register(Api fun, char *proto) {
	char *_p = p;
	p = proto;
	next();
	Id *this_id = id;
	declare(GLO);
	if(this_id -> type -> base != FUN) { printf("error1!\n"); exit(-1); }
	this_id -> type -> base = API;
	this_id -> offset = api - apis;
	*api++ = fun;
	p = _p;
}

void api_call(int offset) {
	apis[offset]();
}

int api_getarg(int index) { //index == n 表示第n个参数
	return *(data + *(SP + data) - index);
}

void api_return(int result) {
	*(data + AX) = result;
}