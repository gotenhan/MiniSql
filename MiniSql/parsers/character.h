#pragma once
#include "parser_base.h"

namespace minisql::parser
{
	class character_base : public parser_base<char>
	{
	private:
		[[nodiscard]] virtual auto is_match(const char c) const -> bool = 0;
		auto parse(const std::string& input, unsigned& current_pos) const -> parse_result<char> override
		{
			char c = input[current_pos];

			if (!is_match(c))
				return error_message(input, current_pos);

			current_pos++;
			return ok(c);
		}
	};

	class character final : public character_base
	{
		const char expected;
	public:
		character(const char c) : expected(c) {}
		[[nodiscard]] std::string to_string() const override { return std::string("'") + expected + std::string("'"); }
	private:
		[[nodiscard]] auto is_match(const char c) const -> bool override
		{
			return expected == c;
		}
	};

	class digit final : public character_base
	{
	public:
		[[nodiscard]] auto to_string() const -> std::string override { return "digit"; }
	private:
		[[nodiscard]] auto is_match(const char c) const -> bool override { return std::isdigit(c); }
	};

	class whitespace final : public character_base
	{
	public:
		[[nodiscard]] auto to_string() const -> std::string override { return "whitespace"; }
	private:
		[[nodiscard]] auto is_match(const char c) const -> bool override { return std::isspace(c); }
	};

	class alpha final : public character_base
	{
	public:
		[[nodiscard]] auto to_string() const -> std::string override { return "whitespace"; }
	protected:
		[[nodiscard]] auto is_match(const char c) const -> bool override { return std::isalpha(c); }
	};

	class alphanum final : public character_base
	{
	public:
		[[nodiscard]] auto to_string() const -> std::string override { return "whitespace"; }
	protected:
		[[nodiscard]] auto is_match(const char c) const -> bool override { return std::isalpha(c); }
	};

}
