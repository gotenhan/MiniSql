#pragma once
#include <string>
#include <initializer_list>
#include "parser_base.h"


namespace minisql::parsers
{
	using namespace std::literals;

	template<typename ...TParsers>
	class seq final : public parser_base<std::tuple<typename TParsers::value_type...>>
	{
		std::tuple<TParsers...> parsers;

		template <typename TLast>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TLast& last) const
			-> parse_result<std::tuple<typename TLast::value_type>>;

		template <typename TFirst, typename ...TRest>
		[[nodiscard]] auto fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
			-> parse_result<std::tuple<typename TFirst::value_type, typename TRest::value_type...>>;

	public:
		seq(TParsers... parsers) : parsers(std::make_tuple(std::move(parsers)...)){}
		seq(const std::tuple<TParsers...>& parsers) : parsers(parsers) {}

		[[nodiscard]] std::string to_string() const override;
		template <typename TRight>
		auto constexpr operator&&(const TRight& right);
	protected:
		auto parse(const std::string& input, unsigned& current_pos) const->typename seq::result_type override;
	};

	template <typename TLeft, typename TRight>
	auto constexpr operator&&(const TLeft& left, const TRight& right) { return seq(left, right); }

	/* implementation */
	template <typename ... TParsers>
	template <typename TLast>
	auto seq<TParsers...>::fold(const std::string& input, unsigned& current_pos, const TLast& last) const
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

	template <typename ... TParsers>
	template <typename TFirst, typename ... TRest>
	auto seq<TParsers...>::fold(const std::string& input, unsigned& current_pos, const TFirst& first, const TRest&... rest) const
		-> parse_result<std::tuple<typename TFirst::value_type, typename TRest::value_type...>>
	{
		using tuple_res = std::tuple<typename TFirst::value_type, typename TRest::value_type...>;
		unsigned new_pos = current_pos;
		auto result = first(input, new_pos);
		if (!result.success)
		{
			return failure<tuple_res>(result.error);
		}

		auto result_rest = fold(input, new_pos, rest...);
		if (!result_rest.success)
		{
			return failure<tuple_res>(result_rest.error);
		}

		current_pos = new_pos;
		return ok(std::tuple_cat(std::make_tuple(std::move(result.result)), result_rest.result));
	}

	template <typename ... TParsers>
	auto seq<TParsers...>::to_string() const
		-> std::string
	{
		auto string_func = [](auto&& ...args) { return parser_base_notemplate::to_string_rec(std::forward<decltype(args)>(args)...); };
		return std::apply(string_func, parsers);
	}


	template <typename ... TParsers>
	auto seq<TParsers...>::parse(const std::string& input, unsigned& current_pos) const
		-> typename seq::result_type
	{
		unsigned new_pos = current_pos;
		auto fold_func = [this, &input, &new_pos](auto&& ...args)
		{
			return this->fold(input, new_pos, std::forward<decltype(args)>(args)...);
		};
		auto result = std::apply(fold_func, parsers);
		if (!result.success)
		{
			return failure<typename seq::value_type>(result.error);
		}

		current_pos = new_pos;
		return result;
	}

	template <typename ... TParsers>
	template <typename TRight>
	constexpr auto seq<TParsers...>::operator &&(const TRight& right)
	{
		return seq<TParsers..., TRight>(std::tuple_cat(this->parsers, std::make_tuple(right)));
	}
}
