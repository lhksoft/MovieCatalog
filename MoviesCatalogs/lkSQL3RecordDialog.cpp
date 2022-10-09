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
#include "lkSQL3RecordDialog.h"
#include "../lkSQLite3/lkSQL3Exception.h"

wxIMPLEMENT_ABSTRACT_CLASS(lkSQL3RecordDialog, lkRecordDlg)


lkSQL3RecordDialog::lkSQL3RecordDialog(lkSQL3Database* pDB) : lkRecordDlg()
{
	m_pDB = pDB;
	m_pSet = NULL;
	m_nCurRow = 0;
}

bool lkSQL3RecordDialog::Create(wxWindow* parent, const wxString& sBackImage, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	return lkRecordDlg::Create(parent, wxImage(sBackImage), title, pos, size, style);
}
bool lkSQL3RecordDialog::Create(wxWindow* parent, const wxImage& sBackImage, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	return lkRecordDlg::Create(parent, sBackImage, title, pos, size, style);
}

//virtual
void lkSQL3RecordDialog::BindButtons()
{
	lkRecordDlg::BindButtons();
	Bind(wxEVT_INIT_DIALOG, &lkSQL3RecordDialog::OnInitDialog, this);
	Bind(wxEVT_BUTTON, &lkSQL3RecordDialog::OnButton, this, wxID_ANY);
}

lkSQL3Database* lkSQL3RecordDialog::GetDB()
{
	return m_pDB;
}
void lkSQL3RecordDialog::SetDB(lkSQL3Database* pDB)
{
	m_pDB = pDB;
}

lkSQL3RecordSet* lkSQL3RecordDialog::GetRecordset()
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

void lkSQL3RecordDialog::SetCurRecord(wxUint64 nRow)
{
	m_nCurRow = nRow;
}
wxUint64 lkSQL3RecordDialog::GetCurRecord() const
{
	return m_nCurRow;
}

////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////

//virtual
bool lkSQL3RecordDialog::IsOk() const
{
	return (m_pDB && m_pDB->IsOpen());
}

//virtual
bool lkSQL3RecordDialog::CanAppend() // will get it from the recordset
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet )
		return false;
	//else..
	return pSet->CanAdd();
}
//virtual
bool lkSQL3RecordDialog::CanRemove() // will get it from the recordset
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet )
		return false;
	//else..
	return pSet->CanDelete();
}

//virtual
void lkSQL3RecordDialog::DoEscape() // when cancel pressed in Frame, the frame then will call this function
{
	lkSQL3RecordSet* pSet = GetRecordset();
	if ( !pSet )
		return; // should only perform if recordset is open

	bool bWasAdding = pSet->IsAdding();
	if ( bWasAdding )
		pSet->CancelAddNew();
	else
		pSet->CancelUpdate();

	if ( pSet->IsEmpty() )
	{
		if ( !Close(true) )
		{
			pSet->AddNew(); // stay in Adding modus -- empty recordset
			UpdateData(false);
		}
		return;
	}

	if ( bWasAdding )
		DoMove(MoveIDs::RecordLast);
	else
		UpdateData(false);
}

//virtual
bool lkSQL3RecordDialog::DoDelete()
{
	ResetChanged();
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet )
		return true;
	bool b = pSet->Delete();
	if ( b )
	{
		SetChanged();
		if ( pSet->GetRowCount() == 0 )
			DoAdd();
		else
			UpdateData(false);
	}

	return b;
}

//virtual
bool lkSQL3RecordDialog::DoUpdate()
{
	ResetChanged();
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet )
		return true;

	if ( pSet->CanUpdate() )
	{
		//	UpdateData(true) calls DoUpdate - don't want to end up in endless loop
		if ( pSet->IsDirty(NULL) )
		{
			SetChanged();
			DoThingsBeforeUpdate();
			try
			{
				pSet->Update();
			}
			catch ( const lkSQL3Exception& e )
			{
				((lkSQL3Exception*)&e)->ReportError();
				return false;
			}
		}
		else if ( pSet->IsAdding() )
		{
			DoEscape();
		}
	}

	return true;
}

//virtual
bool lkSQL3RecordDialog::DoAdd()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet || !CanAppend() )
		return false;

	if ( !pSet->IsEmpty() )
		pSet->MoveLast();

	if ( pSet->AddNew() )
	{
		UpdateData(false);
		DoThingsBeforeAdd();
		return true;
	}
	return false;
}

//virtual
bool lkSQL3RecordDialog::DoMove(lkRecordDlg::MoveIDs nIDMoveCommand)
{
	if ( !IsOk() )
		return false;

	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already
	wxASSERT(pSet != NULL);
	if ( !pSet )
		return false;

	switch ( nIDMoveCommand )
	{
		case MoveIDs::RecordFirst:
			pSet->MoveFirst();
			break;

		case MoveIDs::RecordPrev:
			pSet->MovePrev();
			break;

		case MoveIDs::RecordNext:
			pSet->MoveNext();
			break;

		case MoveIDs::RecordLast:
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

// Moves to given RowID
bool lkSQL3RecordDialog::GotoRecord()
{
	bool bOk = false;
	if ( m_nCurRow > 0 )
	{
		lkSQL3RecordSet* pSet = GetRecordset();

		int pos = -1;
		if ( (pos = pSet->FindActualCurRow(m_nCurRow)) != -1 )
		{
			pSet->Move(pos);
			UpdateData(false);
			bOk = true;
		}
	}
	return bOk;
}


//virtual
void lkSQL3RecordDialog::UpdateRecordStatusbar()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	// recordset must be allocated already

	if ( pSet )
	{
		wxUint64 u = pSet->IsAdding() ? 1 : 0;
		SetCurrentRecord(pSet->GetCurRow() + u);
		SetTotalRecords(pSet->GetRowCount() + u);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3RecordDialog :: Event Handling

//virtual
bool lkSQL3RecordDialog::DoUpdateRecordFirst()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsOnFirstRecord() && !pSet->IsAdding()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordPrev()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsOnFirstRecord() && !pSet->IsAdding()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordNext()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsOnLastRecord() && !pSet->IsAdding()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordLast()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsOnLastRecord() && !pSet->IsAdding()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordNew()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsAdding() && CanAppend()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordRemove()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? (!pSet->IsAdding() && CanRemove()) : false);
}
//virtual
bool lkSQL3RecordDialog::DoUpdateRecordUpdate()
{
	lkSQL3RecordSet* pSet = GetRecordset();
	return ((pSet) ? pSet->CanUpdate() : false);
}


// Event Handling
////////////////////////////////////////////////////////

void lkSQL3RecordDialog::OnInitDialog(wxInitDialogEvent& event)
{
	lkRecordDlg::OnInitDialog(event);

	lkSQL3RecordSet* pSet = GetRecordset();
	if ( pSet && (pSet->GetRowCount() == 0) )
		DoAdd();
	else
	{
		if ( !GotoRecord() )
			DoMove(MoveIDs::RecordLast);
	}

	UpdateRecordStatusbar();
}

void lkSQL3RecordDialog::OnButton(wxCommandEvent& event)
{
	const int id = event.GetId();
	if ( id == GetAffirmativeId() )
	{
		lkSQL3RecordSet* pSet = GetRecordset();
		if ( (m_nCurRow = (pSet) ? pSet->GetActualCurRow() : 0) == 0 ) // in case no new data set, terminate anyhow
		{
			EndDialog(m_affirmativeId);
			return;
		}
	}
	event.Skip();
}

void lkSQL3RecordDialog::OnFindNext(wxCommandEvent& WXUNUSED(event))
{
	lkSQL3RecordSet* pSet = GetRecordset();

	wxUint64 idx = pSet->FindNext();
	if ( idx > 0 )
	{
		pSet->Move(idx);
		UpdateData(false);
	}
}
void lkSQL3RecordDialog::OnFindPrev(wxCommandEvent& WXUNUSED(event))
{
	lkSQL3RecordSet* pSet = GetRecordset();

	wxUint64 idx = pSet->FindPrev();
	if ( idx > 0 )
	{
		pSet->Move(idx);
		UpdateData(false);
	}
}

