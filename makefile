all:
	@gcc api.c cc.c declare.c expr.c ident.c stmt.c token.c type.c vm.c -o cc -Wall