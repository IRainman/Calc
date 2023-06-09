/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#pragma once

#include "issue_manager.h"

template<typename S, typename V>
inline constexpr S& print_value(S& p_str, const V p_val)
{
	// TODO concepts
	p_str.resize(32); // TODO fix magic number... with what?
	p_str.resize(std::to_chars(p_str.data(), p_str.data() + p_str.size(), p_val, std::chars_format::general, std::numeric_limits<V>::digits10 - 1).ptr - p_str.data());
	return p_str;
}

class Formatter
{
	public:
		/**
		 * Return the link to the static instance.
		 */
		static Formatter& get_instance() noexcept;

		/**
		 * Return full report of expression processing and clear the issue manager queue.
		 */
		[[nodiscard]] const std::string& create_summary_and_clear_issue_manager();

	private:
		static std::string format_severity(const Issue::Severity severity);
		static std::string format_issue(const Issue& issue);

		std::string _summary;
};

