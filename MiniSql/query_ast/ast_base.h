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
			binary_aop,
			select,
			from,
			where
		};

		kind_t kind;
		ast_base(const kind_t kind) : kind(kind){}
		ast_base(const ast_base& other) = default;
		ast_base(ast_base&& other) = default;

		[[nodiscard]] virtual std::string to_string() const = 0;
		[[nodiscard]] bool operator==(const ast_base& other) const { return this->is_equal(other); }

		[[nodiscard]] virtual bool is_equal(const ast_base& other) const = 0;
		virtual ~ast_base() = default;
	};

	inline std::ostream& operator<<(std::ostream& out, const ast_base& aeb)
	{
		out << aeb.to_string();
		return out;
	}

}
