#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

/**
 * Represents a message from the compiler.
 */
class Issue
{
	public:
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
		Issue(std::string&& t, size_t p, Severity s) : text(t), pos(p), severity(s) {};
		Issue(const Issue&) = delete;
		Issue(Issue&&) = default;
	private:
		// Issue describing of the message.
		const std::string text;

		// Position within the context at which the issue has occurred.
		const size_t pos; //-V122

		// Issue severity.
		const Severity severity;

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

		/**
		 * Report a new info message.
		 */
		static void report_info(size_t pos, std::string&& text) noexcept;
		
		/**
		 * Report a new warning.
		 */
		static void report_warning(size_t pos, std::string&& text) noexcept;
		
		/**
		 * Report a new error.
		 */
		static void report_error(size_t pos, std::string&& text) noexcept;
		
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
		static bool _has_errors;
		static std::vector<Issue> _messages;
};
