#include "minitest.h"
#include "../MiniSql/query_parser/expression.h"

namespace minisql::query_parser
{
	struct identifier_test : public minitest::test_fixture<identifier_test>
	{
		identifier_test()
		{
			test("parses single char identifier", []
				{
					unsigned pos = 0;
					const auto pr = identifier()("D", pos);
					is_true(pr.success);
					are_equal("D"s, pr.result.identifier);
				});
			test("parses multi character identifier", []
				{
					unsigned pos = 0;
					const auto pr = identifier()("some_identifier", pos);
					is_true(pr.success);
					are_equal("some_identifier"s, pr.result.identifier);
				});
			test("parses identifier starting with underscore", []
				{
					unsigned pos = 0;
					const auto pr = identifier()("_underscore", pos);
					is_true(pr.success);
					are_equal("_underscore"s, pr.result.identifier);
				});
			test("parses identifier with digits", []
			{
					unsigned pos = 0;
					const auto pr = identifier()("abc123", pos);
					is_true(pr.success);
					are_equal("abc123"s, pr.result.identifier);
			});
			test<std::string>("does not parse identifier starting with non alpha char or underscore", [](const std::string& s)
			{
				{
					unsigned pos = 0;
					const auto pr = identifier()(s, pos);
					is_true(!pr.success);
				}
			}, { "-%$1", "1acbs", "  fad ", "\tbla", "*zonk"});

			test<std::string>("does not parse keywords", [](const std::string &s)
			{
					unsigned pos = 0;
					const auto pr = identifier()(s, pos);
					is_true(!pr.success);
			}, { "select", "from", "where", "and", "or", "order by", "having", "with", "as", "between", "left", "right", "outer", "inner", "join", "cross"});

		}
	} identifier_test_i;
}