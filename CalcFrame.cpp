///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pch.hpp"

#include "CalcFrame.h"

///////////////////////////////////////////////////////////////////////////

CalcFrame::CalcFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 292,357 ), wxDefaultSize );
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	m_textCtrlInput = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_CHARWRAP|wxVSCROLL );
	bSizer->Add( m_textCtrlInput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_buttonEnter = new wxButton( this, wxID_ANY, wxT("Enter"), wxDefaultPosition, wxDefaultSize, 0 );

	m_buttonEnter->SetDefault();
	bSizer->Add( m_buttonEnter, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_textCtrlOutput = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
	bSizer->Add( m_textCtrlOutput, 0, wxALIGN_RIGHT|wxALL, 5 );


	this->SetSizer( bSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CalcFrame::CalcFrameOnClose ) );
	m_textCtrlInput->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CalcFrame::m_textCtrlInputOnTextEnter ), NULL, this );
	m_buttonEnter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CalcFrame::m_buttonEnterOnButtonClick ), NULL, this );
}

CalcFrame::~CalcFrame()
{
}
