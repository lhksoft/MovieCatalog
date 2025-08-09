/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "lkRecordBar.h"

#include "lkDialogEx.h"
#include <wx/frame.h>

#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>

#if defined(__UNIX__)
#include <wx/font.h>
#endif


// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// Statusbar buttons
#include "lkRecordButtons.xpms"
// Toolbar buttons
#include "rec_navigation.xpms"
#include "rec_operation.xpms"

/****************************************************************************************
 * lkRecordStatusbar
 ****************************************************************************************/

wxIMPLEMENT_ABSTRACT_CLASS(lkRecordStatusbar, wxStatusBar);

lkRecordStatusbar::lkRecordStatusbar() : wxStatusBar()
{
	m_recCurrent = NULL;
	m_recTotal = NULL;

#if defined(__UNIX__)
	wxFont defaultFont(*wxSMALL_FONT);
	SetFont(defaultFont);
#endif
}
lkRecordStatusbar::~lkRecordStatusbar()
{
	//	Unref_IDs();
}

//virtual
void lkRecordStatusbar::Init_IDs()
{
	mID_RecFirst = wxWindow::NewControlId(1);
	mID_RecPrev = wxWindow::NewControlId(1);
	mID_RecNext = wxWindow::NewControlId(1);
	mID_RecLast = wxWindow::NewControlId(1);
	mID_RecNew = wxWindow::NewControlId(1);
	mID_RecRemove = wxWindow::NewControlId(1);
	mID_RecUpdate = wxWindow::NewControlId(1);
	mID_RecCancel = wxWindow::NewControlId(1);
}

//virtual
int* lkRecordStatusbar::GetWidths(int number)
{
	if ( number <= 0 )
		return NULL;

	int* widths = new int[static_cast<size_t>(number)];
	for ( int i = 0; i < number; i++ )
		widths[i] = -1; // variable

	return widths;
}

bool lkRecordStatusbar::Create(wxWindow* pParent, int number, long style, wxWindowID id)
{
	if ( !wxStatusBar::Create(pParent, id, style) )
		return false;


	Init_IDs();

	if ( pParent && pParent->IsKindOf(wxCLASSINFO(wxFrame)) )
		(dynamic_cast<wxFrame*>(pParent))->SetStatusBarPane((number > 0) ? number : -1);
	else if ( pParent && pParent->IsKindOf(wxCLASSINFO(lkDialogEx)) )
		(dynamic_cast<lkDialogEx*>(pParent))->SetStatusBarPane((number > 0) ? number : -1);

	int t = number + 1;
	int r = number - 1;
	if ( r < 0 ) r = 0;
	SetFieldsCount(t);

	int* widths = GetWidths(t);
	wxASSERT(widths != NULL);
	if ( widths == NULL )
		return false;

	widths[r] = 430/*380*/; // fixed

	SetStatusWidths(t, widths);
#ifdef __WXMSW__
    SetMinHeight(15);
#else
    SetMinHeight(25);
#endif // __WXMSW__

	wxSize btnSize = wxSize(27, 21);
	long btnFlags = wxBU_NOTEXT | wxBU_AUTODRAW | wxBU_ALIGN_MASK;

	wxBitmapButton* pBut;
	long statFlags = wxST_NO_AUTORESIZE | wxFULL_REPAINT_ON_RESIZE/* | wxCLIP_CHILDREN*/;
	wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
	szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Record :"), wxPoint(-1, 3), wxSize(51, -1), statFlags | wxALIGN_RIGHT), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 1);
	szHor->Add(pBut = new wxBitmapButton(this, mID_RecFirst, wxBitmap(RECNAV_FIRSTU_XPM), wxDefaultPosition, btnSize, btnFlags), 0, wxALIGN_CENTER_VERTICAL, 0);
	pBut->SetBitmapSelected(wxBitmap(RECNAV_FIRSTD_XPM));
	pBut->SetBitmapDisabled(wxBitmap(RECNAV_FIRSTX_XPM));
	szHor->Add(pBut = new wxBitmapButton(this, mID_RecPrev, wxBitmap(RECNAV_PREVU_XPM), wxDefaultPosition, btnSize, btnFlags), 0, wxALIGN_CENTER_VERTICAL, 0);
	pBut->SetBitmapSelected(wxBitmap(RECNAV_PREVD_XPM));
	pBut->SetBitmapDisabled(wxBitmap(RECNAV_PREVX_XPM));
	szHor->Add(m_recCurrent = new wxStaticText(this, wxID_ANY, wxT("12345 "), wxPoint(-1, 3), wxSize(61, -1), statFlags | wxALIGN_CENTER), 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL | wxFIXED_MINSIZE, 1);
	szHor->Add(pBut = new wxBitmapButton(this, mID_RecNext, wxBitmap(RECNAV_NEXTU_XPM), wxDefaultPosition, btnSize, btnFlags), 0, wxALIGN_CENTER_VERTICAL, 0);
	pBut->SetBitmapSelected(wxBitmap(RECNAV_NEXTD_XPM));
	pBut->SetBitmapDisabled(wxBitmap(RECNAV_NEXTX_XPM));
	szHor->Add(pBut = new wxBitmapButton(this, mID_RecLast, wxBitmap(RECNAV_LASTU_XPM), wxDefaultPosition, btnSize, btnFlags), 0, wxALIGN_CENTER_VERTICAL, 0);
	pBut->SetBitmapSelected(wxBitmap(RECNAV_LASTD_XPM));
	pBut->SetBitmapDisabled(wxBitmap(RECNAV_LASTX_XPM));
	szHor->Add(pBut = new wxBitmapButton(this, mID_RecNew, wxBitmap(RECNAV_NEWU_XPM), wxDefaultPosition, btnSize, btnFlags), 0, wxALIGN_CENTER_VERTICAL, 0);
	pBut->SetBitmapSelected(wxBitmap(RECNAV_NEWD_XPM));
	pBut->SetBitmapDisabled(wxBitmap(RECNAV_NEWX_XPM));
	szHor->Add(m_recTotal = new wxStaticText(this, wxID_ANY, wxT("of %I64u records."), wxPoint(-1, 3), wxSize(130,-1), statFlags | wxALIGN_LEFT), 0, wxLEFT | wxALIGN_CENTER_VERTICAL | wxFIXED_MINSIZE, 1);

	SetSizer(szHor);
	Layout();
	delete[]widths;
	
	if ( number > 0 )
		SetStatusText(wxT("Welcome to wxWidgets!"), number);

	Bind(wxEVT_BUTTON, &lkRecordStatusbar::OnRecordButton, this, mID_RecFirst);
	Bind(wxEVT_BUTTON, &lkRecordStatusbar::OnRecordButton, this, mID_RecPrev);
	Bind(wxEVT_BUTTON, &lkRecordStatusbar::OnRecordButton, this, mID_RecNext);
	Bind(wxEVT_BUTTON, &lkRecordStatusbar::OnRecordButton, this, mID_RecLast);
	Bind(wxEVT_BUTTON, &lkRecordStatusbar::OnRecordButton, this, mID_RecNew);

	return true;
}

wxWindowID lkRecordStatusbar::GetID_RecFirst() const
{
	return mID_RecFirst;
}
wxWindowID lkRecordStatusbar::GetID_RecPrev() const
{
	return mID_RecPrev;
}
wxWindowID lkRecordStatusbar::GetID_RecNext() const
{
	return mID_RecNext;
}
wxWindowID lkRecordStatusbar::GetID_RecLast() const
{
	return mID_RecLast;
}
wxWindowID lkRecordStatusbar::GetID_RecNew() const
{
	return mID_RecNew;
}
wxWindowID lkRecordStatusbar::GetID_RecRemove() const
{
	return mID_RecRemove;
}
wxWindowID lkRecordStatusbar::GetID_RecUpdate() const
{
	return mID_RecUpdate;
}
wxWindowID lkRecordStatusbar::GetID_RecCancel() const
{
	return mID_RecCancel;
}


// ----------------------------------------------------------------------------
// Overrides
// ----------------------------------------------------------------------------

//virtual
void lkRecordStatusbar::SetStatusText(const wxString& text, int i)
{
	wxStatusBar::SetStatusText(text, i);
}

// ----------------------------------------------------------------------------
// Member Operations
// ----------------------------------------------------------------------------

void lkRecordStatusbar::SetCurrentRecord(wxUint64 u)
{
	if ( m_recCurrent && (m_recCurrent->GetHandle() != 0) )
	{
		wxLongLong ll = static_cast<wxLongLong>(u);
		wxString s = ll.ToString();
		// s.Printf(wxT("%llu"), u);
		m_recCurrent->SetLabel(s);
	}
}
void lkRecordStatusbar::SetTotalRecords(wxUint64 u)
{
	if ( m_recTotal && (m_recTotal->GetHandle() != 0) )
	{
		wxString s;
		s.Printf(wxT("of %llu records."), u);
		m_recTotal->SetLabel(s);
	}
}

void lkRecordStatusbar::OnRecordButton(wxCommandEvent& event)
{
	wxCommandEvent newEvent(wxEVT_TOOL, event.GetId());
	newEvent.SetEventObject(this);
	newEvent.SetClientData(GetParent());
	ProcessWindowEvent(newEvent);
}

/****************************************************************************************
 * lkRecordToolbar -- Class definition
 ****************************************************************************************/
wxIMPLEMENT_DYNAMIC_CLASS(lkRecordToolbar, wxToolBar)

lkRecordToolbar::lkRecordToolbar() : wxToolBar()
{
}
lkRecordToolbar::lkRecordToolbar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxToolBar(parent, id, pos, size, style)
{
}
lkRecordToolbar::~lkRecordToolbar()
{
}

void lkRecordToolbar::AddFirst(wxWindowID id)
{
	AddTool(id, wxT("First"), wxBitmap(REC_FIRST_XPM), wxT("First Record"));
}
void lkRecordToolbar::AddPrev(wxWindowID id)
{
	AddTool(id, wxT("Previous"), wxBitmap(REC_PREV_XPM), wxT("Previous Record"));
}
void lkRecordToolbar::AddNext(wxWindowID id)
{
	AddTool(id, wxT("Next"), wxBitmap(REC_NEXT_XPM), wxT("Next Record"));
}
void lkRecordToolbar::AddLast(wxWindowID id)
{
	AddTool(id, wxT("Last"), wxBitmap(REC_LAST_XPM), wxT("Last Record"));
}
void lkRecordToolbar::AddNew(wxWindowID id)
{
	AddTool(id, wxT("New"), wxBitmap(REC_NEW_XPM), wxT("New Record"));
}
void lkRecordToolbar::AddRem(wxWindowID id)
{
	AddTool(id, wxT("Delete"), wxBitmap(REC_DEL_XPM), wxT("Remove Record"));
}
void lkRecordToolbar::AddUpd(wxWindowID id)
{
	AddTool(id, wxT("Update"), wxBitmap(REC_UPDATE_XPM), wxT("Update Record"));
}

