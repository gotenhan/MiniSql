#include "zero_or_more.h"

namespace minisql::parser
{
	template <typename TVal>
	zero_or_more<TVal>::zero_or_more(const parser_base<TVal>& p): _parser(p)
	{
	}

	template <typename TVal>
	std::string zero_or_more<TVal>::to_string() const
	{
		return "zero or more " + _parser.to_string();
	}

	template <typename TVal>
	auto zero_or_more<TVal>::parse(const std::string& input, unsigned& current_pos) const -> parse_result<TVal>
	{
		parse_result<TVal> pr;
		std::vector<TVal> result;
		unsigned new_pos = current_pos;
		while ((pr = _parser(input, new_pos))._success)
		{
			current_pos = new_pos;
			result.push(pr);
		}

		return success(result);
	}
}
