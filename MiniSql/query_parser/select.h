#pragma once

#include "../parsers.h"
#include "../query_ast.h"
#include "expression.h"

namespace minisql::query_parser
{
	using namespace parsers;
	using namespace query_ast;

	namespace detail
	{
		inline const auto select_keyword_p = string_literal("select") << one_or_more(whitespace());
		struct select_keyword : public parser<decltype(select_keyword_p)> { select_keyword() : parser(select_keyword_p){} };
		inline const auto commexpr_p = (character(',') << ws_p) >> expression();
		struct commexpr : public parser<decltype(commexpr_p)> { commexpr() : parser(commexpr_p){} };
		inline const auto manyexpr_p = zero_or_more(commexpr());
		struct manyexpr : public parser<decltype(manyexpr_p)> { manyexpr() : parser(manyexpr_p){} };
		inline const auto select_p = (expression() && zero_or_more(expression()))
			>>= [](auto&& res)
		{
			const auto& [first, rest] = res;
			std::vector<expression_base_ptr> columns(rest.size() + 1);
			std::copy(rest.begin(), rest.end(), columns.begin() + 1);
			columns[0] = first;
			return std::make_shared<query_ast::select_clause>(columns);
		};
	}

	struct select : parser<decltype(detail::select_p)>
	{
		select() : parser(detail::select_p) {}
	};
	                /*
	using namespace parsers;
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
