#include "minitest.h"
#include "../MiniSql/query_parser.h"
#include "../MiniSql/query_ast.h"
#include "ast_helper.h"

namespace minisql::query_parser::tests
{
	using namespace minitest;
	using namespace query_ast;
	using namespace query_ast::helpers;
	
	struct select_tests : public test_fixture<select_tests>
	{
		select_tests()
		{
			using tc = std::tuple<std::string, std::shared_ptr<ast_base>>;
			test<tc>("parsers simple select statements", [](const std::string& s, std::shared_ptr<ast_base> expected)
				{
					unsigned pos = 0;
					const auto pr = select()(s, pos);
					is_true(pr.success);
					const ast_base& ref = *pr.result;
					are_equal(*expected, ref);
				},
				{
					{"select 1", build({build(1.0)}) },
					{"select 1, true, null, some_column, 'some string'", build({build(1.0), build(true), build(), build<query_ast::identifier>("some_column"), build<string>("some string")}) }
				});
		}
	};
}
