/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#pragma once
#ifndef CALC_VER2
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
void ProcessFunctions(string& p_input_str
#ifdef ENABLE_INPUT_VALIDATION
                      , const string::size_type p_mes_pos_shift = 0
#endif
                     );
//---------------------------------------------------------------------------
#endif