#pragma once
#include "parse_result.h"
#include "parser_base.h"
#include "optional.h"
#include "character.h"
#include "choice.h"
#include "string_literal.h"

namespace minisql::parser
{
	class number : public parser_base<double>
	{
		parse_result<double> parse(const std::string& input, unsigned& current_pos) const override
		{
			const auto func = [](char plus_or_minus, const std::vector<char>& int_part, const std::vector<char>& float_part)
			{
				std::string str({ plus_or_minus });
				str.append(int_part.begin(), int_part.end());
				str.append(".");
				str.append(float_part.begin(), float_part.end());
				return str;
			};

			const auto number_parser = make_combine<std::string>
				(
					func,
					optional(choice<char, character, character>(character('-'), character('+')), '+'),
					one_or_more(digit()),
					optional((character('.'), one_or_more(digit())), { '0' })
					);

			/*
			const auto pr = make_choice<std::string>(
				string_literal("Inf"s),
				string_literal("-Inf"s),
				string_literal("NaN"s),
				number_parser
				)(input, current_pos);
				*/
			const auto pr = number_parser(input, current_pos);

			return pr.success
				? ok(std::stod(pr.result))
				: failure<double>(pr.error);
		}

		[[nodiscard]] std::string to_string() const override
		{
			return "number";
		}
	public:

		number() = default;

		number(number&& parser_base) = default;

		number(const number& parser_base) = default;
	};
}
