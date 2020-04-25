#pragma once
#include <string>
#include <variant>
#include "ast_base.h"

namespace minisql::query_ast
{
	struct column_identifier : public ast_base
	{
		const std::string identifier;
		column_identifier() : ast_base(""), identifier() {}
		column_identifier(const std::string& raw) : ast_base(raw), identifier(raw) {}
	};

	struct number : public ast_base
	{
		double value;
		number(const std::string& raw, double value) : ast_base(raw), value(value) {}
	};

	struct expression : public ast_base
	{
		using expression_t = std::variant<number, column_identifier>;
		const expression_t expr;

		expression(const std::string& raw, const number& val) : ast_base(raw), expr(val) {}
		expression(const std::string& raw, const column_identifier& val) : ast_base(raw), expr(val) {}
		expression(const std::string& raw, const expression_t& val) : ast_base(raw), expr(val) {}
	};

}
