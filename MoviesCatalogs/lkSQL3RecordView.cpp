/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#include "lkSQL3RecordView.h"

#include "MainDocument.h"
#include "Defines.h"
#include "main.h"

#include <wx/artprov.h>
#include "../lkSQLite3/lkSQL3Exception.h"
#include <wx/msgdlg.h>

#include "../lkControls/lkConfigTools.h"

wxDEFINE_EVENT(lkEVT_SQL3MOVE, lkSQL3MoveEvent);

// /////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3RecordDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(lkSQL3RecordDocument, lkDocument)
/*
	EVT_UPDATE_UI(ID_RecFirst, lkSQL3RecordDocument::OnUpdateRecordFirst)
	EVT_UPDATE_UI(ID_RecPrev, lkSQL3RecordDocument::OnUpdateRecordPrev)
	EVT_UPDATE_UI(ID_RecNext, lkSQL3RecordDocument::OnUpdateRecordNext)
	EVT_UPDATE_UI(ID_RecLast, lkSQL3RecordDocument::OnUpdateRecordLast)
	EVT_UPDATE_UI(ID_RecNew, lkSQL3RecordDocument::OnUpdateRecordAdd)
	EVT_UPDATE_UI(ID_RecRemove, lkSQL3RecordDocument::OnUpdateRecordDelete)
	EVT_UPDATE_UI(ID_RecUpdate, lkSQL3RecordDocument::OnUpdateRecordUpdate)
	EVT_COMMAND_RANGE(ID_RecFirst, ID_RecLast, wxEVT_TOOL, lkSQL3RecordDocument::OnMove)
	EVT_TOOL(ID_RecNew, lkSQL3RecordDocument::OnAdd)
	EVT_TOOL(ID_RecUpdate, lkSQL3RecordDocument::OnUpdate)
	EVT_TOOL(ID_RecRemove, lkSQL3RecordDocument::OnDelete)
*/
wxEND_EVENT_TABLE()

wxIMPLEMENT_ABSTRACT_CLASS(lkSQL3RecordDocument, lkDocument)

lkSQL3RecordDocument::lkSQL3RecordDocument(wxDocument* pParent) : lkDocument(pParent)
{
	wxASSERT(IsChildDocument());

	m_pSet = NULL;
	m_bChanged = false;

	m_bCanRemove = false;
	// following should be called from View::OnCreate, the ids used gets created after the Doc allready created !
	// BindButtons();
}
lkSQL3RecordDocument::~lkSQL3RecordDocument()
{
}

bool lkSQL3RecordDocument::HasChanged() const
{
	return m_bChanged;
}
void lkSQL3RecordDocument::ResetChanged()
{
	m_bChanged = false;
}

wxWindow* lkSQL3RecordDocument::GetFrame()
{
	wxView* v = GetFirstView();
	wxWindow* f = (v) ? v->GetFrame() : NULL;

	if ( f && f->IsKindOf(wxCLASSINFO(lkSQL3RecordChildFrame)) )
		return f;
	// else
	return NULL;
}

void lkSQL3RecordDocument::BindButtons()
{
	lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());

	if ( !f )
		return;

	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordFirst, this, f->GetID_RecFirst());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordPrev, this, f->GetID_RecPrev());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordNext, this, f->GetID_RecNext());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordLast, this, f->GetID_RecLast());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordAdd, this, f->GetID_RecNew());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordDelete, this, f->GetID_RecRemove());
	Bind(wxEVT_UPDATE_UI, &lkSQL3RecordDocument::OnUpdateRecordUpdate, this, f->GetID_RecUpdate());

	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnMove, this, f->GetID_RecFirst());
	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnMove, this, f->GetID_RecPrev());
	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnMove, this, f->GetID_RecNext());
	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnMove, this, f->GetID_RecLast());

	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnAdd, this, f->GetID_RecNew());
	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnUpdate, this, f->GetID_RecUpdate());
	Bind(wxEVT_TOOL, &lkSQL3RecordDocument::OnDelete, this, f->GetID_RecRemove());
}

lkSQL3Database* lkSQL3RecordDocument::GetDB()
{
	wxDocument* parent = (IsChildDocument()) ? m_documentParent : NULL;
	if ( parent && parent->IsKindOf(wxCLASSINFO(MainDocument)) )
		return (dynamic_cast<MainDocument*>(parent))->GetDB();
	// else
	return NULL;
}

bool lkSQL3RecordDocument::IsOk() const
{ // true if IsOpen && m_bValidDB==true
	wxDocument* parent = (IsChildDocument()) ? m_documentParent : NULL;
	if ( parent && parent->IsKindOf(wxCLASSINFO(MainDocument)) )
		return (dynamic_cast<MainDocument*>(parent))->IsOk();
	// else
	return false;
}

void lkSQL3RecordDocument::SetCanRemove()
{
	// doing things like this, while some tables might run a big sql for this and that takes time
	m_bCanRemove = CanRemove();
}

lkSQL3RecordView* lkSQL3RecordDocument::sql3GetView()
{
	lkSQL3RecordView* view = NULL;

	wxView* const tV = GetFirstView();
	if ( tV && tV->IsKindOf(wxCLASSINFO(lkSQL3RecordView)) )
		view = dynamic_cast<lkSQL3RecordView*>(tV);

	return view;
}

// virtual
bool lkSQL3RecordDocument::UpdateData(bool bSaveAndValidate)
{
	bool bRet = false;
	lkSQL3RecordView* view = sql3GetView();
	if ( view )
		bRet = view->UpdateData(bSaveAndValidate);

	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3RecordDocument :: Operations

lkSQL3RecordSet* lkSQL3RecordDocument::GetRecordset()
{
	if ( !m_pSet )
	{
		// not opened yet, open now
		lkSQL3Database* curDB = GetDB();
		wxASSERT(curDB && IsOk());

		if ( !curDB || !IsOk() )
			return NULL;

		lkSQL3RecordSet* pSet = GetBaseSet();
		wxASSERT(pSet != NULL);
		if ( !pSet ) return NULL;

		wxASSERT(pSet->IsKindOf(wxCLASSINFO(lkSQL3RecordSet)));

		m_pSet = pSet;
		pSet = NULL;
	}

	if ( m_pSet && (!m_pSet->IsOpen()) )
	{
		wxString cst = m_pSet->GetOrder();
		m_pSet->SetOrder(cst.IsEmpty() ? GetDefaultOrder() : cst);
		cst = m_pSet->GetFilter();
		m_pSet->SetFilter(cst.IsEmpty() ? GetDefaultFilter() : cst);

		try
		{
			m_pSet->Open();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			if ( m_pSet ) delete m_pSet;
			m_pSet = NULL;
		}
	}

	return m_pSet;
}

//virtual
wxString lkSQL3RecordDocument::GetDefaultFilter() const
{
	return wxT("");
}

//virtual
wxString lkSQL3RecordDocument::GetDefaultOrder() const
{
	return wxT("");
}

bool lkSQL3RecordDocument::CanAppend() // will get it from the recordset
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	return pSet->CanAdd();
}
bool lkSQL3RecordDocument::CanRemove() // will get it from the recordset
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	return pSet->CanDelete();
}

bool lkSQL3RecordDocument::sql3Delete()
{
	ResetChanged();
	// recordset must be allocated already
	wxASSERT(m_pSet != NULL);
	if ( !m_pSet )
		return true;
	bool b = m_pSet->Delete();
	if ( b )
	{
		m_bChanged = true;
		UpdateAllViews(NULL, m_pSet);
		if ( m_pSet->GetRowCount() == 0 )
			sql3Add();
		else
			UpdateData(false);
	}

	return b;
}

bool lkSQL3RecordDocument::sql3Update()
{
	ResetChanged();
	wxASSERT(m_pSet != NULL);
	if ( !m_pSet )
		return true; // don't let the app block

	if ( m_pSet->CanUpdate() )
	{
//		if ( view ) // && view->UpdateData(true) )
//		UpdateData(true) calls sql3Update - don't want to end up in endless loop
		if ( m_pSet->IsDirty(NULL) )
		{
			m_bChanged = true;
			DoThingsBeforeUpdate();
			try
			{
				m_pSet->Update();
			}
			catch ( const lkSQL3Exception& e )
			{
				((lkSQL3Exception*)&e)->ReportError();
				return false;
			}

			UpdateAllViews(NULL, m_pSet);
		}
		else if ( m_pSet->IsAdding() )
		{
			sql3Cancel();
		}
	}

	return true;
}

bool lkSQL3RecordDocument::sql3Add()
{
	if ( !m_pSet || !CanAppend() )
		return false;

	if ( !m_pSet->IsEmpty() )
		m_pSet->MoveLast();

	bool b = m_pSet->AddNew();
	DoThingsBeforeAdd();
	UpdateData(false);

	return b;
}

void lkSQL3RecordDocument::sql3Cancel() // when cancel pressed in Frame, the frame then will call this function
{
	if ( !m_pSet )
		return; // should only perform if recordset is open

	bool bWasAdding = m_pSet->IsAdding();
	if ( bWasAdding )
		m_pSet->CancelAddNew();
	else
		m_pSet->CancelUpdate();

	if ( m_pSet->IsEmpty() )
	{
		wxView* v = GetFirstView();
		wxWindow* w = (v) ? v->GetFrame() : NULL;
		if ( w )
		{
			// empty recordset, but cancel pressed, so Close the ViewFrame -- nothing in it to browse
			if ( v && v->IsKindOf(wxCLASSINFO(lkSQL3RecordView)) )
				(dynamic_cast<lkSQL3RecordView*>(v))->SetClosing(true);
			w->Close(true);
			return;
		}
		// else..
		m_pSet->AddNew(); // stay in Adding modus -- empty recordset
		UpdateData(false);
		return;
	}

	if ( bWasAdding )
		sql3Move(RecordLast);
	else
		UpdateData(false);
}

bool lkSQL3RecordDocument::sql3Move(lkSQL3RecordDocument::MoveIDs nIDMoveCommand)
{
	lkSQL3RecordSet* pSet = GetRecordset();
	if ( !pSet || !pSet->IsOpen() )
		return false;

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
	return true;
}

//virtual
void lkSQL3RecordDocument::DoThingsBeforeUpdate()
{
}
//virtual
void lkSQL3RecordDocument::DoThingsBeforeAdd()
{
}

bool lkSQL3RecordDocument::ConfirmDelete()
{
	wxString msg = wxT("This action will permanently destroy current record.\nAre you sure you want to DELETE current record ?");
	wxString ttl = wxT("Confirm Removal");

	wxView* v = GetFirstView();
	wxWindow* w = (v) ? v->GetFrame() : NULL;
	lkView* lV = (v && v->IsKindOf(wxCLASSINFO(lkView))) ? dynamic_cast<lkView*>(v) : NULL;

	if ( lV )
		lV->SetCanClose(false);

	bool bRet = (wxMessageBox(msg, ttl, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, w) == wxYES);

	if ( lV )
		lV->SetCanClose(true);

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3RecordDocument :: Event Handling

void lkSQL3RecordDocument::OnUpdateRecordFirst(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsOnFirstRecord() && !m_pSet->IsAdding()) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordPrev(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsOnFirstRecord() && !m_pSet->IsAdding()) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordNext(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsOnLastRecord() && !m_pSet->IsAdding()) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordLast(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsOnLastRecord() && !m_pSet->IsAdding()) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordAdd(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && CanAppend() ) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordDelete(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && m_bCanRemove/*CanRemove()*/) : false);
}
void lkSQL3RecordDocument::OnUpdateRecordUpdate(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? m_pSet->CanUpdate() : false);
}

void lkSQL3RecordDocument::OnUpdateFind(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false);
}
void lkSQL3RecordDocument::OnUpdateFindNext(wxUpdateUIEvent& event)
{
	event.Enable(m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindNext);
}
void lkSQL3RecordDocument::OnUpdateFindPrev(wxUpdateUIEvent& event)
{
	event.Enable(m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindPrev);
}

void lkSQL3RecordDocument::OnMove(wxCommandEvent& event)
{
	lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());

	if ( !f )
		return;

	MoveIDs move_id;

	if ( event.GetId() == f->GetID_RecFirst() )
		move_id = RecordFirst;
	else if ( event.GetId() == f->GetID_RecPrev() )
		move_id = RecordPrev;
	else if ( event.GetId() == f->GetID_RecNext() )
		move_id = RecordNext;
	else if ( event.GetId() == f->GetID_RecLast() )
		move_id = RecordLast;
	else
		move_id = RecordINVALID;

	if ( move_id != RecordINVALID )
	{
		if (UpdateData(true))
			sql3Move(move_id);
	}
}

void lkSQL3RecordDocument::OnAdd(wxCommandEvent& event)
{
	if ( !m_pSet || !UpdateData(true) )
	{
		// event.Skip();
		return;
	}

	sql3Add(); // will call 'DoThingsBeforeAdd'
}

void lkSQL3RecordDocument::OnUpdate(wxCommandEvent& event)
{
	if ( UpdateData(true) )
		UpdateData(false);
}

void lkSQL3RecordDocument::OnDelete(wxCommandEvent& event)
{
	if ( /*CanRemove()*/ m_bCanRemove && ConfirmDelete() )
		sql3Delete();
}

void lkSQL3RecordDocument::OnSQL3Move(lkSQL3MoveEvent& event)
{
	lkSQL3RecordSet* pSet = GetRecordset();
	if ( IsOk() && pSet && UpdateData(true) )
	{
		pSet->Move(event.GetRow());
		UpdateData(false);
	}
}

void lkSQL3RecordDocument::OnSearchNext(wxCommandEvent& WXUNUSED(event))
{
	lkSQL3RecordSet* pSet = GetRecordset();

	wxUint64 idx = pSet->FindNext();
	if ( idx > 0 )
	{
		pSet->Move(idx);
		UpdateData(false);
	}
}
void lkSQL3RecordDocument::OnSearchPrev(wxCommandEvent& WXUNUSED(event))
{
	lkSQL3RecordSet* pSet = GetRecordset();

	wxUint64 idx = pSet->FindPrev();
	if ( idx > 0 )
	{
		pSet->Move(idx);
		UpdateData(false);
	}
}

// ////////////////////////////////////////////////////////////////////
// class lkSQL3panel
// ////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(lkSQL3panel, lkCanvas)

wxBEGIN_EVENT_TABLE(lkSQL3panel, lkCanvas)
	EVT_RIGHT_DOWN(lkSQL3panel::OnRightClick)
wxEND_EVENT_TABLE()

lkSQL3panel::lkSQL3panel(wxView* view, wxWindow* parent) : lkCanvas(view, parent)
{
}

lkSQL3RecordDocument* lkSQL3panel::GetDocument()
{
	wxDocument* doc = (m_pView) ? m_pView->GetDocument() :
		((m_Parent && m_Parent->IsKindOf(wxCLASSINFO(lkSQL3RecordChildFrame))) ? (dynamic_cast<lkSQL3RecordChildFrame*>(m_Parent))->GetDocument() : NULL);

	if ( doc && doc->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
		return dynamic_cast<lkSQL3RecordDocument*>(doc);

	return NULL;
}
lkSQL3RecordView* lkSQL3panel::GetView()
{
	if ( m_pView && m_pView->IsKindOf(wxCLASSINFO(lkSQL3RecordView)) )
		return dynamic_cast<lkSQL3RecordView*>(m_pView);

	return NULL;
}

void lkSQL3panel::OnRightClick(wxMouseEvent& event)
{
#ifdef __WXDEBUG__
	lkSQL3RecordView* v = GetView();
	if ( v ) v->SetCanClose(false);
	wxWindowDisabler wd(true);

	wxSize sizeC = GetClientSize();
	wxSize sizeF = GetParent()->GetSize();
	wxString s; s.Printf(wxT("Frame Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);

	if ( v ) v->SetCanClose(true);
#else
	event.Skip();
#endif // WXDEBUG
}


// ////////////////////////////////////////////////////////////////////
// class lkSQL3RecordView
// ////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS(lkSQL3RecordView, lkView)

lkSQL3RecordView::lkSQL3RecordView() : lkView()
{
	m_pCanvas = NULL;
	m_bClosing = false;
	m_bInitial = false;
}
lkSQL3RecordView::~lkSQL3RecordView()
{
}

lkSQL3Database* lkSQL3RecordView::GetDB()
{
	wxDocument* _doc = GetDocument();
	if ( !_doc || !_doc->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
		return NULL;

	return (dynamic_cast<lkSQL3RecordDocument*>(_doc))->GetDB();
}

void lkSQL3RecordView::SetClosing(bool bClosing)
{
	m_bClosing = bClosing;
}

bool lkSQL3RecordView::IsInitial() const
{
	return m_bInitial;
}

lkSQL3panel* lkSQL3RecordView::GetCanvas()
{
	return m_pCanvas;
}

////////////////////////////////////////////////////////////////
// lkSQL3RecordView Implementation

void lkSQL3RecordView::SetCurrentRecord(wxUint64 u)
{
	wxWindow* f = GetFrame();
	if ( f && f->IsKindOf(wxCLASSINFO(lkSQL3RecordChildFrame)) )
		(dynamic_cast<lkSQL3RecordChildFrame*>(f))->SetCurrentRecord(u);
}
void lkSQL3RecordView::SetTotalRecords(wxUint64 u)
{
	wxWindow* f = GetFrame();
	if ( f && f->IsKindOf(wxCLASSINFO(lkSQL3RecordChildFrame)) )
		(dynamic_cast<lkSQL3RecordChildFrame*>(f))->SetTotalRecords(u);
}

void lkSQL3RecordView::InitialUpdate()
{
	m_bInitial = true;
	wxDocument* d = GetDocument();
	if ( d && d->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
	{
		lkSQL3RecordSet* pSet = (dynamic_cast<lkSQL3RecordDocument*>(d))->GetRecordset();
		if ( pSet && (pSet->GetRowCount() == 0) )
			(dynamic_cast<lkSQL3RecordDocument*>(d))->sql3Add();
		else
			UpdateData(false);
	}
	else
		UpdateData(false);

	UpdateRecordStatusbar();
	m_bInitial = false;
}

bool lkSQL3RecordView::UpdateData(bool bSaveAndValidate)
{
	if ( !m_pCanvas )
		return true; // no Panel set, nothing to do
	if ( m_bClosing )
		return true;

	if ( bSaveAndValidate )
	{
		SetCanClose(false);
		bool bV = m_pCanvas->Validate();
		SetCanClose(true);

		if ( bV )
		{
			if ( m_pCanvas->TransferDataFromWindow() )
			{
				wxDocument* doc = GetDocument();
				if ( doc && doc->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
					return (dynamic_cast<lkSQL3RecordDocument*>(doc))->sql3Update();
				else
					return true; // not our sql-recordset-doc, so returning true otherwise the app can't be closed at all
			}
		}
//		else
		return false; // no good
	}
	// else
	wxDocument* doc = GetDocument();
	if ( doc && doc->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->SetCanRemove();

	m_pCanvas->TransferDataToWindow();
	UpdateRecordStatusbar();
	return true;
}

void lkSQL3RecordView::UpdateRecordStatusbar()
{
	wxDocument* d = GetDocument();
	if ( d && d->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
	{
		lkSQL3RecordSet* pSet = (dynamic_cast<lkSQL3RecordDocument*>(d))->GetRecordset();
		if ( pSet )
		{
			wxUint64 u = pSet->IsAdding() ? 1 : 0;
			SetCurrentRecord(pSet->GetCurRow() + u);
			SetTotalRecords(pSet->GetRowCount() + u);
		}
	}
}

void lkSQL3RecordView::SetConfigPosition() // called in 'OnCloseWindow' -- will store frame's current position in cofig-file
{
	wxWindow* w = GetFrame();
	wxPoint pos = wxDefaultPosition;
	if (w)
	{
		wxFrame* f = dynamic_cast<wxFrame*>(w);
		pos = f->GetPosition();
	}
	if (pos != wxDefaultPosition)
	{
		SetConfigInt(GetConfigPath(), wxT("PosX"), (wxInt64)pos.x);
		SetConfigInt(GetConfigPath(), wxT("PosY"), (wxInt64)pos.y);
	}
}

wxPoint	lkSQL3RecordView::GetConfigPosition() const
{
	wxPoint pos;
	int x, y;
	x = (int)GetConfigInt(GetConfigPath(), wxT("PosX"));
	y = (int)GetConfigInt(GetConfigPath(), wxT("PosY"));

	if ( x <= 0 ) x = -1;
	if ( y <= 0 ) y = -1;

	if ( (x < 0) && (y < 0) )
		return wxDefaultPosition;
	else
	{
		pos.x = x;
		pos.y = y;
	}
	return pos;
}

void lkSQL3RecordView::ProcessRecordset(lkSQL3RecordSet*)
{ // called from 'OnUpdate'
  // by default, doesn't do anything
}

////////////////////////////////////////////////////////////////
// lkSQL3RecordView Overrides

//virtual
void lkSQL3RecordView::OnUpdate(wxView* pSender, wxObject* pHint)
{
	if ( (pHint == NULL) || (pHint->IsKindOf(wxCLASSINFO(lkSQL3RecordSet))) )
		ProcessRecordset((lkSQL3RecordSet*)pHint);
}

//virtual
bool lkSQL3RecordView::OnClose(bool deleteWindow)
{
	SetConfigPosition();
	return wxView::OnClose(deleteWindow);
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3RecordChildFrame
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(lkSQL3RecordChildFrame, lkRecordChildFrame)

lkSQL3RecordChildFrame::lkSQL3RecordChildFrame() : lkRecordChildFrame()
{
}

//virtual
void lkSQL3RecordChildFrame::DoCancel()
{
	wxDocument* d = GetDocument();
	if ( d && d->IsKindOf(wxCLASSINFO(lkSQL3RecordDocument)) )
		(dynamic_cast<lkSQL3RecordDocument*>(d))->sql3Cancel();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class lkSQL3MoveEvent
////
wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3MoveEvent, wxCommandEvent)

lkSQL3MoveEvent::lkSQL3MoveEvent(int id) : wxCommandEvent(lkEVT_SQL3MOVE, id), m_nRow(0)
{
}
lkSQL3MoveEvent::lkSQL3MoveEvent(const lkSQL3MoveEvent& other) : wxCommandEvent(other), m_nRow(0)
{
	m_nRow = other.m_nRow;
}

//virtual
wxEvent* lkSQL3MoveEvent::Clone() const
{
	return new lkSQL3MoveEvent(*this);
}

wxUint64 lkSQL3MoveEvent::GetRow() const
{
	return m_nRow;
}
void lkSQL3MoveEvent::SetRow(wxUint64 nRow)
{
	m_nRow = nRow;
}
