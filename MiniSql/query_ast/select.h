#pragma once
#include <vector>
#include "expression.h"

#include "ast_base.h"

namespace minisql::query_ast
{
	struct from_clause : public ast_base
	{
	};

	struct where_clause : public ast_base
	{
	};


	struct order_by_clause : public ast_base
	{
	};

	struct select_clause : public ast_base
	{
		const std::vector<arith_expression_base> columns;
		const from_clause from;
		const where_clause where;
		const order_by_clause order_by;
		select_clause(const from_clause& from, const where_clause& where, const order_by_clause& order_by)
			: from(from), where(where), order_by(order_by) {}
	};

	struct scalar_select : public ast_base
	{
		const std::unique_ptr<arith_expression_base> expression;
	};
}
