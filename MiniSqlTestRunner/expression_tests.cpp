#include <tuple>

#include "minitest.h"
#include "../MiniSql/query_ast.h"
#include "../MiniSql/query_parser.h"
#include "ast_helper.h"


namespace minisql::query_parser::tests
{
	using namespace minitest;
	using namespace query_ast;
	using namespace query_ast::helpers;

	struct expression_tests : public test_fixture<expression_tests>
	{

		expression_tests()
		{
			using namespace std::literals;

			using tcn = std::tuple<std::string, query_ast::number>;
			test<tcn>("parses number values", [](const auto& s, const auto& e)
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser(s, pos);
					is_true(pr.success, "expected parsing to succeed");
					const auto result = std::dynamic_pointer_cast<query_ast::number>(pr.result);
					is_true(result != nullptr, "expected a number");
					are_equal(e, *result);
				},
				{
					{"-1.1234", query_ast::number(-1.1234) },
					{ "3.14518", query_ast::number(3.14518) },
				});

			using tci = std::tuple<std::string, query_ast::identifier>;
			test<tci>("parses identifier", [](const auto& s, const auto& e)
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser(s, pos);
					is_true(pr.success, "expected parsing to succeed");
					const auto result = std::dynamic_pointer_cast<query_ast::identifier>(pr.result);
					is_true(result != nullptr, "expected an identifier");
					are_equal(e, *result);
				},
				{
					{"_abc", query_ast::identifier("_abc"s) },
					{ "n12_13", query_ast::identifier("n12_13"s) },
					{"nullident", query_ast::identifier("nullident") }
				});

			test("parses null", []()
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser("null", pos);
					is_true(pr.success);
					const auto result = std::dynamic_pointer_cast<null>(pr.result);
					is_true(result != nullptr, "expected a null");
				});

			test("parsers simple binary operators", []()
				{
					{
						unsigned pos = 0;
						const auto parser = expression();
						const auto pr = parser("1+3", pos);
						is_true(pr.success);
						const auto result = std::dynamic_pointer_cast<binary_op_expression>(pr.result);
						is_true(result != nullptr, "expected a binary + expression");
						const auto left = std::dynamic_pointer_cast<query_ast::number>(result->left);
						const auto right = std::dynamic_pointer_cast<query_ast::number>(result->right);
						is_true(left != nullptr && right != nullptr, "expected numbers as operands");
						are_equal(query_ast::number(1), *left);
						are_equal(query_ast::number(3), *right);
					}

					{
						unsigned pos = 0;
						const auto parser = expression();
						const auto pr = parser("6*7", pos);
						is_true(pr.success);
						const auto result = std::dynamic_pointer_cast<binary_op_expression>(pr.result);
						is_true(result != nullptr, "expected a binary * expression");
						const auto left = std::dynamic_pointer_cast<query_ast::number>(result->left);
						const auto right = std::dynamic_pointer_cast<query_ast::number>(result->right);
						is_true(left != nullptr && right != nullptr, "expected numbers as operands");
						are_equal(query_ast::number(6), *left);
						are_equal(query_ast::number(7), *right);
					}
				});

			test("parsers complex expressions", []()
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser("1+3+(4+5)", pos);
					is_true(pr.success);
					const auto& expected_ptr = build(
						build(
							build(1.0),
							'+',
							build(3.0)),
						'+',
						build(
							build(4.0),
							'+',
							build(5.0)
						)
					);

					are_equal(*expected_ptr, *pr.result);
				});

			test("parsers expressions with different op priorities", []()
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser("1+2*3", pos);
					is_true(pr.success);

					const auto& expected_ptr = build(
						build(1.0),
						'+',
						build(
							build(2.0),
							'*',
							build(3.0)
						)
					);

					are_equal(*expected_ptr, *pr.result);
				});

			test("parsers complex expressions with nulls and identifiers and booleans and strings", []()
				{
					unsigned pos = 0;
					const auto parser = expression();
					const auto pr = parser("true+null*some_id/'some_string *#/'*3+''", pos);
					is_true(pr.success);

					const auto& expected_ptr = build(
						build(
							build(true),
							'+',
							build(
								build(
									build(build(), '*', build<query_ast::identifier>("some_id")),
									'/',
									build<string>("some_string *#/")),
								'*',
								build(3.0))),
						'+',
						build<query_ast::string>(""));

					are_equal(*expected_ptr, *pr.result);
				});

			using tc = std::tuple<std::string, expression_base_ptr>;
			test<tc>("parsers boolean expressions", [](const auto& s, expression_base_ptr expected)
				{
					unsigned pos = 0;
					const auto pr = expression()(s, pos);
					is_true(pr.success);
					are_equal(*expected, *pr.result);
				}, {
						{"1 > 0", build(build(1.0), '>', build(0.0))},
						{"1 + 3 <= 0", build(build(build(1.0), '+', build(3.0)), 243, build(0.0))},
						{"(1 >= 0) + (3 = null)", build(
							build(build(1.0), 242, build(0.0)),
							'+',
							build(build(3.0), '=', build()))} // this is correct as far as parser is concerned, to be rejected or coerced to number at later stage
				});
		}
	} expression_tests_i;
}