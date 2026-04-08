#pragma once
/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef CALC_USE_ERROR_TOKEN
#include <vector>
/**
 * Represents a message from the compiler.
 */
class Issue
{
	public:
		Issue(std::string_view&& _text, const EquationSize _pos) noexcept : text(_text), pos(_pos) {};
		Issue(const Issue&) = delete;
		Issue(Issue&&) = default;
	private:
		// Issue describing of the message.
		const std::string_view text;

		// Position within the context at which the issue has occurred.
		const EquationSize pos; //-V122

		friend class Formatter;
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	using Issues = std::vector<Issue>;
	public:
		/**
		 * Report a new error.
		 */
		static void report_error(const EquationSize pos, std::string_view&& text) noexcept;
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		[[nodiscard]] static bool has_errors() noexcept;

		/**
		 * Clear the manager.
		 */
		static void clear() noexcept;
		
	private:
		static Issues _errors; // Errors that do stop processing.
		friend class Formatter;
};

#endif