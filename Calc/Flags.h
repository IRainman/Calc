/*
 * Copyright 2009-2010 Solomin Alexey Leonovich, a.rainman on gmail point com
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

#ifndef _FLAGS_H
#define _FLAGS_H
//---------------------------------------------------------------------------
// TODO: Add support for all blocks and delete this header from project
//---------------------------------------------------------------------------
// Deprecated because is very slow and get high value of cyclomatic complexity https://en.wikipedia.org/wiki/Cyclomatic_complexity
//#define ENABLE_INPUT_VALIDATION // Deprecated

//#define NEW_CODECVT_CONVERTER // TODO fix me, I don't know how 

#define ENABLE_LOG
#ifdef ENABLE_LOG
// TODO
//#define ENABLE_WARNINGS_IN_LOG
//#define EXTENDENT_REPORT_OF_POSITION_IN_LOG
#ifdef _DEBUG
//#define ENABLE_LOG_DEBUG
#endif // _DEBUG
#endif // ENABLE_LOG
//---------------------------------------------------------------------------
#endif