#pragma once

#include "parse_result.h"
#include "parser_base.h"

namespace minisql::parsers
{
	class string_literal final : public parser_base<std::string>
	{
		const std::string expected;
	public:
		string_literal(std::string expected) : expected(std::move(expected))
		{
		}

		[[nodiscard]] std::string to_string() const override { return expected; }
	protected:
		auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<std::string> override;
	};

	/* implementation */
	inline auto string_literal::parse(const std::string& input, unsigned& current_pos) const -> parse_result<std::string>
	{
		if(current_pos >= input.size() && !expected.empty())
		{
			return error_message(input, current_pos);
		}

		auto current_it = input.begin() + current_pos;
		const std::string_view input_view(input.c_str() + current_pos, expected.size());
		const std::string_view expected_view(expected);
		if (input_view == expected_view)
		{
			current_pos += expected.size();
			return ok(expected);
		}
		return failure<std::string>( "Expected " + expected + " but got " + std::string(input_view.begin(), input_view.end()));
	}
}
