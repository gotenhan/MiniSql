#pragma once
#include <string>


namespace minisql::parsers
{
	using namespace std::literals;

	template<typename T> struct parse_result;
	template<typename T> parse_result<T> ok(const T& val);
	template<typename U> parse_result<std::unique_ptr<U>> ok(std::unique_ptr<U> val);
	template<typename T> parse_result<T> failure(const std::string& error);

	template<typename T>
	struct parse_result final
	{
		friend parse_result<T> ok<T>(const T& val);
		template <typename U>
		friend parse_result<std::unique_ptr<U>> ok(std::unique_ptr<U> val);
		friend parse_result failure<T>(const std::string& error);

		const bool success;
		const T result;
		const std::string error;

		parse_result() : success(false), result(), error("default parse result"s) {}
		parse_result(const parse_result&) = default;
		parse_result(parse_result&&) noexcept = default;

		template <typename TFunc>
		parse_result<std::invoke_result_t<TFunc, T>> apply(TFunc func) const;

		template <typename TOther, typename TDummy = std::enable_if_t<std::is_convertible_v<T, TOther>, parse_result<TOther>>>
		operator parse_result<TOther>()
		{
			if (!this->success)
			{
				return failure<TOther>(this->error);
			}

			return ok<TOther>(result);
		}

	private:
		parse_result(bool success, T result, std::string error) : success(success), result(std::move(result)), error(std::move(error))
		{
		};
	};

	template<typename T>
	parse_result<T> ok(const T& val) { return parse_result<T>(true, std::move(val), std::string()); }
	template<typename U>
	parse_result<std::unique_ptr<U>> ok(std::unique_ptr<U> val) { return parse_result<std::unique_ptr<U>>(true, std::move(val), std::string()); }
	template<typename T>
	parse_result<T> failure(const std::string& error)
	{
		if(error.empty())
		{
			return parse_result<T>(false, T(), "mam cie lobuzie");
		}
		return parse_result<T>(false, T(), error);
	}

	template <typename T>
	template <typename TFunc>
	[[nodiscard]] parse_result<std::invoke_result_t<TFunc, T>> parse_result<T>::apply(TFunc func) const
	{
		if (success)
		{
			return ok(func(result));
		}
		else
		{
			return failure<std::invoke_result_t<TFunc, T>>(error);
		}
	}
}
