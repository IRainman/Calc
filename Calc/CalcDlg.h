/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
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

class CCalcDlg : public CDialog
{
	public:
		CCalcDlg(CWnd* pParent = nullptr) noexcept;
		
		enum { IDD = IDD_CALC_DIALOG };
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		
	protected:
		HICON m_hIcon;//-V122_NOPTR
		
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg HCURSOR OnQueryDragIcon() noexcept;
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnEnChangeEditInput();
	private:
		static constexpr std::string::size_type c_max_edit_input_size = 4096;
		std::string m_input;
		std::string m_result;
};
