#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "flags.h"
#include "token.h"
/**
 * Represents a message from the compiler.
 */
class Issue
{
	using EquationSize = Token::EquationSize;
	public:
		Issue(std::string&& t, EquationSize p) noexcept : text(t), pos(p) {};
		Issue(const Issue&) = delete;
		Issue(Issue&&) = default;
	private:
		// Issue describing of the message.
		const std::string text;

		// Position within the context at which the issue has occurred.
		const EquationSize pos; //-V122

		friend class Formatter;
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	using EquationSize = Token::EquationSize;
	public:
		/**
		 * Speedup the manager if needed.
		 */
		static void speedup() noexcept;

#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		/**
		 * Report a new info message.
		 */
		static void report_info(EquationSize pos, std::string&& text) noexcept;
		
		/**
		 * Report a new warning.
		 */
		static void report_warning(EquationSize pos, std::string&& text) noexcept;
#endif
		/**
		 * Report a new error.
		 */
		static void report_error(EquationSize pos, std::string&& text) noexcept;
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		[[nodiscard]] static bool has_errors() noexcept;

		/**
		 * Clear the manager.
		 */
		static void clear() noexcept;
		
	private:
		static void emplace(std::vector<Issue>& v, EquationSize pos, std::string&& text) noexcept;
		static std::vector<Issue> _errors; // Errors that do stop processing.
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		static std::vector<Issue> _warnings; // Warnings that don't stop processing.
		static std::vector<Issue> _infos; // Information and comments.
#endif
		friend class Formatter;
};
