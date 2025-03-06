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
#ifdef CALC_USING_STATIC_VECTOR
	//using opt = boost::container::static_vector_options<boost::container::throw_on_overflow<false>, boost::container::inplace_alignment<4>>;
	using Issues = boost::container::static_vector<Issue, CALC_MAX_ISSUES>;
#else
	using Issues = std::vector<Issue>;
#endif
	using EquationSize = Token::EquationSize;
	public:
#ifndef CALC_USING_STATIC_VECTOR
		/**
		 * Speedup the manager if needed.
		 */
		static void speedup() noexcept;
#endif
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
		static void emplace(Issues& v, EquationSize pos, std::string&& text) noexcept;
		static Issues _errors; // Errors that do stop processing.
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		static Issues _warnings; // Warnings that don't stop processing.
		static Issues _infos; // Information and comments.
#endif
		friend class Formatter;
};
