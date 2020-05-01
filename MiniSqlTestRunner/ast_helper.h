#pragma once
#include "../MiniSql/query_ast.h"

namespace minisql::query_ast::helpers
{
	static expression_base_ptr build(const double d) { return std::make_shared<query_ast::number>(d); }
	static expression_base_ptr build(const bool b) { return std::make_shared<query_ast::boolean>(b); }
	static expression_base_ptr build() { return std::make_shared<null>(); }
	static expression_base_ptr build(expression_base_ptr left, char op, expression_base_ptr right) { return std::make_shared<binary_op_expression>(left, binary_op(op), right); }

	template <typename T> static expression_base_ptr build(const std::string& s) { static_assert(false, "should not be called"); return nullptr; }
	template<> static inline expression_base_ptr build<query_ast::identifier>(const std::string& s) { return std::make_shared<query_ast::identifier>(s); }
	template<> static inline expression_base_ptr build<string>(const std::string& s) { return std::make_shared<string>(s); }

	static std::shared_ptr<ast_base> build(std::initializer_list<expression_base_ptr> items)
	{
		return std::make_shared<select_clause>(items);
	}
}
