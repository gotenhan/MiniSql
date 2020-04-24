#pragma once
#include "parser_base.h"

namespace minisql::parser
{
	template <typename TParser>
	class optional final : public parser_base<typename TParser::value_type>
	{
		TParser _parser;
		typename TParser::value_type _default_val;
	public:
		optional(const TParser& p, const typename TParser::value_type& default_val) : _parser(p),
		                                                                              _default_val(default_val)
		{
		};

		[[nodiscard]] std::string to_string() const override
		{
			return "optional " + _parser.to_string();
		}

	protected:
		parse_result<typename TParser::value_type> parse(const std::string& input, unsigned& current_pos) const
		override;
	};

	/* implementation */
	template <typename TParser>
	auto optional<TParser>::parse(const std::string& input, unsigned& current_pos) const
	-> parse_result<typename TParser::value_type>
	{
		unsigned new_pos = current_pos;
		auto pr = _parser(input, new_pos);
		if (pr.success)
		{
			current_pos = new_pos;
			return pr;
		}
		return ok(_default_val);
	}
}
