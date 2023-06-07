//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

#pragma once

#include "Flags.h"

#include "token.h"

/**
 * Represents a message from the compiler.
 */
struct Message
{
	/**
	 * How severe is this message?
	 */
	enum class Severity: unsigned int
	{
	    // Errors that do stop processing.
	    ERR,
	    
	    // Warnings that don't stop processing.
	    WARN,
	    
	    // Information and comments.
	    INFO,
	};
	
	// Message describing of the the message.
	std::string text;
	
	// Message severity.
	Severity severity;
	
	// Position withing the context at which the issue has occurred.
	size_t pos; //-V122
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	public:
	
		static IssueManager& get_instance() noexcept;
		
		/**
		 * Report a new issue.
		 */
		void report(Message&& issue);
		void report(Message::Severity severity, size_t pos, std::string&& text);
		
		/**
		 * Report a new info message.
		 */
		void report_info(size_t pos, std::string&& text);
		
		/**
		 * Report a new warning.
		 */
		void report_warning(size_t pos, std::string&& text);
		
		/**
		 * Report a new error.
		 */
		void report_error(size_t pos, std::string&& text);
		
		/**
		 * Indicate whether any messages have been reported so far.
		 */
		bool has_errors() const noexcept
		{
		    return _has_errors;
		}
		
		/**
		 * Return reference to the vector of all messages have been reported so far.
		 */
		const auto& messages() const noexcept
		{
		    return _messages;
		}
		
		/**
		 * Return full report of expression processing and clear the manager queue.
		 */
		[[deprecated("needs to move from here to another new class, like formatter, or something like this")]]
		[[nodiscard]] const std::string& create_summary_and_clear();
		
		/**
		 * Clear the manager.
		 */
		void clear() noexcept
		{
			_messages.clear();
			_has_errors = false;
		}
		
	private:
		bool _has_errors = false;
		std::vector<Message> _messages;
		std::string _summary;
};
