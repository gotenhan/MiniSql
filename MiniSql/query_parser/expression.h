#pragma once
#include "../parsers.h"
#include "../query_ast.h"

namespace minisql::query_parser
{
	using namespace parser;

	class identifier : public parser_base<query_ast::column_identifier>
	{
	public:
		[[nodiscard]] std::string to_string() const override { return "identifier"; }
	private:
		auto parse(const std::string& input,
		           unsigned& current_pos) const -> parse_result<query_ast::column_identifier> override;
	};

	/* implementation */
	inline auto identifier::parse(const std::string& input,
	                              unsigned& current_pos) const -> parse_result<query_ast::column_identifier>
	{
		const auto first_char = make_choice<char>(character('_'), alpha());
		const auto rest = zero_or_more(make_choice<char>(alphanum(), character('_')));

		const auto comb = make_combine<std::string>([](char f, const std::vector<char>& r)
			{
				std::string s(1, f);
				s.reserve(r.size() + 1);
				std::copy(r.begin(), r.end(), std::back_inserter(s));
				return s;
			}, first_char, rest);

		const auto pr = comb(input, current_pos);
		return pr.apply([](const std::string& s) { return query_ast::column_identifier(s); });
	}

	/*
	class expression : parser_base<query_ast::expression>
	{
	public:
		[[nodiscard]] std::string to_string() const override;
	private:
		auto parse(const std::string& input,
			unsigned& current_pos) const -> parse_result<query_ast::expression> override;
	};

	inline std::string expression::to_string() const
	{
		return "expression";
	}

	inline auto expression::parse(const std::string& input,
		unsigned& current_pos) const -> parse_result<query_ast::expression>
	{
	}
*/
}
