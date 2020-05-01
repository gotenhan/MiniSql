#pragma once
#include <iostream>
#include <string>
#include <ostream>
#include <sstream>
#include <tuple>
#include <vector>
#include "color.h"

namespace minitest
{
	using namespace std::literals;

	template<typename Type, unsigned N, unsigned Last>
	struct tuple_printer {

		static void print(std::ostream& out, const Type& value) {
			out << std::get<N>(value) << ", ";
			tuple_printer<Type, N + 1, Last>::print(out, value);
		}
	};

	template<typename Type, unsigned N>
	struct tuple_printer<Type, N, N> {

		static void print(std::ostream& out, const Type& value) {
			out << std::get<N>(value);
		}

	};

	template<typename... Types>
	std::ostream& operator<<(std::ostream& out, const std::tuple<Types...>& value) {
		out << "(";
		tuple_printer<std::tuple<Types...>, 0, sizeof...(Types) - 1>::print(out, value);
		out << ")";
		return out;
	}

	template <typename ...TArgs>
	struct test_case
	{
		std::string name;
		std::tuple<TArgs...> args;
		test_case(std::string name, TArgs... args) : name(std::move(name)), args(std::make_tuple(std::forward<decltype(args)>(args)...)) {}
	};

	template <typename ...TTestCaseArgs>
	std::ostream& operator<<(std::ostream& stream, const test_case<TTestCaseArgs...>& test_case)
	{
		stream << test_case.name;
		return stream;
	}

	struct failure 
	{
		std::string message;
	};

	template<typename T>
	struct has_name_method
	{
	private:
		typedef std::true_type yes;
		typedef std::false_type no;
			
		template<typename U> static auto test(int a ) -> decltype(a == 1 && U::name() == ""s, yes());

		template<typename> static no test(...);

	public:
		static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
	};

	template <typename TFixture>
	struct test_fixture
	{
		template <typename TArgType>
		static std::tuple<TArgType> get_args(const TArgType& arg) noexcept { return std::make_tuple(arg); }

		template <typename ...TArgs>
		static std::tuple<TArgs...> get_args(const std::tuple<TArgs...>& args) noexcept { return args; }

		template <typename ...TArgs>
		static std::tuple<TArgs...> get_args(const test_case<TArgs...>& tc) noexcept { return tc.args; }

		static constexpr std::string fixture_name()
		{
			if constexpr (has_name_method<TFixture>::value)
				return TFixture::name();
			else
				return typeid(TFixture).name();
		}

		template <typename TTestCase, typename TCallable>
		static void test(std::string test_name, TCallable test_method, std::vector<TTestCase> test_cases) noexcept
		{
			for (auto&& t : test_cases)
			{
				try {
					std::apply(test_method, get_args(t));

					std::cout << hue::green << "[PASSED] [" << fixture_name() << "] [" << test_name << "] [" << t << "]" <<  hue::reset  << std::endl;
				}
				catch (const failure& f)
				{
					std::cout << hue::red << "[FAILED] [" << fixture_name() << "] [" << test_name << "] [" << t << "] : " << f.message << hue::reset << std::endl;
				}
				catch (...)
				{
					std::cout << hue::yellow << "[ERROR] [" << fixture_name() << "] [" << test_name << "] [" << t << "] : exception thrown" << hue::reset << std::endl;
				}
			}
		}

		template <typename TCallable>
		static void test(std::string test_name, TCallable test_method) noexcept
		{
			try {
				test_method();

				std::cout << hue::green << "[PASSED] [" << fixture_name() << "] [" << test_name << "]" << hue::reset << std::endl;
			}
			catch (const failure& f)
			{
				std::cout << hue::red << "[FAILED] [" << fixture_name() << "] [" << test_name << "] : " << f.message << hue::reset << std::endl;
			}
			catch (...)
			{
				std::cout << hue::yellow << "[ERROR] [" << fixture_name() << "] [" << test_name << "] : exception thrown" << hue::reset << std::endl;
			}
		}

		static void is_true(bool result, const std::string& failure_message = "expected true but got false"s)
		{
			if(!result)
			{
				throw failure{ failure_message };
			}
		}

		template <typename TPredicate, typename ...TArgs>
		static void is_true(TPredicate predicate, const TArgs&... args, const std::string& failure_message = "expected true but got false"s)
		{
			const bool result = predicate(args...);
			if(!result)
			{
				throw failure{ failure_message };
			}
		}

		template <typename T1>
		static void are_equal(const T1& arg1, const T1& arg2)
		{
			std::ostringstream str;
			using std::to_string;
			str << "Expected [" << arg2 << "] to be equal to [" << arg1 << "]";
			is_true<std::equal_to<T1>, T1, T1>({}, arg1, arg2, str.str());
		}
	};
}
