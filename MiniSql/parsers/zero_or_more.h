#pragma once
#include "parser_base.h"

namespace minisql::parser
{
	template <typename TVal>
	class zero_or_more final : public parser_base<std::vector<TVal>>
	{
		parser_base<TVal> _parser;
	public:
		zero_or_more(const parser_base<TVal>& p);;
		[[nodiscard]] std::string to_string() const override;
	protected:
		auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<TVal> override;
	};

	
}
