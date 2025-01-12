#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "flags.h"
/**
 * Represents a message from the compiler.
 */
class Issue
{
	public:
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		/**
		 * How severe is this message?
		 */
		enum class Severity : unsigned int
		{
			// Information and comments.
			INFO = 0,

			// Warnings that don't stop processing.
			WARN = 1,

			// Errors that do stop processing.
			ERR = 1 << 1,
		};
#endif
		Issue(std::string&& t, size_t p
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
			, Severity s
#endif
		) : text(t), pos(p)
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
			, severity(s)
#endif
		{};
		Issue(const Issue&) = delete;
		Issue(Issue&&) = default;
	private:
		// Issue describing of the message.
		const std::string text;

		// Position within the context at which the issue has occurred.
		const size_t pos; //-V122

#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		// Issue severity.
		const Severity severity;
#endif

		friend class Formatter;
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	public:
		/**
		 * Speedup the manager if needed.
		 */
		static void speedup() noexcept
		{
			_messages.reserve(10);
		}

#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		/**
		 * Report a new info message.
		 */
		static void report_info(size_t pos, std::string&& text) noexcept;
		
		/**
		 * Report a new warning.
		 */
		static void report_warning(size_t pos, std::string&& text) noexcept;
#endif
		/**
		 * Report a new error.
		 */
		static void report_error(size_t pos, std::string&& text) noexcept;
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		[[nodiscard]] static bool has_errors() noexcept
		{
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		    return _has_errors;
#else
			return !_messages.empty();
#endif
		}
		
		/**
		 * Clear the manager.
		 */
		static void clear() noexcept
		{
			_messages.clear();
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
			_has_errors = false;
#endif
		}
		
	private:
		/**
		 * Report a new issue.
		 */
		static std::vector<Issue> _messages;
#ifdef ISSUE_MANAGER_HAVE_SEVERITY
		static bool _has_errors;
#endif
		friend class Formatter;
};
