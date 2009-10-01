#include "stdafx.h"
#include "Calculator.h"
//TODO #define _USE_MATH_DEFINES
#include <math.h>
#include <stack>
#include <list>
#include "MyTypes.h"

list <string::size_type> c_coordinate;
//static string m_buf;
static string::size_type start;//, end/*, point*/;
static wstring m_ErrorString;

//static double a, b;

static const uint_8 c_err_ok =	0;
static const uint_8 c_err_end =	1;
//static const uint_8 c_err_nf =	2;
static const uint_8 c_err_emp =	4;
//static const uint_8 c_err_arg =	8;

static const uint_8 priority_function =	16;
static const uint_8 priority_power =	8;
static const uint_8 priority_multiply =	4;
static const uint_8 priority_addition =	2;
static const uint_8 priority_bracket =	1;
static const uint_8 priority_default =	0;
static const uint_8 priority_error =	-1;

void AddError(uint_8 p_message, string::size_type p_count = -1, string::size_type p_1 = 0, string::size_type p_2 = 0)
{
	wstring l_error;
	wchar_t l_1[6];
	_itow_s(p_1, l_1, 10);
	wchar_t l_2[6];
	_itow_s(p_2, l_2, 10);
	switch(p_message)
	{
		case 0:
			l_error = L"Недопустимый символ";
			break;
		case 1:
			l_error = (wstring)L"Количество открывающих " + (wstring)l_1 +
				(wstring)L" и закрывающих " + (wstring)l_2 +
				(wstring)L" скобок не совпадает";
			break;
		case 2:
			l_error = L"Недопустимый символ";
			break;
		case 3:
			l_error = L"Недопустимый символ";
			break;
		case 4:
			l_error = L"Недопустимый символ";
			break;
	}
	TCHAR l_wchar_buf[1024];
	if(p_count == -1)
	{
		swprintf_s(l_wchar_buf, L"Ошибка: %s\r\n", l_error.c_str());
	}
	else
	{
		swprintf_s(l_wchar_buf, L"В позиции %d ошибка: %s\r\n", p_count, l_error.c_str());
	}
	m_ErrorString += l_wchar_buf;
}

uint_8 GetPriority(char p_sym)
{
	switch(p_sym)
	{
		case 'P':
			return priority_function;

		case '^':
			return priority_power;

		case '*':
		case '/':
			return priority_multiply;

		case '+':
		case '-':
			return priority_addition;

		case '(':
		case ')':
			return priority_bracket;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '=':
		case '.':
		case ' ':
			return priority_default;

		default:
			return priority_error;
	}
}

uint_8 replace(string& p_input_str, string p_in, char p_out)
{
	string::size_type l_start, l_end;
	string l_buf;
	char l_char_buf[200];
	float l_a, l_b;
	while(true)
	{
		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return c_err_ok;

		l_buf = p_input_str;
		l_buf.erase(0, l_start);

		l_end = l_buf.find(")");
		if(l_end == string::npos)
		{
			start = l_start;
			return c_err_end;
		}

		l_buf = l_buf.substr(p_in.size() + 1, l_end - p_in.size() - 1);
		if(l_buf.empty())
		{
			start = l_start;
			return c_err_emp;
		}

		sscanf_s(l_buf.c_str(), "%f,%f", &l_a, &l_b);
		sprintf_s(l_char_buf, " %f%c%f ", l_a, p_out, l_b);
		p_input_str.erase(l_start, l_start + l_end + 1);
		p_input_str.insert(l_start, l_char_buf);
	}
}
/*
void replace(string& p_input_str, string p_in, string p_out)
{
	string::size_type l_start;
	while(true)
	{
		l_start = p_input_str.find(p_in);

		if(l_start == string::npos)
		{
			break;
		}
		else
		{
			p_input_str.erase(l_start, p_in.size() - l_start);
			p_input_str.insert(l_start, p_out);
		}
	}
}
*/
bool ValidateAndPrepareInputString(string& p_input_str)
{
	uint count_inp = 0, count_outp = 0;
	string::size_type c;
	while(true)
	{
		c = p_input_str.find(" ");
		if(c != string::npos)
			p_input_str.erase(c, 1);
		else
			break;
	}
	for(string::size_type l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		if( (p_input_str.c_str()[l_count] >= '(' && p_input_str.c_str()[l_count] <= '9') ||
			(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z') ||
			p_input_str.c_str()[l_count] == '=' || p_input_str.c_str()[l_count] == '^' )
		{
			if(p_input_str.c_str()[l_count] == '(')
			{
				count_inp++;
				continue;
			}
			if(p_input_str.c_str()[l_count] == ')')
			{
				count_outp++;
				continue;
			}
		}
		else
		{
			AddError(0, l_count);
			return false;
		}
	}
	if(count_inp != count_outp)
	{
		AddError(1, -1, count_inp, count_outp);
		return false;
	}

	replace(p_input_str, "pow", 'P');

	return true;
}

/*
uint_8 find(const string& inp, const string& st, const string& en) {
	start = inp.find(st);
	if(start == string::npos)
		return c_err_nf;

	end = inp.find(en);
	if(end == string::npos)
		return c_err_end;

	m_buf = inp.substr(start + st.size(), end - (start + st.size()));
	if(m_buf.empty())
		return c_err_emp;

	return c_err_ok;
}
*/
bool CalculateLineExpression(const string& p_input_str, string& p_output_str)
{
	uint_8 l_current_prioritet, l_old_prioritet = priority_error;
	stack <char> c_operations;
	if(!c_coordinate.empty())
	{
		c_coordinate.erase(c_coordinate.begin());
	}
	for(string::size_type l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		l_current_prioritet = GetPriority(p_input_str.c_str()[l_count]);
		if(l_current_prioritet == priority_error)
		{
			AddError(l_count, 0);
			return false;
		}
		if(l_current_prioritet)
		{
			if(l_current_prioritet == l_old_prioritet)
			{
				AddError(l_count, 1);
				return false;
			}
			if(c_operations.empty())
			{
				/*
				a)	если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				c_operations.push(p_input_str.c_str()[l_count]);
				p_output_str += " ";
			}
			else
			{
				if(l_current_prioritet != priority_bracket)
				{
					do
					{
						if(GetPriority(c_operations.top()) >= l_current_prioritet)
						{
							/*
							b)	опеpация выталкивает из стека все опеpации с большим или pавным пpиоpитетом в выходную стpоку;
							*/
							p_output_str += c_operations.top();
							c_coordinate.push_back(p_output_str.size());
							c_operations.pop();
							if(c_operations.empty())
							{
								c_operations.push(p_input_str.c_str()[l_count]);
								p_output_str += " ";
								break;
							}
						}
						else
						{
							c_operations.push(p_input_str.c_str()[l_count]);
							p_output_str += " ";
							break;
						}
					}
					while(!c_operations.empty());
				}
				else
				{
					if(p_input_str.c_str()[l_count] != ')')
					{
						/*
						c)	если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						c_operations.push(p_input_str.c_str()[l_count]);
						p_output_str += " ";
					}
					else
					{
						while(!c_operations.empty())
						{
							/*
							d)	закpывающая кpуглая скобка выталкивает все опеpации из стека до ближайшей откpывающей скобки,
							сами скобки в выходную стpоку не пеpеписываются, а уничтожают дpуг дpуга. 
							*/
							if(GetPriority(c_operations.top()) == priority_bracket && c_operations.top() != ')')
							{
								c_operations.pop();
								break;
							}
							else
							{
								p_output_str += c_operations.top();
								c_coordinate.push_back(p_output_str.size());
								c_operations.pop();
								p_output_str += " ";
							}
						}
					}
				}
			}
		}
		else
		{
			p_output_str += p_input_str.c_str()[l_count];
		}
	}
	while(!c_operations.empty())
	{
		p_output_str += c_operations.top();
		c_coordinate.push_back(p_output_str.size());
		c_operations.pop();
	}
	return true;
}
void Calculate(string& p_to_process_str)
{
	string input = p_to_process_str;
	p_to_process_str = "";
	char l_char_buf[200];
	float a, b;
	string::size_type l_count = 0;
	while(l_count < input.size())
	{
		sscanf_s(input.c_str(), "%f %f", &a, &b);
		if(!c_coordinate.empty())
		{
			input.erase(l_count, c_coordinate.front() - 1);
			c_coordinate.pop_front();
		}
		for(l_count = 0; l_count < input.size(); l_count++)
		{
			switch(input.c_str()[l_count])
			{
				case 'P':
					sprintf_s(l_char_buf, " %f ", pow(a,b));
					p_to_process_str += l_char_buf;
					break;
				case '^':
					sprintf_s(l_char_buf, " %f ", (int)a^(int)b);
					p_to_process_str += l_char_buf;
					break;
				case '*':
					sprintf_s(l_char_buf, " %f ", a*b);
					p_to_process_str += l_char_buf;
					break;
				case '/':
					if(!b)
					{
						p_to_process_str = "Нельзя делить на ноль";
						return;
					}
					sprintf_s(l_char_buf, " %f ", a/b);
					p_to_process_str += l_char_buf;
					break;
				case '+':
					sprintf_s(l_char_buf, " %f ", a+b);
					p_to_process_str += l_char_buf;
					break;
				case '-':
					sprintf_s(l_char_buf, " %f ", a-b);
					p_to_process_str += l_char_buf;
					break;
#ifdef _DEBUG
				default:
					p_to_process_str = "Otput processing error!";
					break;
#endif
			}
		}
	}
}

void Analize(wstring p_input, wstring& p_output, wstring& p_ErrorString)
{
	m_ErrorString = L"";
	bool l_No_Error = true;

	static string l_input_str, l_output_str, l_ErrorString;
	l_input_str.assign(p_input.begin(), p_input.end());
	l_output_str = "";

	if(l_No_Error)
		l_ErrorString = "\tFunction:\r\n" + l_input_str + "\r\n";

	if(l_No_Error)
	{
		l_No_Error &= ValidateAndPrepareInputString(l_input_str);
		if(l_No_Error)
		{
			l_ErrorString += "\tPreparing:\r\n" + l_input_str + "\r\n";
			CalculateLineExpression(l_input_str, l_output_str);
			if(l_No_Error)
			{
				l_ErrorString += "\tRPN:\r\n" + l_output_str + "\r\n";
				string temp = l_output_str;
				Calculate(l_output_str);
			}
		}
	}

	if(l_No_Error)
	{
		p_ErrorString.assign(l_ErrorString.begin(), l_ErrorString.end());
		p_ErrorString += L"\r\nПреобразование выполнено успешно";

		p_output = L"";
		p_output.assign(l_output_str.begin(), l_output_str.end());
	}
	else
	{
		p_ErrorString = L"";
		p_ErrorString.assign(m_ErrorString.begin(), m_ErrorString.end());
	}
}
