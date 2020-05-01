#include "../MiniSql/parsers/character.h"
#include "minitest.h"


namespace minisql::parsers::tests
{
	using namespace minitest;
	using namespace std::literals;

	struct character_tests : test_fixture<character_tests>
	{
		character_tests()
		{
			// character tests
			{
				auto parser = minisql::parsers::character('a');
				test("parsers specific character", [&parser]()
					{
						unsigned pos = 0;
						const auto pr = parser("abcd", pos);
						is_true(pr.success);
						are_equal('a', pr.result);
					});

				test("fails to parse character other than specified", [&parser]()
					{
						unsigned pos = 1;
						const auto pr = parser("abcd", pos);
						is_true(!pr.success);
						are_equal("Pos 1: Expected 'a' but got bcd"s, pr.error);
					});

				test("fails to parse character past the end of input", [&parser]()
					{
						unsigned pos = 7;
						const auto pr = parser("abcd", pos);
						is_true(!pr.success);
						are_equal("Pos 7: Expected 'a' but got end of string"s, pr.error);
					});
			}

			// whitespace tests
			{
				const auto parser = minisql::parsers::whitespace();
				test("parses whitespace", [&parser]()
					{
						unsigned pos = 0;
						{
							const auto pr = parser(" \t\nabcd", pos);
							is_true(pr.success);
							are_equal(' ', pr.result);
						}
						{
							const auto pr = parser(" \t\nabcd", pos);
							is_true(pr.success);
							are_equal('\t', pr.result);
						}
						{
							const auto pr = parser(" \t\nabcd", pos);
							is_true(pr.success);
							are_equal('\n', pr.result);
						}
					});

				test("fails to parse whitespace when other character found", [&parser]()
					{
						unsigned pos = 0;
						const auto pr = parser("abcd", pos);
						is_true(!pr.success);
						are_equal("Pos 0: Expected whitespace but got abcd"s, pr.error);
					});

				test("fails to parse whitespace at end of string", [&parser]()
					{
						unsigned pos = 0;
						const auto pr = parser("", pos);
						is_true(!pr.success);
						are_equal("Pos 0: Expected whitespace but got end of string"s, pr.error);
					});
			}

			// digit tests
			{
				auto parser = minisql::parsers::digit{};
				using tc = std::tuple<std::string, char>;
				std::vector<tc> test_cases(10);
				std::generate(test_cases.begin(), test_cases.end(), []()
					{
						static int i = -1;

						return i++, std::make_tuple(std::to_string(i), '0' + i);
					});

				test_cases.emplace_back("13", '1');
				test_cases.emplace_back("9ab", '9');

				test<tc>("parses single digit"s, [&parser](std::string s, char e)
					{
						unsigned pos = 0;
						const auto pr = parser(s, pos);
						is_true(pr.success);
						is_true<std::equal_to<char>, char, char>({}, pr.result, e);
					}, test_cases);

				test("fails to parse empty string"s, [&parser]()
					{
						unsigned pos = 0;
						const auto pr = parser("", pos);
						is_true(!pr.success);
					});

				test("fails to parse past the input"s, [&parser]()
					{
						unsigned pos = 5;
						const auto pr = parser("1234"s, pos);
						is_true(!pr.success);
					});

				test("fails to parse when no digit at specified pos"s, [&parser]()
					{
						unsigned pos = 0;
						const auto pr = parser("abcd"s, pos);
						is_true(!pr.success);
						are_equal("Pos 0: Expected digit but got abcd"s, pr.error);
					});
			}
		}
	} character_tests_i;
}
