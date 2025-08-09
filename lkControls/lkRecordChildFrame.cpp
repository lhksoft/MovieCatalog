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
#include "lkRecordChildFrame.h"
#include "lkRecordBar.h"

#include <wx/menu.h>
#include <wx/menuitem.h>

#if defined(__UNIX__)
#include <wx/font.h>
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// Icon bundle
#include "Form.xpms"

 /****************************************************************************************
 * lkRecordChildFrame
 ****************************************************************************************/

wxIMPLEMENT_CLASS(lkRecordChildFrame, wxDocChildFrame)

lkRecordChildFrame::lkRecordChildFrame() : wxDocChildFrame()
{
}

bool lkRecordChildFrame::Create(wxDocument* doc, wxView* view, wxFrame* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	if ( !wxDocChildFrame::Create(doc, view, parent, wxID_ANY, title, pos, size, style) )
		return false;

	SetIcons(CreateBundle_Form());
	CreateStatusBar();
	DoSetAcceleratorTable();

	Bind(wxEVT_MENU, &lkRecordChildFrame::OnCancel, this, GetID_RecCancel());
	Bind(wxEVT_MENU_HIGHLIGHT, &lkRecordChildFrame::OnMenuHighlight, this);
	Bind(wxEVT_MENU_CLOSE, &lkRecordChildFrame::OnMenuClose, this);

	// Create a standard Toolbar with record navigation
	wxToolBar* tB = CreateToolBar();
	if ( tB != NULL )
		tB->Realize();

	return true;
}

//virtual
wxStatusBar* lkRecordChildFrame::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& WXUNUSED(name))
{
	lkRecordStatusbar* sB = new lkRecordStatusbar();
	if ( !sB->Create(this, number, style, id) )
	{ // something wend wrong
		delete sB;
		sB = NULL;
	}
	return sB;
}

wxToolBar* lkRecordChildFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& WXUNUSED(name))
{
	lkRecordToolbar* tB = new lkRecordToolbar(this, id, wxDefaultPosition, wxDefaultSize, style);

	tB->AddFirst(GetID_RecFirst());
	tB->AddPrev(GetID_RecPrev());
	tB->AddNext(GetID_RecNext());
	tB->AddLast(GetID_RecLast());
	tB->AddSeparator();
	tB->AddNew(GetID_RecNew());
	tB->AddRem(GetID_RecRemove());
	tB->AddUpd(GetID_RecUpdate());

	return tB;
}

wxIconBundle lkRecordChildFrame::CreateBundle_Form()
{
	wxIconBundle bundle;
	{
		wxIcon small_icon;
		small_icon.CopyFromBitmap(wxBitmap(FORM_SMALL_XPM));
		if ( small_icon.IsOk() )
			bundle.AddIcon(small_icon);
	}

	{
		wxIcon big_icon;
		big_icon.CopyFromBitmap(wxBitmap(FORM_BIG_XPM));
		if ( big_icon.IsOk() )
			bundle.AddIcon(big_icon);
	}

	return bundle;
}

//virtual
void lkRecordChildFrame::AddAcceleratorTableEntries(wxVector<wxAcceleratorEntry>& Entries)
{
	// Bind Escape to Record_Cancel.
	Entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_ESCAPE, GetID_RecCancel()));
}

void lkRecordChildFrame::DoSetAcceleratorTable()
{
	wxVector<wxAcceleratorEntry> Entries;

	// Allow derived classes an opportunity to register accelerator keys.
	AddAcceleratorTableEntries(Entries);

	// Create the new table with these.
	SetAcceleratorTable(wxAcceleratorTable(Entries.size(), Entries.begin()));
}

// ----------------------------------------------------------------------------
// Event processors
// ----------------------------------------------------------------------------

void lkRecordChildFrame::OnCancel(wxCommandEvent& event)
{
	event.Skip();
	DoCancel();
}

//virtual
void lkRecordChildFrame::DoCancel()
{
/*
	wxDocument* d = GetDocument();
	if ( d && d->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
		(dynamic_cast<lkSQL3RecordDocument*>(d))->sql3Cancel();
*/
}

void lkRecordChildFrame::OnMenuHighlight(wxMenuEvent& event)
{
//	event.Skip();
	(void)ShowMenuHelp(event.GetMenuId(), event.GetMenu());
}

void lkRecordChildFrame::OnMenuClose(wxMenuEvent& event)
{
	event.Skip();
	DoGiveHelp(wxEmptyString, false);
}

bool lkRecordChildFrame::ShowMenuHelp(int menuId, wxMenu* mnu)
{
	if ( !mnu )
		return wxDocChildFrame::ShowMenuHelp(menuId);
	else
	{
		// if no help string found, we will clear the status bar text
		//
		// NB: wxID_NONE is used for (sub)menus themselves by wxMSW
		wxString helpString;
		if ( menuId != wxID_SEPARATOR && menuId != wxID_NONE )
		{
			const wxMenuItem* const item = mnu->FindItem(menuId);
			if ( item && !item->IsSeparator() )
				helpString = item->GetHelp();

			// notice that it's ok if we don't find the item because it might
			// belong to the popup menu, so don't assert here
		}

		DoGiveHelp(helpString, true);

		return !helpString.empty();
	}
}

// ----------------------------------------------------------------------------
// Overrides
// ----------------------------------------------------------------------------

wxWindowID lkRecordChildFrame::GetID_RecFirst() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecFirst();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecPrev() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecPrev();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecNext() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecNext();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecLast() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecLast();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecNew() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecNew();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecRemove() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecRemove();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecUpdate() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecUpdate();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordChildFrame::GetID_RecCancel() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecCancel();
	// else
	return wxID_ANY;
}

void lkRecordChildFrame::SetCurrentRecord(wxUint64 u)
{
	wxStatusBar* bar = GetStatusBar();
	if ( bar && bar->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		(dynamic_cast<lkRecordStatusbar*>(bar))->SetCurrentRecord(u);
}
void lkRecordChildFrame::SetTotalRecords(wxUint64 u)
{
	wxStatusBar* bar = GetStatusBar();
	if ( bar && bar->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		(dynamic_cast<lkRecordStatusbar*>(bar))->SetTotalRecords(u);
}

