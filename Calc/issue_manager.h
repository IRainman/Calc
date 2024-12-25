#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

/**
 * Represents a message from the compiler.
 */
struct Issue
{
	/**
	 * How severe is this message?
	 */
	enum class Severity : unsigned int
	{
		// Errors that do stop processing.
		ERR,

		// Warnings that don't stop processing.
		WARN,

		// Information and comments.
		INFO,
	};

	// Issue describing of the message.
	const std::string text;

	// Position withing the context at which the issue has occurred.
	const size_t pos; //-V122

	// Issue severity.
	const Severity severity;
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	public:
		/**
		 * Report a new info message.
		 */
		static void report_info(size_t pos, std::string&& text);
		
		/**
		 * Report a new warning.
		 */
		static void report_warning(size_t pos, std::string&& text);
		
		/**
		 * Report a new error.
		 */
		static void report_error(size_t pos, std::string&& text);
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		[[nodiscard]] static bool has_errors() noexcept
		{
		    return _has_errors;
		}
		
		/**
		 * Return reference to the vector of all messages have been reported so far.
		 */
		[[nodiscard]] static const auto& messages() noexcept
		{
		    return _messages;
		}
		
		/**
		 * Clear the manager.
		 */
		static void clear() noexcept
		{
			_messages.clear();
			_has_errors = false;
		}
		
	private:
		/**
		 * Report a new issue.
		 */
		static void report(Issue&& issue);
		static void report(Issue::Severity severity, size_t pos, std::string&& text);

		static bool _has_errors;
		static std::vector<Issue> _messages;
};
