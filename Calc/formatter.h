#pragma once

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

class Formatter
{
	public:
		/**
		 * Format output value of the expression returned.
		 */
		[[nodiscard]] static std::string format(Value value) noexcept;
#ifndef CALC_USE_ERROR_TOKEN
		/**
		 * Create full report of expression processing.
		 */
		[[nodiscard]] static std::string create_summary() noexcept;
#endif
};

