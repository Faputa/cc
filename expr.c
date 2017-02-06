//表达式分析

#include "cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int expr_null(void) {
	if(tki == Null) {
		next();
		return 0;
	} else { printf("error26!\n"); exit(-1); }
}

void expr_arr(int env, Type *type, int offset) {
	if(env == GLO) {
		memset(data + offset, 0, type -> count);
		if(strcmp(tks, "{")) { printf("error27!\n"); exit(-1); }
		next();
		if(strcmp(tks, "}")) {
			int count = 0;
			while(1) {
				count++;
				if(type -> rely -> base == INT) {
					*(data + offset) = expr_int("");
				} else if(type -> rely -> base == PTR) {
					*(data + offset) = expr_null();
				} else if(type -> rely -> base == ARR) {
					expr_arr(GLO, type -> rely, offset);
				}
				if(!strcmp(tks, "}")) {
					break;
				} else if(!strcmp(tks, ",")) {
					offset += typesize(type -> rely);
					next();
				} else { printf("error28!\n"); exit(-1); }
			}
			if(count > type -> count) { printf("error29!\n"); exit(-1); }
		}
		next();
	} else if(env == LOC) {
		if(strcmp(tks, "{")) { printf("error30!\n"); exit(-1); }
		next();
		if(strcmp(tks, "}")) {
			int count = 0;
			while(1) {
				count++;
				if(type -> rely -> base == INT) {
					*e++ = AL; *e++ = offset;
					*e++ = PUSH; *e++ = AX;
					if(type -> rely != expr("").type) { printf("error31!\n"); exit(-1); }
					*e++ = ASS;
				} else if(type -> rely -> base == PTR) {
					*e++ = AL; *e++ = offset;
					*e++ = PUSH; *e++ = AX;
					if(type -> rely != expr("").type) { printf("error32!\n"); exit(-1); }
					*e++ = ASS;
				} else if(type -> rely -> base == ARR) {
					expr_arr(LOC, type -> rely, offset);
				}
				if(!strcmp(tks, "}")) {
					break;
				} else if(!strcmp(tks, ",")) {
					offset += typesize(type -> rely);
					next();
				} else { printf("error33!\n"); exit(-1); }
			}
			if(count > type -> count) { printf("error34!\n"); exit(-1); }
		}
		next();
	}
}

static int lev(char *opr) { //优先级越高lev越大，其他符号lev为0
	char *oprs[] = {
		")", "]",
		"", "&&", "||", "!",
		"", "==", "!=",
		"", ">", "<", ">=", "<=",
		"", "+", "-",
		"", "*", "/", "%",
		"", "=",
		"", "*_", "&_",
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

int expr_int(char *last_opr) {
	int a;
	if(tki == INT) {
		a = atoi(tks);
		next();
	} else if(!strcmp(tks, "(")) {
		next();
		a = expr_int(")");
		if(!strcmp(tks, ")")) next(); else { printf("error37!\n"); exit(-1); }
	} else if(!strcmp(tks, "!")) {
		next();
		a = !expr_int("!");
	} else if(!strcmp(tks, "-")) {
		next();
		a = -expr_int("-");
	} else { printf("error38!\n"); exit(-1); }
	
	while(lev(tks) > lev(last_opr)) {
		char *opr = tks;
		next();
		int b = expr_int(opr);
		if (!strcmp(opr, "+")) a += b;
		else if(!strcmp(opr, "-")) a -= b;
		else if(!strcmp(opr, "*")) a *= b;
		else if(!strcmp(opr, "/")) a /= b;
		else if(!strcmp(opr, "%")) a %= b;
		else if(!strcmp(opr, ">")) a = a > b;
		else if(!strcmp(opr, "<")) a = a < b;
		else if(!strcmp(opr, "==")) a = a == b;
		else if(!strcmp(opr, ">=")) a = a >= b;
		else if(!strcmp(opr, "<=")) a = a <= b;
		else if(!strcmp(opr, "!=")) a = a != b;
		else if(!strcmp(opr, "&&")) a = a && b;
		else if(!strcmp(opr, "||")) a = a || b;
		else { printf("error39!\n"); exit(-1); }
	}
	return a;
}

Er expr(char *last_opr) { //1 + 2 ^ 3 * 4 == (1 + (2 ^ (3) * (4)))
	Er er = {NULL, 0, 1};
	if(tki == INT) {
		er.type = deriv_type(INT, NULL, 0);
		*e++ = SET; *e++ = AX; *e++ = atoi(tks);
		next();
	} else if(tki == ID) {
		Id *this_id = getid(tks);
		er.type = this_id -> type;
		*e++ = this_id -> class == GLO ? AG: AL; *e++ = this_id -> offset;
		if(er.type -> base == INT || er.type -> base == PTR) {
			*e++ = VAL;
			er.is_const = 0;
		}
		er.is_lvalue = 1;
		next();
	} else if(!strcmp(tks, "(")) {
		next();
		er.type = expr(")").type;
		if(strcmp(tks, ")")) { printf("error38!\n"); exit(-1); }
		next();
	} else if(!strcmp(tks, "*")) {
		next();
		er.type = expr("*_").type;
		if(er.type -> base != PTR) { printf("error39!\n"); exit(-1); }
		er.type = er.type -> rely;
		if(er.type -> base == INT || er.type -> base == PTR) {
			*e++ = VAL;
			er.is_const = 0;
		}
		er.is_lvalue = 1;
	} else if(!strcmp(tks, "&")) {
		next();
		Er _er = expr("&_");
		if(!_er.is_lvalue) { printf("error40!\n"); exit(-1); }
		if(_er.type -> base == INT || _er.type -> base == PTR) e--;
		er.type = deriv_type(PTR, _er.type, 0);
	} else if(!strcmp(tks, "!")) {
		next();
		er.type = expr("!").type;
		if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
		*e++ = NOT;
	} else if(!strcmp(tks, "-")) {
		next();
		*e++ = SET; *e++ = AX; *e++ = 0;
		*e++ = PUSH; *e++ = AX;
		er.type = expr("-").type;
		if(er.type -> base != INT) { printf("error56!\n"); exit(-1); }
		*e++ = SUB;
	} else { printf("error41!\n"); exit(-1); }
	
	while(lev(tks) > lev(last_opr)) {
		*e++ = PUSH; *e++ = AX;
		if(!strcmp(tks, "=")) {
			next();
			e -= 3; *e++ = PUSH; *e++ = AX;
			if(er.type != expr("").type) { printf("error42!\n"); exit(-1); }
			if(!er.is_const) *e++ = ASS; else { printf("error43!\n"); exit(-1); }
		} else if(!strcmp(tks, "(")) {
			next();
			int argc = 0;
			if(er.type -> base == PTR) er.type = er.type -> rely; //将函数指针转化为函数
			if(er.type -> base != FUN && er.type -> base != API) { printf("error44!\n"); exit(-1); }
			if(strcmp(tks, ")")) {
				while(1) {
					if(argc > er.type -> count) { printf("error45!\n"); exit(-1); } //参数过多
					Type *argtype = expr(")").type;
					if(argtype -> base == FUN) argtype = deriv_type(PTR, argtype, 0);
					else if(argtype -> base == ARR) argtype = deriv_type(PTR, argtype -> rely, 0);
					if((er.type -> argtyls)[argc] != argtype) { printf("error46!\n"); exit(-1); } //参数类型检查
					*e++ = PUSH; *e++ = AX;
					argc++;
					if(!strcmp(tks, ")")) break;
					else if(!strcmp(tks, ",")) next();
					else { printf("error47!\n"); exit(-1); }
				}
			}
			next();
			if(argc < er.type -> count) { printf("error48!\n"); exit(-1); } //参数过少
			if(er.type -> base == FUN) {
				*e++ = SET; *e++ = AX; int *_e = e++; //set next ip
				*e++ = PUSH; *e++ = AX;
				*e++ = CALL; *e++ = argc;
				*_e = e - emit;
				*e++ = DEC; *e++ = SP; *e++ = argc + 1;
			} else if(er.type -> base == API) {
				*e++ = CAPI; *e++ = argc;
				*e++ = DEC; *e++ = SP; *e++ = argc + 1;
			}
			er.type = er.type -> rely;
		} else if(!strcmp(tks, "[")) {
			next();
			if(er.type -> base == PTR || er.type -> base == ARR) er.type = er.type -> rely;
			else { printf("error49!\n"); exit(-1); }
			if(expr("]").type -> base != INT) { printf("error50!\n"); exit(-1); }
			*e++ = PUSH; *e++ = AX;
			*e++ = SET; *e++ = AX; *e++ = typesize(er.type);
			*e++ = MUL;
			*e++ = ADD;
			if(er.type -> base == INT || er.type -> base == PTR) {
				*e++ = VAL;
				er.is_const = 0;
			}
			er.is_lvalue = 1;
			next();
		} else if(!strcmp(tks, "+")) {
			next();
			if(expr("+").type -> base != INT) { printf("error51!\n"); exit(-1); }
			if(er.type -> base == INT) {
				*e++ = ADD;
			} else if(er.type -> base == PTR || er.type -> base == ARR) {
				*e++ = PUSH; *e++ = AX;
				*e++ = SET; *e++ = AX; *e++ = typesize(er.type);
				*e++ = MUL;
				*e++ = ADD;
				if(er.type -> base == ARR) er.type = deriv_type(PTR, er.type -> rely, 0);
			} else { printf("error52!\n"); exit(-1); }
		} else if(!strcmp(tks, "-")) {
			next();
			if(expr("-").type -> base != INT) { printf("error53!\n"); exit(-1); }
			if(er.type -> base == INT) {
				*e++ = SUB;
			} else if(er.type -> base == PTR || er.type -> base == ARR) {
				*e++ = PUSH; *e++ = AX;
				*e++ = SET; *e++ = AX; *e++ = typesize(er.type);
				*e++ = MUL;
				*e++ = SUB;
				if(er.type -> base == ARR) er.type = deriv_type(PTR, er.type -> rely, 0);
			} else { printf("error54!\n"); exit(-1); }
		} else if(!strcmp(tks, "*")) {
			next();
			if(er.type -> base != INT) { printf("error55!\n"); exit(-1); }
			if(expr("*").type -> base != INT) { printf("error56!\n"); exit(-1); }
			*e++ = MUL;
		} else if(!strcmp(tks, "/")) {
			next();
			if(er.type -> base != INT) { printf("error57!\n"); exit(-1); }
			if(expr("/").type -> base != INT) { printf("error58!\n"); exit(-1); }
			*e++ = DIV;
		} else if(!strcmp(tks, "%")) {
			next();
			if(er.type -> base != INT) { printf("error59!\n"); exit(-1); }
			if(expr("%").type -> base != INT) { printf("error60!\n"); exit(-1); }
			*e++ = MOD;
		} else if(!strcmp(tks, "==")) {
			next();
			if(er.type != expr("==").type) { printf("error61!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = EQ;
		} else if(!strcmp(tks, ">")) {
			next();
			if(er.type != expr(">").type) { printf("error62!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = GT;
		} else if(!strcmp(tks, "<")) {
			next();
			if(er.type != expr("<").type) { printf("error63!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = LT;
		} else if(!strcmp(tks, "!=")) {
			next();
			if(er.type != expr("!=").type) { printf("error64!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = EQ;
			*e++ = NOT;
		} else if(!strcmp(tks, ">=")) {
			next();
			if(er.type != expr(">=").type) { printf("error65!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = LT;
			*e++ = NOT;
		} else if(!strcmp(tks, "<=")) {
			next();
			if(er.type != expr("<=").type) { printf("error66!\n"); exit(-1); }
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = GT;
			*e++ = NOT;
		} else if(!strcmp(tks, "&&")) {
			next();
			expr("&&");
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = AND;
		} else if(!strcmp(tks, "||")) {
			next();
			expr("||");
			if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
			*e++ = OR;
		} else { printf("error67!\n"); exit(-1); }
	}
	return er;
}
