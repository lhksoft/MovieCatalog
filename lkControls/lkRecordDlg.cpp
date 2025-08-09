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
#include "lkRecordDlg.h"
#include "lkRecordBar.h"
#include <wx/msgdlg.h> 

wxIMPLEMENT_CLASS(lkRecordDlg, lkDialogEx)

lkRecordDlg::lkRecordDlg() : lkDialogEx()
{
	m_bChangedAny = false;
	m_bChanged = false;
	m_bInitial = false;
}
lkRecordDlg::~lkRecordDlg()
{
}

bool lkRecordDlg::Create(wxWindow* parent, const wxImage& background, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	wxBitmap bmp;
	if ( background.IsOk() )
		bmp = wxBitmap(background);

	return lkRecordDlg::Create(parent, bmp, title, pos, size, style);
}
bool lkRecordDlg::Create(wxWindow* parent, const wxBitmap& background, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	if ( !lkDialogEx::Create(parent, background, wxID_ANY, title, pos, size, style) )
		return false;

	CreateStatusBar();
	DoSetAcceleratorTable();

	// Create a standard Toolbar with record navigation
	wxToolBar* tB = CreateToolBar();
	if ( tB != NULL )
		tB->Realize();

	BindButtons();

	return true;
}

//virtual
wxStatusBar* lkRecordDlg::OnCreateStatusBar(int number, long style, wxWindowID id)
{
	lkRecordStatusbar* sB = new lkRecordStatusbar();
	if ( !sB->Create(this, number, style, id) )
	{ // something wend wrong
		delete sB;
		sB = NULL;
	}
	return sB;
}

//virtual
wxToolBar* lkRecordDlg::OnCreateToolBar(long style, wxWindowID id, const wxString& WXUNUSED(name))
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

bool lkRecordDlg::AnythingChanged() const
{
	return m_bChangedAny;
}
bool lkRecordDlg::HasChanged() const
{
	return m_bChanged;
}
void lkRecordDlg::ResetChanged()
{
	m_bChanged = false;
}
void lkRecordDlg::SetChanged()
{
	m_bChangedAny = m_bChanged = true;
}

//virtual
void lkRecordDlg::BindButtons()
{
	Bind(wxEVT_MENU, &lkRecordDlg::OnEscape, this, GetID_RecCancel());
	Bind(wxEVT_MENU_HIGHLIGHT, &lkRecordDlg::OnMenuHighlight, this);
	Bind(wxEVT_MENU_CLOSE, &lkRecordDlg::OnMenuClose, this);

	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordFirst, this, GetID_RecFirst());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordPrev, this, GetID_RecPrev());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordNext, this, GetID_RecNext());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordLast, this, GetID_RecLast());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordAdd, this, GetID_RecNew());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordDelete, this, GetID_RecRemove());
	Bind(wxEVT_UPDATE_UI, &lkRecordDlg::OnUpdateRecordUpdate, this, GetID_RecUpdate());

	Bind(wxEVT_TOOL, &lkRecordDlg::OnMove, this, GetID_RecFirst());
	Bind(wxEVT_TOOL, &lkRecordDlg::OnMove, this, GetID_RecPrev());
	Bind(wxEVT_TOOL, &lkRecordDlg::OnMove, this, GetID_RecNext());
	Bind(wxEVT_TOOL, &lkRecordDlg::OnMove, this, GetID_RecLast());

	Bind(wxEVT_TOOL, &lkRecordDlg::OnAdd, this, GetID_RecNew());
	Bind(wxEVT_TOOL, &lkRecordDlg::OnUpdate, this, GetID_RecUpdate());
	Bind(wxEVT_TOOL, &lkRecordDlg::OnDelete, this, GetID_RecRemove());

	Bind(wxEVT_INIT_DIALOG, &lkRecordDlg::OnInitDialog, this);
	Bind(wxEVT_CHAR_HOOK, &lkRecordDlg::OnCharHook, this);
	Bind(wxEVT_BUTTON, &lkRecordDlg::OnButton, this);
}

//virtual
void lkRecordDlg::AddAcceleratorTableEntries(wxVector<wxAcceleratorEntry>& Entries)
{
	// Bind Escape to Record_Cancel.
	Entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_ESCAPE, GetID_RecCancel()));
	// we also must overwrite OnCharHook (see above) to intercept normal behavior of wxDialog
}

void lkRecordDlg::DoSetAcceleratorTable()
{
	wxVector<wxAcceleratorEntry> Entries;

	// Allow derived classes an opportunity to register accelerator keys.
	AddAcceleratorTableEntries(Entries);

	// Create the new table with these.
	SetAcceleratorTable(wxAcceleratorTable(Entries.size(), Entries.begin()));
}

///virtual
bool lkRecordDlg::IsOk() const
{
	return false;
}

//virtual
bool lkRecordDlg::UpdateData(bool bSaveAndValidate)
{
	if ( bSaveAndValidate )
	{
		bool bV = Validate();

		if ( bV )
		{
			if ( TransferDataFromWindow() )
				return DoUpdate();
		}
		//else..
		return false; // no good
	}
	// else
	TransferDataToWindow();
	UpdateRecordStatusbar();
	return true;
}

//virtual
wxString lkRecordDlg::GetDefaultFilter() const
{
	return wxEmptyString;
}
//virtual
wxString lkRecordDlg::GetDefaultOrder() const
{
	return wxEmptyString;
}

//virtual
bool lkRecordDlg::CanAppend()
{
	return false;
}
//virtual
bool lkRecordDlg::CanRemove()
{
	return false;
}

//virtual
void lkRecordDlg::DoEscape()
{
}

//virtual
bool lkRecordDlg::DoDelete()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdate()
{
	return true;
}
//virtual
bool lkRecordDlg::DoAdd()
{
	DoThingsBeforeAdd();
	return false;
}

//virtual
bool lkRecordDlg::DoMove(lkRecordDlg::MoveIDs nIDMoveCommand)
{
	if ( !IsOk() )
		return false;

#if 0
	switch ( nIDMoveCommand )
	{
		case RecordFirst:
			pSet->MoveFirst();
			break;

		case RecordPrev:
			pSet->MovePrev();
			break;

		case RecordNext:
			pSet->MoveNext();
			break;

		case RecordLast:
			pSet->MoveLast();
			break;

		default:
			// Unexpected case value
			pSet->Move(0);
			break;
	}

	// Show results of move operation
	UpdateData(false);
#endif
	return true;
}

//virtual
void lkRecordDlg::DoThingsBeforeUpdate()
{
}
//virtual
void lkRecordDlg::DoThingsBeforeAdd()
{
}

bool lkRecordDlg::ConfirmDelete()
{
	wxString msg = wxT("This action will permanently destroy current record.\nAre you sure you want to DELETE current record ?");
	wxString ttl = wxT("Confirm Removal");

	bool bRet = (wxMessageBox(msg, ttl, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxYES);

	return bRet;
}

//virtual
void lkRecordDlg::UpdateRecordStatusbar()
{
#if 0
	lkSQL3RecordSet* pSet = (dynamic_cast<lkSQL3RecordDocument*>(d))->GetRecordset();
	if ( pSet )
	{
		wxUint64 u = pSet->IsAdding() ? 1 : 0;
		SetCurrentRecord(pSet->GetCurRow() + u);
		SetTotalRecords(pSet->GetRowCount() + u);
	}
#endif
}


//virtual
bool lkRecordDlg::DoUpdateRecordFirst()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordPrev()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordNext()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordLast()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordNew()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordRemove()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordUpdate()
{
	return false;
}

//virtual
bool lkRecordDlg::DoUpdateRecordFind()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordFindNext()
{
	return false;
}
//virtual
bool lkRecordDlg::DoUpdateRecordFindPrev()
{
	return false;
}

bool lkRecordDlg::ShowMenuHelp(int menuId, wxMenu* mnu)
{
	if ( !mnu )
		return lkDialogEx::ShowMenuHelp(menuId);
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

wxWindowID lkRecordDlg::GetID_RecFirst() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecFirst();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecPrev() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecPrev();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecNext() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecNext();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecLast() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecLast();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecNew() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecNew();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecRemove() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecRemove();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecUpdate() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecUpdate();
	// else
	return wxID_ANY;
}
wxWindowID lkRecordDlg::GetID_RecCancel() const
{
	wxStatusBar* pS = GetStatusBar();
	if ( pS && pS->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		return (dynamic_cast<lkRecordStatusbar*>(pS))->GetID_RecCancel();
	// else
	return wxID_ANY;
}

void lkRecordDlg::SetCurrentRecord(wxUint64 u)
{
	wxStatusBar* bar = GetStatusBar();
	if ( bar && bar->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		(dynamic_cast<lkRecordStatusbar*>(bar))->SetCurrentRecord(u);
}
void lkRecordDlg::SetTotalRecords(wxUint64 u)
{
	wxStatusBar* bar = GetStatusBar();
	if ( bar && bar->IsKindOf(wxCLASSINFO(lkRecordStatusbar)) )
		(dynamic_cast<lkRecordStatusbar*>(bar))->SetTotalRecords(u);
}


// ----------------------------------------------------------------------------
// Event processors
// ----------------------------------------------------------------------------

void lkRecordDlg::OnUpdateRecordFirst(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordFirst());
}
void lkRecordDlg::OnUpdateRecordPrev(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordPrev());
}
void lkRecordDlg::OnUpdateRecordNext(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordNext());
}
void lkRecordDlg::OnUpdateRecordLast(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordLast());
}
void lkRecordDlg::OnUpdateRecordAdd(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordNew());
}
void lkRecordDlg::OnUpdateRecordDelete(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordRemove());
}
void lkRecordDlg::OnUpdateRecordUpdate(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordUpdate());
}

void lkRecordDlg::OnUpdateFind(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordFind());
}
void lkRecordDlg::OnUpdateFindNext(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordFindNext());
}
void lkRecordDlg::OnUpdateFindPrev(wxUpdateUIEvent& event)
{
	event.Enable(DoUpdateRecordFindPrev());
}

void lkRecordDlg::OnInitDialog(wxInitDialogEvent& event)
{
	m_bInitial = true;
	lkDialogEx::OnInitDialog(event);
	m_bInitial = false;
//	event.Skip();
}

void lkRecordDlg::OnMove(wxCommandEvent& event)
{
	MoveIDs move_id;

	if ( event.GetId() == GetID_RecFirst() )
		move_id = MoveIDs::RecordFirst;
	else if ( event.GetId() == GetID_RecPrev() )
		move_id = MoveIDs::RecordPrev;
	else if ( event.GetId() == GetID_RecNext() )
		move_id = MoveIDs::RecordNext;
	else if ( event.GetId() == GetID_RecLast() )
		move_id = MoveIDs::RecordLast;
	else
		move_id = MoveIDs::RecordINVALID;

	if ( move_id != MoveIDs::RecordINVALID )
	{
		if ( UpdateData(true) )
			DoMove(move_id);
	}
}


void lkRecordDlg::OnAdd(wxCommandEvent& event)
{
	if ( !IsOk() || !UpdateData(true) )
	{
		// event.Skip();
		return;
	}

	DoAdd();
}

void lkRecordDlg::OnUpdate(wxCommandEvent& event)
{
	if ( UpdateData(true) )
		UpdateData(false);
}

void lkRecordDlg::OnDelete(wxCommandEvent& event)
{
	if ( CanRemove() && ConfirmDelete() )
		DoDelete();
}


void lkRecordDlg::OnEscape(wxCommandEvent& event)
{
	event.Skip();
	DoEscape();
}

void lkRecordDlg::OnMenuHighlight(wxMenuEvent& event)
{
	//	event.Skip();
	(void)ShowMenuHelp(event.GetMenuId(), event.GetMenu());
}

void lkRecordDlg::OnMenuClose(wxMenuEvent& event)
{
	event.Skip();
	DoGiveHelp(wxEmptyString, false);
}

void lkRecordDlg::OnCharHook(wxKeyEvent& event)
{
	if ( IsEscapeKey(event) )
	{

		wxCommandEvent evt(wxEVT_MENU, GetID_RecCancel());
		evt.SetEventObject(this);
		ProcessWindowEvent(evt);
		// Skip the call to event.Skip() below, we did handle this key.
		return;
	}

	event.Skip();
}

void lkRecordDlg::OnButton(wxCommandEvent& event)
{
	const int id = event.GetId();
	if ( id == GetAffirmativeId() )
	{
		if (UpdateData(true) )
			EndDialog(m_affirmativeId);
	}
	else if ( id == wxID_APPLY )
	{
		UpdateData(true);
		// TODO: disable the Apply button until things change again
		event.Skip(); // maybe in a future release this will be handled, so skip the event. This code was written using wx version 3.1.5
	}
	else
	{
		event.Skip();
	}
}
