///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class CalcFrame
///////////////////////////////////////////////////////////////////////////////
class CalcFrame : public wxFrame
{
	private:

	protected:
		wxTextCtrl* m_textCtrlInput;
		wxButton* m_buttonEnter;
		wxTextCtrl* m_textCtrlOutput;

		// Virtual event handlers, override them in your derived class
		void CalcFrameOnClose( wxCloseEvent& event );
		void m_textCtrlInputOnTextEnter( wxCommandEvent& event );
		void m_buttonEnterOnButtonClick( wxCommandEvent& event );


	public:

		CalcFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Calc"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 292,357 ), long style = wxCAPTION|wxDEFAULT_FRAME_STYLE|wxSYSTEM_MENU );

		~CalcFrame();

};

