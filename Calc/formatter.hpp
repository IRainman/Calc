#pragma once

/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

class Formatter
{
	public:		
		using Result = std::array<char, 64>;
		using Summary = std::array<char, 128>;
		/**
		 * Format output value of the expression returned. Return the end of formated value.
		 */
		static char* format(Value value, Result& ret) noexcept;
#ifndef CALC_USE_ERROR_TOKEN
		/**
		 * Create full report of expression processing. Return the end of formated summary.
		 */
		static char* create_summary(Summary& ret) noexcept;
#endif
};

