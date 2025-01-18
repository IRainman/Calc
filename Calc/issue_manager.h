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
		static void speedup() noexcept
		{
			_errors.reserve(10);
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
			_warnings.reserve(10);
			_infos.reserve(10);
#endif
		}

#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		/**
		 * Report a new info message.
		 */
		static void report_info(EquationSize pos, std::string&& text) noexcept
		{
			_infos.emplace_back(Issue(std::move(text), pos));
		}
		
		/**
		 * Report a new warning.
		 */
		static void report_warning(EquationSize pos, std::string&& text) noexcept
		{
			_warnings.emplace_back(Issue(std::move(text), pos));
		}
#endif
		/**
		 * Report a new error.
		 */
		static void report_error(EquationSize pos, std::string&& text) noexcept
		{
			_errors.emplace_back(std::move(Issue(std::move(text), pos)));
		}
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		[[nodiscard]] static bool has_errors() noexcept
		{
			return !_errors.empty();
		}

		/**
		 * Clear the manager.
		 */
		static void clear() noexcept
		{
			_errors.clear();
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
			_warnings.clear();
			_infos.clear();
#endif
		}
		
	private:
		static std::vector<Issue> _errors; // Errors that do stop processing.
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		static std::vector<Issue> _warnings; // Warnings that don't stop processing.
		static std::vector<Issue> _infos; // Information and comments.
#endif
		friend class Formatter;
};
