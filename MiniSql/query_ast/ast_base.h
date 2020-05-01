#pragma once

namespace minisql::query_ast
{
	struct ast_base
	{
		enum class kind_t : short
		{
			unknown = 0,
			number,
			boolean,
			string,
			null,
			identifier,
			binary_aop
		};

		kind_t kind;
		ast_base(const kind_t kind) : kind(kind){}
		ast_base(const ast_base& other) = default;
		ast_base(ast_base&& other) = default;

		virtual std::string to_string() const = 0;
		bool operator==(const ast_base& other) const { return is_equal(other); }

		virtual bool is_equal(const ast_base& other) const = 0;
	};
}
