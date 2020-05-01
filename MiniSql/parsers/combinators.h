#pragma once
#include "parser_base.h"

namespace minisql::parsers
{ 
	template <typename TParser>
	class parser : public parser_base<typename TParser::value_type>
	{
		const std::string name;
		const TParser underlying;
	public:
		parser(std::string name, TParser parser) : name(std::move(name)), underlying(std::move(parser)) {}
		parser(TParser parser) : name(), underlying(std::move(parser)) {}
		[[nodiscard]] std::string to_string() const override { return name.empty() ? underlying.to_string() : name; }
	private:
		auto parse(const std::string& input, unsigned& current_pos) const -> typename parser::result_type override { return underlying(input, current_pos); }
	};

	template <typename TSkip, typename TParser>
	auto constexpr operator>>(TSkip skipped, TParser parser)
	{
		return seq(std::move(skipped), std::move(parser)) >>= [](auto&& res) { const auto& [s, p] = res; return p; };
	}

	template <typename TParser, typename TSkip>
	auto constexpr operator<<(TParser parser, TSkip skipped)
	{
		return seq(std::move(parser), std::move(skipped)) >>= [](auto&& res) { const auto& [p, s] = res; return p; };
	}

	template <typename TLeft, typename TParser, typename TRight>
	auto constexpr between(TLeft left, TRight right, TParser parser) { return left >> parser << right; }

	template <typename TParser>
	auto constexpr parens(TParser parser) { return character('(') >> parser << character(')'); }

	template <typename TParser>
	auto constexpr brackets(TParser parser) { return character('[') >> parser << character(']'); }

	template <typename TParser>
	auto constexpr single_quotes(TParser parser) { return character('\'') >> parser << character('\''); }

	template <typename TParser>
	auto constexpr double_quotes(TParser parser) { return character('"') >> parser << character('"'); }

	struct unit {};
	template <typename TParser>
	class not_p : public parser_base<unit>
	{
		const TParser underlying;

		auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<unit> override
		{
			unsigned new_pos = current_pos;
			const auto pr = underlying(input, new_pos);
			if(pr.success)
			{
				return error_message<unit>(input, current_pos);
			}
			else
			{
				return ok(unit());
			}
		}
	public:
		not_p(TParser underlying) : underlying(std::move(underlying)) {}
		[[nodiscard]] std::string to_string() const override { return "something different than " + underlying.to_string(); }
	};

	/*
	template<typename TParser1, typename TParser2>
	class followed_by final : public parser<typename TParser1::value_type>
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

	/*implementation#1#

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
	*/

	/*
	template <typename TParser1, typename TParser2>
	followed_by<TParser1, TParser2> operator,(const TParser1& parser1, const TParser2& parser2)
	{
		return followed_by<TParser1, TParser2>(parser1, parser2);
	}
*/
}
