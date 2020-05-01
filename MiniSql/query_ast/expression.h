#pragma once
#include <string>
#include <utility>
#include "ast_base.h"

namespace minisql::query_ast
{
	struct arith_expression_base : public ast_base
	{
		bool operator==(const arith_expression_base& other) const { return this->is_equal(other); }
	protected:
		arith_expression_base() = default;
		arith_expression_base(const kind_t kind) : ast_base(kind) {}
		arith_expression_base(const arith_expression_base&) = default;
		arith_expression_base(arith_expression_base&&) = default;
	};

	inline std::ostream& operator<<(std::ostream& out, const arith_expression_base& aeb)
	{
		out << aeb.to_string();
		return out;
	}

	using expression_base_ptr = std::shared_ptr<arith_expression_base>;

	struct identifier : public arith_expression_base
	{
		const std::string name;
		identifier() : arith_expression_base(kind_t::identifier) {}
		identifier(std::string identifier) : arith_expression_base(kind_t::identifier), name(std::move(identifier)) {}
		identifier(const identifier& other) : arith_expression_base(kind_t::identifier), name(other.name) {}

		std::string to_string() const override { return  name; }

		bool is_equal(const ast_base& other) const override { return other.kind == kind && *this == dynamic_cast<const identifier&>(other); } 
		bool operator==(const identifier& right) const { return name == right.name; }
	};

	using identifier_ptr = std::shared_ptr<identifier>;

	enum class binary_op : char {
		add = '+',
		sub = '-',
		mul = '*',
		div = '/',
		mod = '%',
		equal = '=',
		less = '<',
		less_equal = 243,
		greater = '>',
		greater_equal = 242,
		not_equal = '~'
	};

	struct binary_op_expression : public arith_expression_base
	{
		const std::shared_ptr<arith_expression_base> left;
		const std::shared_ptr<arith_expression_base> right;
		const binary_op op;

		binary_op_expression(std::shared_ptr<arith_expression_base> left, binary_op op, std::shared_ptr<arith_expression_base> right)
			: arith_expression_base(kind_t::binary_aop), left(move(left)), right(std::move(right)), op(op){}
		binary_op_expression(const binary_op_expression& other) : arith_expression_base(kind_t::binary_aop), left(other.left), right(other.right), op(op) {}

		std::string to_string() const override
		{
			return "(" + left->to_string() + ")" + static_cast<char>(op) + "(" + right->to_string() + ")";
		}

		bool is_equal(const ast_base& other) const override
		{
			return other.kind == kind && *this == dynamic_cast<const binary_op_expression&>(other);
		}

		bool operator==(const binary_op_expression& other) const
		{
			return op == other.op && left->is_equal(*other.left) && right->is_equal(*other.right);
		}
	};
}
