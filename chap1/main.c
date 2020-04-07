#include "slp.h"
#include "prog1.h"
#include <stdio.h>
#include <string.h>

int maxargs(A_stm stm);

int maxargsForExp(A_exp exp) {
	if (exp->kind == A_idExp || exp->kind == A_numExp) return 0;
	if (exp->kind == A_opExp) {
		return maxargsForExp(exp->u.op.left) + maxargsForExp(exp->u.op.right);
	} else {
		return maxargs(exp->u.eseq.stm) + maxargsForExp(exp->u.eseq.exp);
	}
}

int maxargsForExpList(A_expList exps) {
	if (exps->kind == A_lastExpList) {
		return maxargsForExp(exps->u.last);
	} else {
		return maxargsForExp(exps->u.pair.head) + maxargsForExpList(exps->u.pair.tail);
	}
}

int maxargs(A_stm stm) {
	if (stm->kind == A_printStm) {
		return 1 + maxargsForExpList(stm->u.print.exps);
	} else if (stm->kind == A_compoundStm) {
		return maxargs(stm->u.compound.stm1) + maxargs(stm->u.compound.stm2);
	} else {
		return maxargsForExp(stm->u.assign.exp);
	}
}

typedef struct table *Table_;
struct table {string id; int value; Table_ tail;};
Table_ Table(string id, int value, struct table *tail) {
	Table_ t = malloc(sizeof(*t));
	t->id = id; t->value = value; t->tail = tail;
	return t;
}
typedef struct intAndTable *IntAndTable_;
struct intAndTable {int i; Table_ t;};
IntAndTable_ IntAndTable(int i, Table_ t) {
	IntAndTable_ it = malloc(sizeof(*it));
	it->i = i; it->t = t;
	return it;
}

IntAndTable_ interpExpList(A_expList exps, Table_ t);
IntAndTable_ interpExp(A_exp exp, Table_ t);
Table_ interpStm(A_stm stm, Table_ t) {
	if (stm->kind == A_compoundStm) {
		t = interpStm(stm->u.compound.stm1, t);
		return interpStm(stm->u.compound.stm2, t);
	} else if (stm->kind == A_assignStm) {
		IntAndTable_ it = interpExp(stm->u.assign.exp, t);
		return Table(stm->u.assign.id, it->i, it->t);
	} else {
		IntAndTable_ it = interpExpList(stm->u.print.exps, t);
		return it->t;
	}
}

IntAndTable_ interpExpList(A_expList exps, Table_ t) {
	if (exps->kind == A_pairExpList) {
		IntAndTable_ it = interpExp(exps->u.pair.head, t);
		printf("%d\n", it->i);
		return interpExpList(exps->u.pair.tail, it->t);
	} else {
		IntAndTable_ it = interpExp(exps->u.last, t);
		printf("%d\n", it->i);
		return it;
	}
}

IntAndTable_ interpExp(A_exp exp, Table_ t) {
	if (exp->kind == A_idExp) {
		int v = lookupId(t, exp->u.id);
		return IntAndTable(v, t);
	} else if (exp->kind == A_numExp) {
		return IntAndTable(exp->u.num, t);
	} else if (exp->kind == A_opExp) {
		IntAndTable_ it = interpExp(exp->u.op.left, t);
		int v1 = it->i;
		it = interpExp(exp->u.op.right, it->t);
		int v2 = it->i;
		switch (exp->u.op.oper) {
			case A_plus: return IntAndTable(v1 + v2, it->t);
			case A_minus: return IntAndTable(v1 - v2, it->t);
			case A_times: return IntAndTable(v1 * v2, it->t);
			default: return IntAndTable(v1 / v2, it->t);
		}
	} else {
		t = interpStm(exp->u.eseq.stm, t);
		return interpExp(exp->u.eseq.exp, t);
	}
}

int lookupId(Table_ t, string key) {
	while (t && strcmp(t->id, key)) {
		t = t->tail;
	}
	if (t) return t->value;
	return 0;
}

int main() {
	printf("%d\n", maxargs(prog()));
	interpStm(prog(), NULL);
}
