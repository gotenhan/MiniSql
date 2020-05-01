#pragma once
#include <utility>
#include <variant>
#include "parser_base.h"

namespace minisql::parsers
{
	template <typename T>
	struct cast { };

	namespace detail {
		template<typename T, typename... Rest>
		struct are_same : std::false_type {};

		template<typename T, typename First>
		struct are_same<T, First> : std::is_same<T, First> {};

		template<typename T, typename First, typename... Rest>
		struct are_same<T, First, Rest...>
			: std::integral_constant<bool, std::is_same<T, First>::value&& are_same<T, Rest...>::value>
		{};

		template <typename TFirst, typename ...TParsers>
		struct choice_result_helper
		{
			using are_same_values = are_same<typename TFirst::value_type, typename TFirst::value_type, typename TParsers::value_type...>;
			using variant_type = std::variant<typename TFirst::value_type, typename TParsers::value_type...>;
			using common_type = typename std::conditional<are_same_values::value, typename TFirst::value_type, variant_type>::type;
		};


		template<typename T, typename... Rest>
		struct are_convertible : std::false_type
		{
		};

		template<typename T, typename First>
		struct are_convertible<T, First> : std::is_convertible<First, T>
		{
		};

		template<typename TTo, typename First, typename... Rest>
		struct are_convertible<TTo, First, Rest...>
			: std::integral_constant<bool, std::is_convertible<First, TTo>::value> // && are_convertible<TTo, Rest...>::value>
		{
		};
	}
	
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
		choice(std::tuple<TParsers...> parsers);

		[[nodiscard]] std::string to_string() const override;

		template <typename TRight>
		auto operator||(const TRight& right);

		template <class TCast>
		auto operator||(const cast<TCast>& cast);
	};


	/* implementation */
	template <typename TVal, typename ... TParsers>
	choice<TVal, TParsers...>::choice(TParsers ... parsers): parsers(std::make_tuple(std::move(parsers)...))
	{
		/*
		static_assert(detail::are_convertible<
			std::remove_reference<TVal>,
			std::remove_reference<typename TParsers::value_type>...>::value,
			"result types of parsers should all be convertible to TVal");
	*/
	}

	template <typename TVal, typename ... TParsers>
	choice<TVal, TParsers...>::choice(std::tuple<TParsers...> parsers) : parsers(std::move(parsers))
	{
	}

	template <typename TVal, typename ... TParsers>
	[[nodiscard]] std::string choice<TVal, TParsers...>::to_string() const
	{
		auto to_string_func = [](auto&& ...args)
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
		// todo: this does not work if TVal is a variant with duplicate types, check if 
		unsigned new_pos = current_pos;
		auto result = first(input, new_pos);
		if (result.success)
		{
			current_pos = new_pos;
			return ok(result.result);
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

	template <typename TFirst, typename ...TRest>
	auto make_choice(const TFirst& first, const TRest&... rest)
	{
		return choice<
			typename detail::choice_result_helper<
				std::remove_reference_t<TFirst>,
				std::remove_reference_t<TRest>...
			>::common_type,
			std::remove_reference_t<TFirst>,
			std::remove_reference_t<TRest>...>
		(first, rest...);
	}

	template <typename TOverride, typename ...TParsers>
	auto make_choice(const TParsers& ...parsers)
	{
		return choice<TOverride, std::remove_reference_t<TParsers>...>(parsers...);
	}

	template <typename ...TParsers>
	auto make_choice(const std::tuple<TParsers...>& parsers)
	{
		return choice<typename detail::choice_result_helper<std::remove_reference_t<TParsers>...>::common_type, std::remove_reference_t<TParsers>... >(parsers);
	}

	template <typename TOverride, typename ...TParsers>
	auto make_choice(const std::tuple<TParsers>& ...parsers)
	{
		return choice<TOverride, std::remove_reference_t<TParsers>...>(parsers...);
	}

	template <typename TVal, typename ... TParsers>
	template <typename TRight>
	auto choice<TVal, TParsers...>::operator||(const TRight& right)
	{
		return make_choice(std::tuple_cat(this->parsers, std::make_tuple(right)));
	}

	template <typename TVal, typename ... TParsers>
	template <typename TCast>
	auto choice<TVal, TParsers...>::operator||(const cast<TCast>& cast)
	{
		/*
		static_assert(detail::are_convertible<
				std::remove_reference<TCast>,
				std::remove_reference<typename TParsers::value_type>...
			>::value,
			"value types of parsers should all be convertible to TCast");
		*/
		return choice<TCast, TParsers...>(this->parsers);
	}

	template <typename TLeft, typename TRight>
	auto operator||(const TLeft& left, const TRight& right) { return make_choice(left, right); }
}
