#pragma once
#include <vector>
#include <numeric>
#include "expression.h"

#include "ast_base.h"

namespace minisql::query_ast
{
	/*
	struct from_clause : public ast_base
	{
	};

	struct where_clause : public ast_base
	{
	};


	struct order_by_clause : public ast_base
	{
	};
	*/

	struct select_clause : public ast_base
	{
		const std::vector<expression_base_ptr> columns;
		select_clause(std::vector<expression_base_ptr> columns) : ast_base(kind_t::select), columns(std::move(columns)) {}
		select_clause(const select_clause& other) : ast_base(kind_t::select), columns(other.columns) {}
		select_clause(select_clause&& other) : ast_base(kind_t::select), columns(std::move(other.columns)) {}

		[[nodiscard]] std::string to_string() const override {
			std::string ret = "select ";
			return std::accumulate(
				columns.begin(), columns.end(), ret,
				[&ret](const std::string& acc, expression_base_ptr e) { return ret += e->to_string(); });
		}

		[[nodiscard]] bool is_equal(const ast_base& other) const override { return kind == other.kind && *this == dynamic_cast<const select_clause&>(other); }
		[[nodiscard]] bool operator==(const select_clause& other) const
		{
			if (columns.size() != other.columns.size()) return false;
			for(unsigned i = 0; i < columns.size(); i++)
			{
				if (!columns[i]->is_equal(*other.columns[i])) return false;
			}

			return true;
		}
	};

}
