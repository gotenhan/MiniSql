#pragma once
#include <string>
#include <functional>
#include <algorithm>
#include "parse_result.h"


namespace minisql::parsers
{
	using namespace std::literals;

	class parser_base_notemplate
	{
	protected:
		parser_base_notemplate() = default;


		parser_base_notemplate(const parser_base_notemplate& other) = default;

		parser_base_notemplate(parser_base_notemplate&& other) noexcept = default;

		template<typename TFirst, typename ...TRest>
		[[nodiscard]] static std::string to_string_rec(const TFirst& first, const TRest& ...rest);
	public:
		[[nodiscard]] virtual std::string to_string() const = 0;
		virtual ~parser_base_notemplate() = default;
	};

	template <typename TVal>
	class parser_base : public parser_base_notemplate
	{
		virtual auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<TVal> = 0;
	public:
		using value_type = TVal;
		using result_type = parse_result<TVal>;

		auto operator()(const std::string& input, unsigned& current_pos) const -> parse_result<TVal>;
	protected:
		parser_base();
		parser_base(parser_base<TVal>&&) noexcept;
		parser_base(const parser_base<TVal>&);

		template <typename TError=TVal>
		auto error_message(const std::string& input, unsigned pos) const -> parse_result<TError>;
	};

	template <typename TVal>
	class just final : public parser_base<TVal>
	{
		const TVal val;
		virtual auto parse(const std::string& input, unsigned& current_pos) const-> typename just::result_type override;
	public:
		just(TVal val);
		[[nodiscard]] std::string to_string() const override;
	};

	template <typename TFrom, typename TConvertFunc>
	class convert final : public parser_base<std::invoke_result_t<TConvertFunc, typename TFrom::value_type>>
	{
		const TFrom from;
		const TConvertFunc func;
	public:
		[[nodiscard]] std::string to_string() const override { return from.to_string(); }
		convert(TFrom from, TConvertFunc func) : from(std::move(from)), func(func) {}
	private:
		auto parse(const std::string& input, unsigned& current_pos) const -> typename convert::result_type override
		{
			const auto pr = from(input, current_pos);
			return pr.apply(func);
		}
	};

	template <typename TFrom, typename TConvertFunc>
	convert<TFrom, TConvertFunc> operator>>=(const TFrom& p, TConvertFunc f) 
	{
		return convert<TFrom, TConvertFunc>(p, f);
	}

	/* implementation */
	template <typename TFirst, typename ...TRest>
	std::string parser_base_notemplate::to_string_rec(const TFirst& first, const TRest& ...rest)
	{
		if constexpr (sizeof...(rest) == 0)
			return first.to_string();
		else
			return first.to_string() + ", "s + parser_base_notemplate::to_string_rec(rest...);
	}


	template <typename TVal>
	auto parser_base<TVal>::operator()(const std::string& input, unsigned& current_pos) const -> parse_result<TVal>
	{
		return parse(input, current_pos);
	}

	template <typename TVal>
	parser_base<TVal>::parser_base() = default;

	template <typename TVal>
	parser_base<TVal>::parser_base(parser_base<TVal>&&) noexcept = default;

	template <typename TVal>
	parser_base<TVal>::parser_base(const parser_base<TVal>&) = default;


	template <typename TVal>
	template <typename TError>
	[[nodiscard]] auto parser_base<TVal>::error_message(const std::string& input, unsigned pos) const -> parse_result<TError>
	{
		const std::string msg = pos < input.size()
			? input.substr(pos, std::min(input.size() - pos, 10u))
			: "end of string";
		return failure<TError>("Pos "s + std::to_string(pos) + ": Expected "s + to_string() + " but got "s + msg);
	}

	template <typename TVal>
	just<TVal>::just(TVal val) : val(std::move(val))
	{
	}

	template <typename TVal>
	std::string just<TVal>::to_string() const
	{
		return ""s;
	}

	template <typename TVal>
	auto just<TVal>::parse(const std::string& input, unsigned& current_pos) const -> typename just::result_type
	{
		return ok(val);
	}
}

