#include <tuple>

#include "minitest.h"
#include "../MiniSql/query_ast.h"
#include "../MiniSql/query_parser.h"


namespace minisql::query_parser::tests
{
	using namespace minitest;

	struct expression_tests : public test_fixture<expression_tests>
	{
		expression_tests()
		{
			using namespace std::literals;

			using tcn = std::tuple<std::string, query_ast::number>;
			test<tcn>("parses number values", [](const auto& s, const auto& e)
				{
					unsigned pos = 0;
					const auto parser = binary_op_expression();
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
					const auto parser = binary_op_expression();
					const auto pr = parser(s, pos);
					is_true(pr.success, "expected parsing to succeed");
					const auto result = std::dynamic_pointer_cast<query_ast::identifier>(pr.result);
					is_true(result != nullptr, "expected an identifier");
					are_equal(e, *result);
				},
				{
					{"_abc", query_ast::identifier("_abc"s) },
					{ "n12_13", query_ast::identifier("n12_13"s) },
				});

			test("parses null", []()
				{
					unsigned pos = 0;
					const auto parser = binary_op_expression();
					const auto pr = parser("null", pos);
					is_true(pr.success);
					const auto result = std::dynamic_pointer_cast<query_ast::null>(pr.result);
					is_true(result != nullptr, "expected a null");
				});

			test("parsers simple binary operators", []()
				{
					{
						unsigned pos = 0;
						const auto parser = binary_op_expression();
						const auto pr = parser("1+3", pos);
						is_true(pr.success);
						const auto result = std::dynamic_pointer_cast<query_ast::binary_op_expression>(pr.result);
						is_true(result != nullptr, "expected a binary + expression");
						const auto left = std::dynamic_pointer_cast<query_ast::number>(result->left);
						const auto right = std::dynamic_pointer_cast<query_ast::number>(result->right);
						is_true(left != nullptr && right != nullptr, "expected numbers as operands");
						are_equal(query_ast::number(1), *left);
						are_equal(query_ast::number(3), *right);
					}

					{
						unsigned pos = 0;
						const auto parser = binary_op_expression();
						const auto pr = parser("6*7", pos);
						is_true(pr.success);
						const auto result = std::dynamic_pointer_cast<query_ast::binary_op_expression>(pr.result);
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
					const auto parser = binary_op_expression();
					const auto pr = parser("1+3+(4+5)", pos);
					is_true(pr.success);
					const auto result = std::dynamic_pointer_cast<query_ast::binary_op_expression>(pr.result);
					is_true(result != nullptr, "expected a binary op expression");
					const auto left_expr = std::dynamic_pointer_cast<query_ast::binary_op_expression>(result->left);
					const auto right_expr = std::dynamic_pointer_cast<query_ast::binary_op_expression>(result->right);
					is_true(left_expr != nullptr && right_expr != nullptr);
					const auto one = std::dynamic_pointer_cast<query_ast::number>(left_expr->left);
					const auto three = std::dynamic_pointer_cast<query_ast::number>(left_expr->right);
					const auto four = std::dynamic_pointer_cast<query_ast::number>(right_expr->left);
					const auto five = std::dynamic_pointer_cast<query_ast::number>(right_expr->right);
					are_equal(*one, query_ast::number(1));
					are_equal(*three, query_ast::number(3));
					are_equal(*four, query_ast::number(4));
					are_equal(*five, query_ast::number(5));
				});

			test("parsers expressions with different op priorities", []()
				{
					unsigned pos = 0;
					const auto parser = binary_op_expression();
					const auto pr = parser("1+2*3", pos);
					is_true(pr.success);
					const auto result = std::dynamic_pointer_cast<query_ast::binary_op_expression>(pr.result);
					is_true(result != nullptr, "expected a binary op expression");
					const auto left_expr = std::dynamic_pointer_cast<query_ast::number>(result->left);
					const auto right_expr = std::dynamic_pointer_cast<query_ast::binary_op_expression>(result->right);
					is_true(left_expr != nullptr && right_expr != nullptr);
					const auto one = std::dynamic_pointer_cast<query_ast::number>(left_expr);
					const auto two = std::dynamic_pointer_cast<query_ast::number>(right_expr->left);
					const auto three = std::dynamic_pointer_cast<query_ast::number>(right_expr->right);
					are_equal(*one, query_ast::number(1));
					are_equal(*two, query_ast::number(2));
					are_equal(*three, query_ast::number(3));
				});
		}
	} expression_tests_i;
}