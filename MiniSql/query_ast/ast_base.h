#pragma once
#include <string>

namespace minisql::query_ast
{
	struct ast_base
	{
		const std::string raw;

		ast_base() {}
		ast_base(const std::string& raw) : raw(raw) {}
	};
}
