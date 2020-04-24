
#include "../MiniSql/parsers.h"
#include "minitest.h"


namespace minisql::parser::tests
{
	using namespace minitest;
	using namespace std::literals;

	struct number_tests : public test_fixture<number_tests>
	{
		number_tests()
		{
			const auto parser = number();

			using tc = std::tuple<std::string, double>;
			auto parse_number = [&parser](const std::string& s, double e)
			{
				unsigned pos = 0;
				const auto pr = parser(s, pos);
				is_true(pr.success);
				are_equal(e, pr.result);
			};
			test<tc>("parsers positive integers", parse_number,
				{
					{"0", 0},
					{"1", 1},
					{"1732", 1732},
					{"1500100900", 1500100900},
					{"+178", 178}
				});

			test<tc>("parsers negative integers", parse_number,
				{
					{"-1", -1},
					{"-2345", -2345}
				});

			test<tc>("parses real numbers", parse_number,
				{
						{"-1.123", -1.123},
						{"+59991.123", 59991.123},
						{"999999999999.123456", 999999999999.123456}
				});

			test("parses nans", [&parser]()
				{
					unsigned pos;
					const auto pr = parser("NaN", pos);
					is_true<decltype(std::isnan<double>), double>(std::isnan, pr.result);
				});
			test("parses infs", [&parser]()
				{
					unsigned pos;
					const auto pr = parser("Inf", pos);
					is_true<decltype(std::isinf<double>), double>(std::isinf, pr.result);
					is_true<std::greater<double>, double, double>({}, pr.result, 0);
					const auto pr2 = parser("-Inf", pos);
					is_true<decltype(std::isinf<double>), double>(std::isinf, pr2.result);
					is_true<std::less<double>, double, double>({}, pr.result, 0);
				});
		}
	} number_tests_i;
}
