#pragma once
#include <string>
#include <vector>
#include "expression.h"

#include "ast_base.h"

namespace minisql::query_ast
{
	struct from_clause : public ast_base
	{
		from_clause(const std::string& raw) : ast_base(raw) {}
	};

	struct where_clause : public ast_base
	{
		where_clause(const std::string& raw) : ast_base(raw) {}
	};


	struct order_by_clause : public ast_base
	{
		order_by_clause(const std::string& raw) : ast_base(raw) {}
	};

	struct select_clause : public ast_base
	{
		const std::vector<expression> columns;
		const from_clause from;
		const where_clause where;
		const order_by_clause order_by;
		select_clause(const std::string& raw, const from_clause& from, const where_clause& where, const order_by_clause& order_by)
			: ast_base(raw), from(from), where(where), order_by(order_by) {}
	};

	struct scalar_select : public ast_base
	{
		const expression expression;
	};
}
