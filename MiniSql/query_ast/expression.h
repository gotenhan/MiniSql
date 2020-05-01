#pragma once
#include <string>
#include <utility>
#include "ast_base.h"

namespace minisql::query_ast
{
	struct arith_expression_base
	{
		virtual std::string to_string() const = 0;
	protected:
		arith_expression_base() = default;
		arith_expression_base(const arith_expression_base&) = default;
		arith_expression_base(arith_expression_base&&) = default;
	};

	inline std::ostream& operator<<(std::ostream& out, const arith_expression_base& aeb)
	{
		out << aeb.to_string();
		return out;
	}

	using expression_base_ptr = std::shared_ptr<arith_expression_base>;

	struct identifier : public ast_base, public arith_expression_base
	{
		const std::string name;
		identifier() = default;
		identifier(std::string identifier) :  name(std::move(identifier)) {}

		std::string to_string() const override { return "identifier(" + name + ")"; }

		friend bool operator==(const identifier& left, const identifier& right) { return left.name == right.name; }
	};

	using identifier_ptr = std::shared_ptr<identifier>;

	enum class binary_arith_op : char {
		add = '+',
		sub = '-',
		mul = '*',
		div = '/',
		mod = '%'
	};

	struct binary_op_expression : public ast_base, public arith_expression_base
	{
		const std::shared_ptr<arith_expression_base> left;
		const std::shared_ptr<arith_expression_base> right;
		const binary_arith_op op;
		binary_op_expression(std::shared_ptr<arith_expression_base> left,
			binary_arith_op op,
			std::shared_ptr<arith_expression_base> right) : left(std::move(left)), right(std::move(right)), op(op){}
		std::string to_string() const override
		{
			return left->to_string() + static_cast<char>(op) + right->to_string();
		}
	};
}
