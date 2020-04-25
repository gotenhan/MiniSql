#pragma once
#include <variant>
#include "parser_base.h"

namespace minisql::parser
{
	template <typename TVal, typename ...TParsers>
	class choice final : public parser_base<TVal>
	{
		const std::tuple<TParsers...> parsers;

		template <typename TLast>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TLast& last) const
			->parse_result<TVal>;

		template <typename TFirst, typename ...TRest>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
			->parse_result<TVal>;

		auto parse(const std::string& input, unsigned& current_pos) const->parse_result<TVal> override;
	public:
		choice(TParsers ... parsers);

		[[nodiscard]] std::string to_string() const override;
	};

	/* implementation */
	template <typename TVal, typename ... TParsers>
	choice<TVal, TParsers...>::choice(TParsers ... parsers): parsers(std::make_tuple(std::move(parsers)...))
	{
	}

	template <typename TVal, typename ... TParsers>
	[[nodiscard]] std::string choice<TVal, TParsers...>::to_string() const
	{
		auto to_string_func = [this](auto&& ...args)
		{
			return parser_base_notemplate::to_string_rec(std::forward<decltype(args)>(args)...);
		};
		return "choice of [ "s + std::apply(to_string_func, parsers) + "]"s;
	}

	/* implementation */
	template <typename TVal,typename ... TParsers>
	template <typename TLast>
	auto choice<TVal, TParsers...>::fold(const std::string& input, unsigned& current_pos, const TLast& last) const
		-> parse_result<TVal>
	{
		unsigned new_pos = current_pos;
		auto result = last(input, new_pos);
		if (!result.success)
		{
			return this->template error_message<TVal>(input, current_pos);
		}

		current_pos = new_pos;
		return ok(result.result);
	}

	template <typename TVal, typename ... TParsers>
	template <typename TFirst, typename ... TRest>
	auto choice<TVal, TParsers...>::fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
		->parse_result<TVal>
	{
		unsigned new_pos = current_pos;
		auto result = first(input, new_pos);
		if (result.success)
		{
			current_pos = new_pos;
			return ok( result.result );
		}

		new_pos = current_pos;
		auto result_rest = fold(input, new_pos, rest...);
		if (result_rest.success)
		{
			current_pos = new_pos;
			return ok(result_rest.result);
		}

		return this->template error_message<TVal>(input, current_pos);
	}

	template <typename TVal, typename ... TParsers>
	auto choice<TVal, TParsers...>::parse(const std::string& input, unsigned& current_pos) const -> parse_result<TVal>
	{
		auto fold_func = [this, &input, &current_pos](auto&& ...args)
		{
			return this->fold(input, current_pos, std::forward<decltype(args)>(args)...);
		};
		return std::apply(fold_func, parsers);
	}

	template <typename TVal, typename ...TParsers>
	auto make_choice(TParsers&&... parsers)
		-> choice<TVal, TParsers...>
	{
		return choice<TVal, TParsers...>(std::forward<TParsers>(parsers)...);
	}

}
