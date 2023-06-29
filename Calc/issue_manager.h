#pragma once
//
// Prototype writed by Tamika Nomara on 28.05.2023.
//
// Full version implemented by Elle Solomina and maintained since 29.05.2023.
//

/**
 * Represents a message from the compiler.
 */
struct Issue
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
	
	// Issue describing of the message.
	const std::string text;
	
	// Issue severity.
	const Severity severity;
	
	// Position withing the context at which the issue has occurred.
	const size_t pos; //-V122
};

/**
 * Reports and prints issues.
 */
class IssueManager
{
	public:
		/**
		 * Return the link to the static instance.
		 */
		[[nodiscard]] static IssueManager& get_instance() noexcept;
		
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
		[[nodiscard]] bool has_errors() const noexcept
		{
		    return _has_errors;
		}
		
		/**
		 * Return reference to the vector of all messages have been reported so far.
		 */
		[[nodiscard]] const auto& messages() const noexcept
		{
		    return _messages;
		}
		
		/**
		 * Clear the manager.
		 */
		void clear() noexcept
		{
			_messages.clear();
			_has_errors = false;
		}
		
	private:
		/**
		 * Report a new issue.
		 */
		void report(Issue&& issue);
		void report(Issue::Severity severity, size_t pos, std::string&& text);

		bool _has_errors = false;
		std::vector<Issue> _messages;
};
