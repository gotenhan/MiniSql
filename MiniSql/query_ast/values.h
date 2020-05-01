#pragma once
#include <string>
#include <ostream>

namespace minisql::query_ast
{
	template<typename T>
	struct value_base : public ast_base, public arith_expression_base
	{
		const T value;
		value_base() : value() {}
		value_base(T value) : value(std::move(value)) {}

		std::string to_string() const override
		{
			return std::to_string(value);
		}

		friend bool operator==(const value_base& lhs, const value_base& rhs)
		{
			return lhs.value == rhs.value;
		}

		friend bool operator!=(const value_base& lhs, const value_base& rhs)
		{
			return !(lhs == rhs);
		}
		friend std::ostream& operator<<(std::ostream& out, const value_base& vb)
		{
			out << vb.value;
			return out;
		}
	};


	struct nulltype {};
	template<> inline std::string value_base<nulltype>::to_string() const { return "null"; }

	struct null final : public value_base<nulltype>
	{
		std::string to_string() const override { return "null"; }
	};

	struct number final : public value_base<double>
	{
		number() = default;
		number(const double value) :  value_base(value) {}
	};

	template<> inline std::string value_base<std::string>::to_string() const { return value; }
	struct string final : public value_base<std::string>
	{
		string() = default;
		string(std::string s) : value_base(std::move(s)) {}
		string(std::string&& s) : value_base(std::move(s)) {}
	};

	struct boolean final : public value_base<bool>
	{
		boolean() = default;
		boolean(const bool value) : value_base(value) {}
	};
}
