#pragma once
#include <string>
#include <initializer_list>
#include "parser_base.h"

using namespace std::literals;

namespace minisql::parser
{
	template<typename TVal, typename TCombineFunc, typename ...TParsers>
	class combine final : public parser_base<TVal>
	{
		std::tuple<TParsers...> parsers;
		TCombineFunc combine_func;

		template <typename TLast>
		[[nodiscard]] static auto to_string_impl(const TLast& last) -> std::string;

		template <typename TFirst, typename ...TRest>
		[[nodiscard]] static auto to_string_impl(const TFirst& first, const TRest&... rest) -> std::string;

		template <typename TLast>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TLast& last) const
			-> parse_result<std::tuple<typename TLast::value_type>>;

		template <typename TFirst, typename ...TRest>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
			-> parse_result<std::tuple<typename TFirst::value_type, typename TRest::value_type...>>;

	public:
		combine(TParsers... parsers, TCombineFunc combine_func) : parsers(std::make_tuple(std::move(parsers)...)), combine_func(std::move(combine_func)){}

		[[nodiscard]] std::string to_string() const override;
	protected:
		parse_result<TVal> parse(const std::string& input, unsigned& current_pos) const override;;
	};

	template<typename TVal, typename TCombineFunc, typename ...TParsers>
	auto make_combine(TCombineFunc combine_func, TParsers&&... parsers)->combine<TVal, TCombineFunc, TParsers...>;

	/* implementation */
	template <typename TVal, typename TCombineFunc, typename ... TParsers>
	template <typename TLast>
	auto combine<TVal, TCombineFunc, TParsers...>::fold(const std::string& input, unsigned& current_pos, const TLast& last) const
		-> parse_result<std::tuple<typename TLast::value_type>>
	{
		using tuple_res = std::tuple<typename TLast::value_type>;
		unsigned new_pos = current_pos;
		auto result = last(input, new_pos);
		if (!result.success)
		{
			return this->template error_message<tuple_res>(input, current_pos);
		}

		current_pos = new_pos;
		return ok<tuple_res>(result.result);
	}

	template <typename TVal, typename TCombineFunc, typename ... TParsers>
	template <typename TFirst, typename ... TRest>
	auto combine<TVal, TCombineFunc, TParsers...>::fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
		-> parse_result<std::tuple<typename TFirst::value_type, typename TRest::value_type...>>
	{
		using tuple_res = std::tuple<typename TFirst::value_type, typename TRest::value_type...>;
		unsigned new_pos = current_pos;
		auto result = first(input, new_pos);
		if (!result.success)
		{
			return failure<tuple_res>(result.error);
		}

		current_pos = new_pos;
		auto result_rest = fold(input, new_pos, rest...);
		if (!result_rest.success)
		{
			return failure<tuple_res>(result.error);
		}

		current_pos = new_pos;
		return ok(std::tuple_cat(std::make_tuple(result.result), result_rest.result));
	}

	template <typename TVal, typename TCombineFunc, typename ... TParsers>
	auto combine<TVal, TCombineFunc, TParsers...>::to_string() const
		-> std::string
	{
		auto string_func = [this](auto&& ...args) { return parser_base_notemplate::to_string_rec(std::forward<decltype(args)>(args)...); };
		return std::apply(string_func, parsers);
	}

	template< typename TVal, typename TCombineFunc, typename ...TParsers >
	inline void verify_pod()
	{
		static_assert(std::is_same_v<TVal, decltype(std::declval<TCombineFunc>()(std::declval<typename TParsers::value_type>()...))>, "T is not a pod");
	}


	template <typename TVal, typename TCombineFunc, typename ... TParsers>
	auto combine<TVal, TCombineFunc, TParsers...>::parse(const std::string& input, unsigned& current_pos) const
		-> parse_result<TVal>
	{
		unsigned new_pos = current_pos;
		auto fold_func = [this, &input, &new_pos](auto&& ...args)
		{
			return this->fold(input, new_pos, std::forward<decltype(args)>(args)...);
		};
		auto result = std::apply(fold_func, parsers);
		if (!result.success)
		{
			current_pos = new_pos;
			return failure<TVal>(result.error);
		}

		const auto res = std::apply(combine_func, result.result);
		return ok(res);
	}

	template <typename TVal, typename TCombineFunc, typename ...TParsers>
	auto make_combine(TCombineFunc combine_func, TParsers&&... parsers)
		-> combine<TVal, TCombineFunc, TParsers...>
	{
		return combine<TVal, TCombineFunc, TParsers...>(std::forward<TParsers>(parsers)..., std::forward<TCombineFunc>(combine_func));
	}
}
