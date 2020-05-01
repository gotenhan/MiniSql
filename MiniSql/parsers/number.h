#pragma once
#include "parse_result.h"
#include "parser_base.h"
#include "optional.h"
#include "character.h"
#include "choice.h"
#include "string_literal.h"
#include "combinators.h"

namespace minisql::parsers
{
	class number : public parser_base<double>
	{
		parse_result<double> parse(const std::string& input, unsigned& current_pos) const override
		{
			const auto func = [](auto&& res)
			{
				const auto& [plus_or_minus, int_part, float_part] = res;
				std::string str({ plus_or_minus });
				str.append(int_part.begin(), int_part.end());
				str.append(".");
				str.append(float_part.begin(), float_part.end());
				return str;
			};

			const auto number_parser = seq(
					optional(character('-') || character('+'), '+'),
					one_or_more(digit()),
					optional(character('.') >> one_or_more(digit()), { '0' })
					) >>= func;

			const auto parser = (
				string_literal("Inf"s) ||
				string_literal("-Inf"s) ||
				string_literal("NaN"s) ||
				number_parser
				);
			const auto pr = parser(input, current_pos);

			return pr.success
				? ok(std::stod(pr.result))
				: failure<double>(pr.error);
		}

	public:

		constexpr number() = default;

		constexpr number(number&& parser_base) = default;

		constexpr number(const number& parser_base) = default;

		[[nodiscard]] std::string to_string() const override
		{
			return "number";
		}
	};
}
