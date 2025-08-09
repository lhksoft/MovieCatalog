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
#include "VBase_controls.h"
#include "TCountry.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

#include "../lkSQLite3/lkSQL3Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseCountryBox
////
wxIMPLEMENT_ABSTRACT_CLASS(BaseCountryBox, lkChkImgListbox)

BaseCountryBox::BaseCountryBox(lkSQL3Database* pDB, bool bActingAsAudio) : lkChkImgListbox(), m_TmpList()
{
	m_pDB = pDB;
	m_bActingAsAudio = bActingAsAudio;
	m_TmpList.DeleteContents(true);
}
BaseCountryBox::BaseCountryBox(lkSQL3Database* pDB, bool bActingAsAudio, wxWindow* pParent, wxWindowID nId, const wxPoint& nPos, const wxSize& nSize, long nStyle, const wxValidator& validator)
	: lkChkImgListbox(pParent, nId, nPos, nSize, nStyle, validator), m_TmpList()
{
	m_pDB = pDB;
	m_bActingAsAudio = bActingAsAudio;
	m_TmpList.DeleteContents(true);
}
BaseCountryBox::~BaseCountryBox()
{
}

void BaseCountryBox::SetCheck(const wxString& ctr)
{
	if ( IsEmpty() )
		return; // nothing to do

	wxArrayString strArr;
	if ( !ctr.IsEmpty() )
	{
		// Split given Countries-String
		wxStringTokenizer tok(ctr, wxT(" "));
		while ( tok.HasMoreTokens() )
			strArr.Add(tok.GetNextToken(), 1);
	}

	wxUint64 lParam;
	lkStringObj* o;
	bool bCheck;
	int sel = wxNOT_FOUND, c = (int)GetCount(), curSel = lkChkImgListbox::GetSelection();
	for ( int i = 0; i < c; i++ )
	{
		lParam = GetLParam((unsigned int)i);
		o = (lkStringObj*)lParam;
		if ( o && o->IsKindOf(wxCLASSINFO(lkStringObj)) )
		{
			// find if this country is in the Array
			bCheck = (!strArr.IsEmpty()) ? (strArr.Index(o->m_String1) != wxNOT_FOUND) : false;

			lkChkImgListbox::SetCheck(i, bCheck);
			if ( (sel == wxNOT_FOUND) && bCheck )
				sel = i;
			if ( (curSel == i) && !bCheck )
				curSel = wxNOT_FOUND;
		}
	}
	if ( curSel == wxNOT_FOUND )
		curSel = sel;

	lkChkImgListbox::SetSelection(curSel);
	Refresh(false);
	//	lkChkImgListbox::ScrollIntoView(sel);
}

wxString BaseCountryBox::GetChecked() const
{
	wxString res;
	if ( IsEmpty() )
		return res; // nothing to do

	wxUint64 lParam;
	lkStringObj* o;
	int c = GetCount();
	for ( int i = 0; i < c; i++ )
	{
		if ( lkChkImgListbox::IsChecked(i) )
			if ( (lParam = GetLParam((unsigned int)i)) != 0 )
				if ( (o = (lkStringObj*)lParam)->IsKindOf(wxCLASSINFO(lkStringObj)) )
				{
					if ( !res.IsEmpty() )
						res += wxT(" ");
					res += o->m_String1;
				}
	}

	return res;
}

wxString BaseCountryBox::GetFilter(const wxString& field) const
{
	wxString res;
	if ( IsEmpty() || field.IsEmpty())
		return res; // nothing to do
	wxASSERT(field[0] = '[');

	wxString flt, s;
	wxUint64 lParam;
	lkStringObj* o;
	int c = GetCount();
	for ( int i = 0; i < c; i++ )
	{
		if ( lkChkImgListbox::IsChecked(i) )
			if ( (lParam = GetLParam((unsigned int)i)) != 0 )
				if ( (o = (lkStringObj*)lParam)->IsKindOf(wxCLASSINFO(lkStringObj)) )
				{
					s.Printf(wxT("(%s LIKE '%%%s%%')"), field, o->m_String1);
					if ( !flt.IsEmpty() )
						flt += wxT(" AND ");
					flt += s;
				}
	}

	if ( flt.IsEmpty() )
		res.Printf(wxT("(%s IS NULL)"), field);
	else
		res.Printf(wxT("(%s)"), flt);
	return res;
}

bool BaseCountryBox::BuildList(lkExpandFlags flags)
{
	wxASSERT(m_pDB != NULL);
	if ( !m_pDB || !m_pDB->IsOpen() ) return false; // nothing to do

	TCountry rsCtr;
	rsCtr.SetDatabase(m_pDB);
	{
		wxString s;
		s.Printf(wxT("ifnull([%s], ifnull([%s], [%s])) COLLATE lkUTF8compare"), t3Country_SPOKEN, t3Country_CUSTOM, t3Country_DEF);
		rsCtr.SetOrder(s);

		s.Printf(wxT("([%s] & %u)"), t3Country_AUDIOSUBS, ((m_bActingAsAudio) ? 1 : 2));
		rsCtr.SetFilter(s);
	}

	try
	{
		rsCtr.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( !m_TmpList.IsEmpty() )
		m_TmpList.Clear();

	lkBoxInitList* pInitList = NULL;
	if ( !rsCtr.IsEmpty() )
	{
		wxImage			img;
		wxString		sName;
		wxUint64		lParam;
		lkStringObj* oParam;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		rsCtr.MoveFirst();
		while ( !rsCtr.IsEOF() )
		{
			sName = rsCtr.GetSpokenValue();
			if ( sName.IsEmpty() )
				sName = rsCtr.GetCustomCountryValue();
			if ( sName.IsEmpty() )
				sName = rsCtr.GetDefaultCountryValue();

			if ( !(img = rsCtr.GetSpokenFlag()).IsOk() )
				img = rsCtr.GetFlag();

			m_TmpList.Append(oParam = new lkStringObj(rsCtr.GetActualCurRow(), rsCtr.GetIsoValue()));
			lParam = (wxUint64)oParam;

			// fill initlist
			pInitList->Append(new lkBoxInit(sName, img, lParam));

			rsCtr.MoveNext();
		}
	}

	bool bRet = false;
	if ( pInitList != NULL )
		if ( InitializeFromList(pInitList, true, flags) )
			bRet = Populate();

	if ( pInitList )
		delete pInitList;
	return bRet;
}


