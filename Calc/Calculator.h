/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#pragma once

#include "Flags.h"
//---------------------------------------------------------------------------
#ifdef CALC_VER2
const std::string& Calculate(const std::string_view p_input, std::string& p_output);
#else
const std::string& Calculate(std::string p_input, std::string& p_output);
#endif
//---------------------------------------------------------------------------