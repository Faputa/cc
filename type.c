#include "cc.h"
#include <stdio.h>
#include <malloc.h>

static Type *ty, *tys;
static Type **argtyls, **argtylss;

void type_init(void) {
	tys = ty = (Type*)malloc(MAXSIZE * sizeof(Type));
	argtylss = argtyls = (Type**)malloc(MAXSIZE * sizeof(Type*));
}

static Type** getargtyls(int count) {
	if(count == 0) return NULL;
	Id *_id = id - count;
	Type **_argtyls = argtylss;
	int i = 0;
	while(_argtyls < argtyls) {
		i = 0;
		while(i < count && i < argtyls - _argtyls) {
			if(_id[i].type != _argtyls[i]) break;
			i++;
		}
		if(i == count) return _argtyls;
		else if(i == argtyls - _argtyls) break;
		else _argtyls++;
	}
	while(i < count) {
		*argtyls++ = _id[i++].type;
	}
	return _argtyls;
}

int typesize(Type *type) {
	if(type -> base == INT) return 1;
	else if(type -> base == PTR) return 1;
	else if(type -> base == ARR) return typesize(type -> rely) * type -> count;
	return 0;
}

Type* deriv_type(int base, Type *rely, int count) { //类型生成
	if(rely == NULL) {
		if(base == INT) {
			for(Type *i = tys; i < ty; i++) {
				if(i -> base == base
				&& i -> rely == NULL) return i;
			}
			ty -> base = base;
			ty -> rely = NULL;
			return ty++;
		} else { printf("error7!\n"); exit(-1); }
	} else {
		if(base == PTR) {
			for(Type *i = tys; i < ty; i++) {
				if(i -> base == base
				&& i -> rely == rely) return i;
			}
			ty -> base = base;
			ty -> rely = rely;
			return ty++;
		} else if(base == ARR) {
			if(rely -> base == FUN) { printf("error8!\n"); exit(-1); }
			for(Type *i = tys; i < ty; i++) {
				if(i -> base == base
				&& i -> rely == rely
				&& i -> count == count) return i;
			}
			ty -> base = base;
			ty -> rely = rely;
			ty -> count = count;
			return ty++;
		} else if(base == FUN) {// || base == API) {
			if(rely -> base == FUN || rely -> base == ARR) { printf("error9!\n"); exit(-1); }
			Type **argtyls = getargtyls(count);
			for(Type *i = tys; i < ty; i++) {
				if(i -> base == base
				&& i -> rely == rely
				&& i -> count == count
				&& i -> argtyls == argtyls) return i;
			}
			ty -> base = base;
			ty -> rely = rely;
			ty -> count = count;
			ty -> argtyls = argtyls;
			return ty++;
		} else { printf("error10!\n"); exit(-1); }
	}
}

static void _print_type(Type *type) {
	if(type -> base == PTR) {
		printf("指向");
		_print_type(type -> rely);
		printf("的指针");
	} else if(type -> base == ARR) {
		printf("拥有%d个类型为", type -> count);
		_print_type(type -> rely);
		printf("的元素的数组");
	} else if(type -> base == FUN) {
		//printf("需要%d个参数且返回值为", type -> count);
		for(int i = 0; i < type -> count; i++) {
			printf("第%d个参数为", i + 1);
			_print_type(type -> argtyls[i]);
			printf("、");
		}
		printf("返回值为");
		_print_type(type -> rely);
		printf("的函数");
	} else if(type -> base == API) {
		printf("需要%d个参数且返回值为", type -> count);
		_print_type(type -> rely);
		printf("的API");
	} else if(type -> base == INT) {
		printf("整型");
	}
}

void print_type(Id *this_id) {
	printf("%s为", this_id -> name);
	_print_type(this_id -> type);
	//printf("\n");
}
