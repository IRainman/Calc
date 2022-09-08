/*
 * Copyright 2016 Solomina Elle Leonovna, a.rainman on gmail point com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once
//---------------------------------------------------------------------------
#if 0 //ifdef _WIN32
//---------------------------------------------------------------------------
#ifdef NEW_CODECVT_CONVERTER
std::u16string& Calculate(const std::u16string& p_input, std::u16string& p_output);
#else
std::wstring& Calculate(const std::wstring& p_input, std::wstring& p_output);
#endif
//---------------------------------------------------------------------------
#endif // _WIN32
//---------------------------------------------------------------------------