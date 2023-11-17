#ifndef STMT_H
#define STMT_H

typedef enum
{
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK
} stmt_t;

struct stmt
{
	stmt_t kind;
	struct decl* decl;
	struct expr* init_expr;
	struct expr* expr;
	struct expr* next_expr;
	struct stmt* body;
	struct stmt* else_body;
	struct stmt* next;
};

struct stmt* stmt_create(
	stmt_t kind,
	struct decl* decl,
	struct expr* init_expr,
	struct expr* expr,
	struct expr* next_expr,
	struct stmt* body,
	struct stmt* else_body,
	struct stmt* next
);

struct stmt* stmt_create_empty(stmt_t kind);
void stmt_print(const struct stmt* s, int indent);
void stmt_resolve(const struct stmt* s);
void stmt_typecheck(const struct stmt* s);;

#endif
