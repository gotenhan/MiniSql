#pragma once

#include "../parsers.h"
#include "../query_ast.h"

namespace minisql::query_parser
{
	/*
	using namespace parser;
	class column_expression final : parser_base<query_ast::column_expression>
	{
	public:
		[[nodiscard]] std::string to_string() const override;
	private:
		auto parse(const std::string& input,
			unsigned& current_pos) const -> parse_result<query_ast::column_expression> override;
	};

	class select : parser::parser_base<query_ast::select_clause>
	{
	public:
		[[nodiscard]] std::string to_string() const override;
	private:
		auto parse(const std::string& input,
			unsigned& current_pos) const -> parse_result<query_ast::select_clause> override;
		
	};

	inline auto column_expression::parse(const std::string& input, unsigned& current_pos) const
		-> parse_result<query_ast::column_expression>
	{
	}

	inline auto select::parse(const std::string& input,
	                          unsigned& current_pos) const -> parse_result<query_ast::select_clause>
	{
		auto p =
			(string_literal("select"),
				whitespace(),
				one_or_more(column_expression()));


	}
*/
}
