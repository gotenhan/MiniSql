#pragma once

#include <numeric>

#include "../parsers.h"
#include "../query_ast.h"
#include "../query_ast/values.h"

namespace minisql::query_parser
{
	using namespace parsers;

	namespace detail
	{
		inline const auto quoted_string_p =
			single_quotes(zero_or_more(character('\\') >> character('\'') || not_char('\''))) >>= [](const std::vector<char>& v)
		{
			return std::make_shared<query_ast::string>(std::string(v.begin(), v.end()));
		};
		inline const auto boolean_p = string_literal("true") || string_literal("false") >>= [](const std::string& s)
		{
			return std::make_shared<query_ast::boolean>(s == "true"s);
		};
		inline const auto null_p = (string_literal("null") >>= [](const std::string& s) { return std::make_shared<query_ast::null>(); });
		inline const auto number_p = (number() >>= [](const double d) { return std::make_shared<query_ast::number>(d); });

		inline const auto value_p = number_p || quoted_string_p || null_p || boolean_p || cast<query_ast::expression_base_ptr>();
		
	}
	struct value : public parser<decltype(detail::value_p)>
	{
		value() : parser("number, boolean, string or null", detail::value_p) {}
	};

	namespace detail{
		inline const auto identifier_p =
			(character('_') || alpha()) &&
			zero_or_more(alphanum() || character('_'))
			>>= [](auto res)
		{
			const auto& [f, r] = res;
			std::string s(1, f);
			s.reserve(r.size() + 1);
			std::copy(r.begin(), r.end(), std::back_inserter(s));
			return std::make_shared<query_ast::identifier>(s);
		};
	}

	struct identifier : public parser<decltype(detail::identifier_p)>
	{
		identifier() : parser("identifier", detail::identifier_p) {}
	};

	/*
	 *  expression ::= term ('+' term)*
	 *  term ::= factor ('*' factor)*
	 *  factor = value() || identifier || between '(' ')' expression
	 */
	struct expression : public parser_base<query_ast::expression_base_ptr>
	{
		[[nodiscard]] std::string to_string() const override { return "arithmetic expression"; }
	private:
		auto parse(const std::string& input, unsigned& current_pos) const
			-> result_type
		override;
	};

	namespace detail
	{
		inline auto addition_operator()
		{
			return parser(
				(character('-') >>= [](const char c) { return query_ast::binary_arith_op::sub; }) ||
				(character('+') >>= [](const char c) { return query_ast::binary_arith_op::add; }));
		}

		inline auto mul_operator()
		{
			return parser(
				(character('*') >>= [](const char c) { return query_ast::binary_arith_op::mul; }) ||
				(character('/') >>= [](const char c) { return query_ast::binary_arith_op::div; }) ||
				(character('%') >>= [](const char c) { return query_ast::binary_arith_op::mod; }));
		}

		inline auto factor()
		{
			return make_choice<query_ast::expression_base_ptr>(
				value(),
				identifier(),
				between(character('('), character(')'), expression()));
		}
		inline auto term()
		{
			return factor() && zero_or_more(mul_operator() && factor()) >>= [](auto&& res)
			{
				const auto& [e1, rest] = res;
				if (rest.empty()) return e1;
				return std::accumulate(rest.begin(), rest.end(), e1, [](auto&& left, auto&& next)
					{
						const auto& [op, right] = next;
						return std::make_shared<query_ast::binary_op_expression>(left, op, right);
					});
			};
		}
	}

	inline auto expression::parse(const std::string& input, unsigned& current_pos) const
		-> result_type
	{
		using namespace detail;
		const auto parser = term() && zero_or_more(addition_operator() && term()) >>= [](auto&& res)
		{
			const auto& [e1, rest] = res;
			if (rest.empty()) return e1;
			return std::accumulate(rest.begin(), rest.end(), e1, [](auto&& left, auto&& next)
				{
					const auto& [op, right] = next;
					return std::make_shared<query_ast::binary_op_expression>(left, op, right);
				});
		};
		return parser(input, current_pos);
	}
}
