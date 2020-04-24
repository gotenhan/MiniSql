#pragma once
#include <utility>


#include "parser_base.h"


namespace minisql::parser
{
	template<typename TParser1, typename TParser2>
	class followed_by final : public parser_base<typename TParser2::value_type>
	{
		const TParser1 parser1;
		const TParser2 parser2;
	public:
		followed_by(TParser1 parser1, TParser2 parser2);
		[[nodiscard]] std::string to_string() const override;
	protected:
		parse_result<typename TParser2::value_type> parse(const std::string& input, unsigned& current_pos) const override;
	};

	template <typename TParser1, typename TParser2>
	followed_by<TParser1, TParser2> operator,(const TParser1& parser1, const TParser2& parser2);

	/*implementation*/

	template <typename TParser1, typename TParser2>
	followed_by<TParser1, TParser2>::
	followed_by(TParser1 parser1, TParser2 parser2): parser1(std::move(parser1)), parser2(std::move(parser2))
	{
	}

	template <typename TParser1, typename TParser2>
	auto followed_by<TParser1, TParser2>::to_string() const -> std::string
	{
		return parser1.to_string() + " followed by "s + parser2.to_string();
	}

	template <typename TParser1, typename TParser2>
	auto followed_by<TParser1, TParser2>::parse(const std::string& input, unsigned& current_pos) const
		-> parse_result<typename TParser2::value_type>
	{
		unsigned new_pos = current_pos;
		auto left_result = parser1(input, new_pos);
		if (!left_result.success)
			return this->error_message(input, current_pos);

		current_pos = new_pos;
		auto right_result = parser2(input, new_pos);
		if (!right_result.success)
			return this->error_message(input, current_pos);

		return right_result;
	}

	template <typename TParser1, typename TParser2>
	followed_by<TParser1, TParser2> operator,(const TParser1& parser1, const TParser2& parser2)
	{
		return followed_by<TParser1, TParser2>(parser1, parser2);
	}
}
