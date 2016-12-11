//复杂声明解析器
//能够解析指针、数组、函数
//前置符号递归读取，后置符号循环读取

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

static Type *ty, *tys;

void declare_init(void) {
	static int is_init = 0;
	if(!is_init) {
		tys = ty = (Type*)malloc(MAXSIZE * sizeof(Type));
		is_init = 1;
	}
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

static Type* specifier(void) {
	if(tki == Int) {
		next();
		return deriv_type(INT, NULL, 0);
	} else { printf("error11!\n"); exit(-1); }
}

static int lev(char *opr) {
	char *oprs[] = {
		")", "]",
		"", "*",
		"", "(", "["
	};
	int lev = 1;
	for(int i = 0; i < sizeof(oprs) / sizeof(*oprs); i++) {
		if(!strcmp(oprs[i], opr)) {
			return lev;
		} else if(!strcmp(oprs[i], "")) {
			lev++;
		}
	}
	return 0; //其他符号
}

static Id* declarator(Type *type);
static int* complex(char *last_opr, int *cpx) { //复杂类型分析
	//前置符号
	if(!strcmp(tks, "*")) { //指针
		next();
		cpx = complex("*", cpx);
		cpx++;
		*cpx++ = PTR;
	} else if(!strcmp(tks, "(")) { //括号
		next();
		cpx = complex(")", cpx);
		if(strcmp(tks, ")")) { printf("error12!\n"); exit(-1); } //"("无法匹配到")"
		next();
	} else if(tki == ID) {
		(id - 1) -> name = tks;
		next();
	} else { printf("error13!\n"); exit(-1); }
	
	//next();
	//后置符号
	while(lev(tks) > lev(last_opr)) {
		if(!strcmp(tks, "[")) { //数组
			next();
			int count = 0;
			if(strcmp(tks, "]")) {
				count = expr_int("");
			}
			*cpx++ = count;
			*cpx++ = ARR;
			if(strcmp(tks, "]")) { printf("error15!\n"); exit(-1); }
		} else if(!strcmp(tks, "(")) { //函数或函数指针
			int count = 0;
			inparam();
			next();
			if(strcmp(tks, ")")) {
				while(1) {
					count++;
					Type *type = specifier();
					declarator(type);
					if(!strcmp(tks, ")")) break;
					else if(!strcmp(tks, ",")) next();
					else { printf("error16!\n"); exit(-1); }
				}
			}
			*cpx++ = count;
			*cpx++ = FUN;
		} else { printf("error17!\n"); exit(-1); }
		next();
	}
	return cpx; //update cpx
}

static Id* declarator(Type *type) {
	//if(strcmp(tks, "*") && strcmp(tks, "(") && tki != ID) { printf("error18!\n"); exit(-1); }
	Id *this_id = id++;
	int cpxs[BUFSIZE]; //复杂类型栈
	int *cpx = cpxs; //复杂类型栈栈顶指针
	cpx = complex("", cpx);
	while(cpx > cpxs) {
		int base = *--cpx;
		int count = *--cpx;
		type = deriv_type(base, type, count);
	}
	setid(this_id, type);
	
	if(type -> base == PTR) { //函数指针*
		Type *rely = type -> rely;
		while(rely -> base == PTR) rely = rely -> rely;
		if(rely -> base == FUN) id = this_id + 1;//infunc(); outfunc();
	} else if(type -> base == FUN && this_id -> class == ARG) { //函数为形参
		this_id -> type = deriv_type(PTR, type, 0);
		id = this_id + 1;//infunc(); outfunc();
	} else if(type -> base == ARR && this_id -> class == ARG) { //数组为形参
		this_id -> type = deriv_type(PTR, type -> rely, 0);
	}//print_type(this_id);printf("\n");
	return this_id;
}

void declare(int env) {
	static int varc;
	if(env == GLO) {//printf("-%s-\n",tks);
		Type *type = specifier();
		Id *this_id = declarator(type);
		if(this_id -> type -> base == FUN) {
			if(!strcmp(tks, "{")) {
				infunc();
				varc = 0;
				this_id -> offset = e - emit;
				*e++ = PUSH; *e++ = BP;
				*e++ = MOV; *e++ = BP; *e++ = SP; //bp = sp
				*e++ = INC; *e++ = SP; int *_e = e++;
				next();
				while(strcmp(tks, "}")) {
					if(tki == Int) declare(LOC);
					else stmt();
					next();
				}
				*_e = varc;
				*e++ = MOV; *e++ = SP; *e++ = BP; //sp = bp
				*e++ = POP; *e++ = BP;
				*e++ = POP; *e++ = IP;
				outfunc();
			} else if(!strcmp(tks, ";")) {
				id = this_id + 1;//infunc(); outfunc();
			} else { printf("error19!\n"); exit(-1); }
		} else {
			while(1) {
				if(!strcmp(tks, "=")) {
					next();
					if(this_id -> type -> base == INT) {
						*(data + this_id -> offset) = expr_int("");
					} else if(this_id -> type -> base == PTR) {
						*(data + this_id -> offset) = expr_null();
					} else if(this_id -> type -> base == ARR) {
						expr_arr(GLO, this_id -> type, this_id -> offset);
					} else { printf("error20!\n"); exit(-1); }
				} else {
					if(this_id -> type -> base == INT) *(data + this_id -> offset) = 0;
					else if(this_id -> type -> base == PTR) *(data + this_id -> offset) = 0;
					else if(this_id -> type -> base == ARR) memset(data + this_id -> offset, 0, this_id -> type -> count);
					else { printf("error21!\n"); exit(-1); }
				}
				if(!strcmp(tks, ";")) break;
				else if(!strcmp(tks, ",")) {
					next();
					this_id = declarator(type);
				} else { printf("error22!\n"); exit(-1); }
			}
		}
	} else if(env == LOC) {
		Type *type = specifier();
		while(1) {
			//varc++;
			Id *this_id = declarator(type);
			if(!strcmp(tks, "=")) {
				next();
				if(this_id -> type -> base == INT) {
					*e++ = AL; *e++ = this_id -> offset;
					*e++ = PUSH; *e++ = AX;
					if(this_id -> type != expr("").type) { printf("error23!\n"); exit(-1); }
					*e++ = ASS;
				} else if(this_id -> type -> base == PTR) {
					*e++ = AL; *e++ = this_id -> offset;
					*e++ = PUSH; *e++ = AX;
					if(this_id -> type != expr("").type) { printf("error24!\n"); exit(-1); }
					*e++ = ASS;
				} else if(this_id -> type -> base == ARR) {
					expr_arr(LOC, this_id -> type, this_id -> offset);
				}
			}
			varc += typesize(this_id -> type);
			if(!strcmp(tks, ";")) break;
			else if(!strcmp(tks, ",")) next();
			else { printf("error25!\n"); exit(-1); }
		}
	}
}
