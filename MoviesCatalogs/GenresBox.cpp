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
#include "GenresBox.h"
#include "TGenre.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include <wx/msgdlg.h>

////////////////////////////////////////////////////////////////////////////
// class GenresBox
////
wxIMPLEMENT_CLASS(GenresBox, lkChkImgListbox)

GenresBox::GenresBox(lkSQL3Database* pDB) : lkChkImgListbox()
{
	m_pDB = pDB;
}
GenresBox::GenresBox(lkSQL3Database* pDB, wxWindow* pParent, wxWindowID nId, const wxPoint& nPos, const wxSize& nSize, long nStyle, const wxValidator& validator)
	: lkChkImgListbox(pParent, nId, nPos, nSize, nStyle, validator)
{
	m_pDB = pDB;
}
GenresBox::~GenresBox()
{
}

void GenresBox::SetCheck(wxUint64 _val)
{
	if ( IsEmpty() )
		return; // nothing to do

	wxUint64 lParam;
	bool bCheck;
	int sel = wxNOT_FOUND, c = (int)GetCount(), curSel = lkChkImgListbox::GetSelection();
	for ( int i = 0; i < c; i++ )
	{
		lParam = GetLParam((unsigned int)i);
		bCheck = ((_val != 0) && (lParam != 0)) ? (_val & lParam) : false;
		lkChkImgListbox::SetCheck(i, bCheck);
		if ( (sel == wxNOT_FOUND) && bCheck )
			sel = i;
		if ( (curSel == i) && !bCheck )
			curSel = wxNOT_FOUND;
	}
	if ( curSel == wxNOT_FOUND )
		curSel = sel;

	lkChkImgListbox::SetSelection(curSel);
	Refresh(false);
	// ScrollIntoView(sel);
	// ScrollToRow(sel);
}

wxUint64 GenresBox::GetChecked() const
{
	wxUint64 _val = 0;
	int c = GetCount();
	for ( int i = 0; i < c; i++ )
	{
		if ( lkChkImgListbox::IsChecked(i) )
			_val |= GetLParam((unsigned int)i);
	}

	return _val;
}

bool GenresBox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( !m_pDB || !m_pDB->IsOpen() ) return false; // nothing to do

	TGenre rsGen;
	rsGen.SetDatabase(m_pDB);
	{
		wxString order;
		order.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Genre_GENRE);
		rsGen.SetOrder(order);
	}

	try
	{
		rsGen.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

//	StartOver();

	lkBoxInitList* pInitList = NULL;
	if ( !rsGen.IsEmpty() )
	{
		wxImage			img;
		wxString		sName;
		wxUint64		lParam;
		
		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		rsGen.MoveFirst();
		while ( !rsGen.IsEOF() )
		{
			sName = rsGen.GetGenreValue();
			img = rsGen.GetImage();
			lParam = rsGen.GetFlagValue();

			// fill initlist
			pInitList->Append(new lkBoxInit(sName, img, lParam));

			rsGen.MoveNext();
		}
	}

	bool bRet = false;
	if ( pInitList != NULL )
		if ( InitializeFromList(pInitList, true, lkExpandFlags::EXPAND_Center) )
			bRet = Populate();

	if ( pInitList )
		delete pInitList;
	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// class GenresValidator
////
wxIMPLEMENT_DYNAMIC_CLASS(GenresValidator, wxValidator)

GenresValidator::GenresValidator() : wxValidator()
{
	m_pField = NULL;
	m_pValue = NULL;
}
GenresValidator::GenresValidator(lkSQL3FieldData_Int* pField) : wxValidator()
{
	m_pField = pField;
	m_pValue = NULL;
}
GenresValidator::GenresValidator(wxUint64* pValue) : wxValidator()
{
	m_pField = NULL;
	m_pValue = pValue;
}
GenresValidator::GenresValidator(const GenresValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
	m_pValue = other.m_pValue;
}

//virtual
wxObject* GenresValidator::Clone() const
{
	return new GenresValidator(*this);
}

GenresBox* GenresValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, GenresBox) )
		return dynamic_cast<GenresBox*>(m_validatorWindow);

	wxFAIL_MSG(wxT("GenresValidator can only be used with a GenresBox"));

	return NULL;
}

//virtual
bool GenresValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField || m_pValue);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled())
		return true;

	GenresBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	if ( m_pField )
	{
		// See if at least 1 item is checked
		bool bChecked = false;
		int c = ctrl->GetCount();
		for ( int i = 0; i < c; i++ )
			if ( bChecked = ctrl->IsChecked(i) )
				break;

		wxString msg;
		if ( !bChecked )
			msg = wxT("No genre set yet, although this field is required.");

		if ( !msg.IsEmpty() )
		{
			m_validatorWindow->SetFocus();
			wxMessageBox(msg, wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

			return false;
		}
	}

	return true;
}

//virtual
bool GenresValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField || m_pValue);

	GenresBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	wxUint64 n = 0;
	if ( m_pField ) // current value from the db
		n = static_cast<wxUint64>(m_pField->GetValue2());
	else if ( m_pValue )
		n = *m_pValue;

	ctrl->SetCheck(n);

	return true;
}

//virtual
bool GenresValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField || m_pValue);

	GenresBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	wxUint64 n = ctrl->GetChecked();

	if ( m_pField )
	{
		// only if previous value (when initialising the controll) was different than now, then tell sql3-db about the new value
		if ( n == 0 )
		{
			if ( !m_pField->IsNull() )
				m_pField->SetValueNull();
		}
		else
		{
			// current value from the db for Validation
			wxUint64 p = static_cast<wxUint64>(m_pField->GetValue2());

			if ( p != n )
				m_pField->SetValue2(static_cast<wxInt64>(n));
		}
	}
	else if ( m_pValue )
		*m_pValue = n;

	return true;
}

