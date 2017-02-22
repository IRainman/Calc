/*
 * Copyright 2009-2017 Solomin Alexey Leonovich, a.rainman on gmail point com
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

#ifndef MYTYPES_H
#define MYTYPES_H

//---------------------------------------------------------------------------
typedef long double calc_variable;
#define calc_input_function strtold
//---------------------------------------------------------------------------
#define CALC_BUFFER_SIZE 256
//---------------------------------------------------------------------------
#define CALC_INTERNAL_ACCURACY_FORMAT "%.100g"
//---------------------------------------------------------------------------
// TODO: Add variable accuracy
#define CALC_OUTPUT_ACCURACY_FORMAT "%.15g"
//const calc_variable calc_output_accuracy_mult = 1.e150L;
//---------------------------------------------------------------------------
#endif// MYTYPES_H
