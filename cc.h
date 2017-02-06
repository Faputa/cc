#ifndef _CC_H_
#define _CC_H_

#define MAXSIZE 10000
#define BUFSIZE 100

typedef struct Type {
	int base;
	int count;
	struct Type *rely;
	struct Type **argtyls;
} Type;

typedef struct {
	int csmk; //class mark
	char *name;
	Type *type;
	int offset;
	int class;
} Id;

typedef struct {
	Type *type;
	int is_lvalue;
	int is_const;
} Er;

typedef void (*Api)();

enum {
	//keyword
	Int, If, Else, While, Do, For, Return, Null,
	//type
	INT, FUN, API, PTR, ARR,
	//other integer
	ID, GLO, LOC, ARG,
	//opcode
	PUSH, POP, SET, INC, DEC, JMP, JZ, MOV, ADD, SUB, MUL, DIV, MOD, ASS, EQ, GT, LT, AND, OR, NOT, AG, AL, VAL, CALL, CAPI, EXIT,
	//reg
	IP = 0, BP, SP, AX
};

extern Id *id;
extern char *p, *tks;
extern int *e, *emit, *data, tki;

//ident.c
void ident_init(void);
void print_ids(void);
void setid(Id *id, Type *type);
Id* getid(char *tks);
void inblock(void);
void outblock(void);
void inparam(void);
void infunc(void);
void outfunc(void);

//type.c
void type_init(void);
int typesize(Type *type);
Type* deriv_type(int base, Type *rely, int count);
void print_type(Id *this_id);

//declare.c
void declare(int env);

//stmt.c
void stmt(void);

//expr.c
int expr_null(void);
int expr_int(char *last_opr);
void expr_arr(int env, Type *type, int offset);
Er expr(char *last_opr);

//token.c
void token_init(void);
void next(void);

//vm.c
void vm_init(void);
void vm_run(int src, int debug);

//api.c
void api_init(void);
void api_register(Api fun, char *proto);
void api_call(int offset);
int api_getarg(int index);
void api_return(int result);

#endif