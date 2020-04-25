#pragma once
#include <string>

using namespace std::literals;

namespace minisql::parser
{
		template<typename T> struct parse_result;
		template<typename T> parse_result<T> ok(const T& val);
		template<typename T> parse_result<T> failure(const std::string& error);

		template<typename T>
		struct parse_result final
		{
			friend parse_result ok<T>(const T& val);
			friend parse_result failure<T>(const std::string& error);

			const bool success;
			const T result;
			const std::string error;

			parse_result() : success(false), result(), error("default parse result"s) {}
			parse_result(const parse_result&) = default;
			parse_result(parse_result&&) noexcept = default;

			template <typename TFunc, typename U=decltype(std::declval<TFunc>()(std::declval<T>()))>
			parse_result<U> apply(TFunc func) const;
		private:
			parse_result(bool success, T result, std::string error) : success(success), result(std::move(result)), error(std::move(error)) {};
		};

		template<typename T>
		parse_result<T> ok(const T& val) { return parse_result<T>(true, val, std::string()); }
		template<typename T>
		parse_result<T> failure(const std::string& error) { return parse_result<T>(false, T(), error); }

		template <typename T>
		template <typename TFunc, typename U>
		parse_result<U> parse_result<T>::apply(TFunc func) const
		{
			if(success)
			{
				return ok(func(result));
			}
			else
			{
				return failure<U>(error);
			}
		}

}
