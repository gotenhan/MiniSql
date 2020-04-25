#pragma once
#include "parser_base.h"

namespace minisql::parser
{
	template <typename TParser, typename TVal = typename TParser::value_type>
	class zero_or_more final : public parser_base<std::vector<TVal>>
	{
		TParser _parser;
		auto parse(const std::string& input, unsigned& current_pos) const->parse_result<std::vector<TVal>> override;
	public:
		zero_or_more(const TParser& p);
		[[nodiscard]] std::string to_string() const override;
	};

	/* implementation */
	template <typename TParser, typename TVal>
	zero_or_more<TParser, TVal>::zero_or_more(const TParser& p) : _parser(p)
	{
	}

	template <typename TParser, typename TVal>
	std::string zero_or_more<TParser, TVal>::to_string() const
	{
		return "one or more " + _parser.to_string();
	}

	template <typename TParser, typename TVal>
	auto zero_or_more<TParser, TVal>::parse(const std::string& input, unsigned& current_pos) const -> parse_result<std::vector<TVal>>
	{
		const auto first_pr = _parser(input, current_pos);
		if (!first_pr.success) return failure<std::vector<TVal>>(first_pr.error);

		std::vector<TVal> result;
		result.push_back(first_pr.result);
		while (true)
		{
			auto pr = _parser(input, current_pos);
			if (!pr.success) break;
			result.push_back(pr.result);
		}

		return ok(result);
	}
}
