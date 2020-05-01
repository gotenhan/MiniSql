#pragma once
#include <string>
#include <ostream>
#include "ast_base.h"
#include "expression.h"

namespace minisql::query_ast
{
	template<typename T>
	struct value_base : public arith_expression_base
	{
		const T value;
		value_base(T value, kind_t kind) : arith_expression_base(kind), value(std::move(value)) {}

		std::string to_string() const override
		{
			return std::to_string(value);
		}

		bool is_equal(const ast_base& other) const override
		{
			if (other.kind != this->kind) return false;
			const auto other_derived = dynamic_cast<const value_base<T>&>(other);
			return *this == other_derived;
		}

		bool operator==(const value_base& rhs) const
		{
			return value == rhs.value;
		}

		friend std::ostream& operator<<(std::ostream& out, const value_base& vb)
		{
			out << vb.value;
			return out;
		}
	};


	struct nulltype {};
	template<> inline std::string value_base<nulltype>::to_string() const { return "null"; }
	template<> inline bool value_base<nulltype>::operator==(const value_base<nulltype>& other) const { return true; }

	struct null final : public value_base<nulltype>
	{
		null() : value_base(nulltype(), kind_t::null) {}
		std::string to_string() const override { return "null"; }
	};

	struct number final : public value_base<double>
	{
		number(const double value) :  value_base(value, kind_t::number) {}
	};

	template<> inline std::string value_base<std::string>::to_string() const { return value; }
	struct string final : public value_base<std::string>
	{
		string(std::string s) : value_base(std::move(s), kind_t::string) {}
	};

	struct boolean final : public value_base<bool>
	{
		boolean(const bool value) : value_base(value, kind_t::boolean) {}
	};
}
