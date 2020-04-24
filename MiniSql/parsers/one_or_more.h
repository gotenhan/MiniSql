#pragma once
#include "parser_base.h"

namespace minisql::parser
{
	template <typename TParser, typename TVal = typename TParser::value_type>
	class one_or_more final : public parser_base<std::vector<TVal>>
	{
		TParser _parser;
	public:
		one_or_more(const TParser& p);;
		[[nodiscard]] std::string to_string() const override;
	protected:
		auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<std::vector<TVal>> override;
	};

	/* implementation */
	template <typename TParser, typename TVal>
	one_or_more<TParser, TVal>::one_or_more(const TParser& p): _parser(p)
	{
	}

	template <typename TParser, typename TVal>
	std::string one_or_more<TParser, TVal>::to_string() const
	{
		return "one or more " + _parser.to_string();
	}

	template <typename TParser, typename TVal>
	auto one_or_more<TParser, TVal>::parse(const std::string& input,
	                                       unsigned& current_pos) const -> parse_result<std::vector<TVal>>
	{
		parse_result<TVal> pr = _parser(input, current_pos);
		if (!pr.success) return failure<std::vector<TVal>>(pr.error);

		std::vector<TVal> result;
		result.push_back(pr.result);
		while (true)
		{
			auto pr = _parser(input, current_pos);
			if (!pr.success) break;
			result.push_back(pr.result);
		}

		return ok(result);
	}
}
