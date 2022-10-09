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
#include "CategoryBox.h"
#include "TCategory.h"

#include "../lkSQLite3/lkSQL3Exception.h"

#define F_UNSET_STR		wxT("<unset>") // having value '0'

/////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(CategoryBox, lkImgComboBox)

wxBEGIN_EVENT_TABLE(CategoryBox, lkImgComboBox)
	EVT_COMBOBOX(wxID_ANY, CategoryBox::OnCatSel)
wxEND_EVENT_TABLE()

CategoryBox::CategoryBox() : lkImgComboBox()
{
	m_SubCatBox = NULL;
	m_nCurSel = wxNOT_FOUND;
	m_pDB = NULL;
}
CategoryBox::CategoryBox(wxWindow* parent, wxWindowID id, const wxSize& size, long style, const wxValidator& validator, const wxPoint& pos)
	: lkImgComboBox(parent, id, size, style, validator, pos)
{
	m_SubCatBox = NULL;
	m_nCurSel = wxNOT_FOUND;
	m_pDB = NULL;
}

CategoryBox::~CategoryBox()
{
}

void CategoryBox::SetSubCatlist(CategoryBox* pC)
{ // only making sence in main category-list
	m_SubCatBox = pC;
}
void CategoryBox::SetDatabase(lkSQL3Database* pDB)
{
	m_pDB = pDB;
}


bool CategoryBox::BuildCatList(bool bKeepSel)
{
	if ( !m_pDB || !m_pDB->IsOpen() )
		return false;

	wxString filter;
	filter.Printf(wxT("([%s] = 0)"), t3Category_subID);

	wxString order;
	order.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Category_NAME);

	if ( BuildList(filter, order, true, bKeepSel) )
	{
		if ( m_SubCatBox )
		{
			if ( bKeepSel )
			{
				wxUint64 curCat = 0;

				int curSel = GetSelection();
				if ( (curSel >= 0) && ((curCat = GetLParam((unsigned int)curSel)) != 0) )
				{
					m_SubCatBox->BuildSubList(curCat, bKeepSel);
					return true;
				}
				// else.. fallthrough
			}
			m_SubCatBox->BuildList(wxEmptyString, wxEmptyString, false, false);
		}
		return true;
	}
	return false;
}

bool CategoryBox::BuildSubList(wxUint64 nCat, bool bKeepSel)
{
	if ( !m_pDB || !m_pDB->IsOpen() )
		return false;

	wxString filter;
	filter.Printf(wxT("([%s] = %llu) AND ([%s] != 0)"), t3Category_catID, nCat, t3Category_subID);

	wxString order;
	order.Printf(wxT("[%s]"), t3Category_subID);

	return BuildList(filter, order, false, bKeepSel);
}

bool CategoryBox::BuildList(const wxString& filter, const wxString& order, bool bIsCat, bool bKeepSel)
{
	wxUint64 curCat = 0;

	if ( bKeepSel )
	{
		int curSel = GetSelection();
		if ( curSel >= 0 )
			curCat = GetLParam((unsigned int)curSel);
	}

// 	StartOver(); -- also will be called in lkImageBox::InitializeFromList
	wxString s;
	lkBoxInitList initList;
	initList.DeleteContents(true);

	// add first - empty item
	s = F_UNSET_STR;
	initList.Append(new lkBoxInit(s, 0));
	bool bRet = false;
	wxUint64 n;

	TCategory tCat;
	tCat.SetDatabase(m_pDB);
	tCat.SetFilter(filter);
	tCat.SetOrder(order);

	// then add all items from database
	try
	{
		tCat.Open();

		if ( !tCat.IsEmpty() )
		{
			tCat.MoveFirst();
			while ( !tCat.IsEOF() )
			{
				s = tCat.GetNameValue();
				n = wxUint64((bIsCat) ? tCat.GetCatValue() : tCat.GetSubValue());
//				m_CatList->Append(param = new CatParam(n, tCat.GetSortValue()));
				initList.Append(new lkBoxInit(s, tCat.GetImage(), n));

				tCat.MoveNext();
			}
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		// return false;
	}

	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		bRet = Populate();

	// select the item with lParam==curCat
	if ( bKeepSel )
		SetSelectedLParam((wxUint64)curCat);
	else
		SetSelection(0);

	return bRet;
}

//virtual
void CategoryBox::SetSelectedLParam(wxUint64 data)
{
	if ( IsListEmpty() )
		return;

	bool bFound = false;
	// find the first item who's LPARAM==data
	unsigned int i, cbCnt = GetCount();
	for ( i = 0; i < cbCnt; i++ )
		if ( GetLParam(i) == data )
		{
			bFound = true;
			break;
		}

	if ( bFound )
	{
		SetSelection(i); // unselect any item

		if ( m_nCurSel != i )
		{
			if ( m_SubCatBox )
				m_SubCatBox->BuildSubList(GetSelectedLParam(), false);
			m_nCurSel = i;
		}
	}
	else
	{
		SetSelection(wxNOT_FOUND);
		if ( m_SubCatBox )
			m_SubCatBox->BuildList(wxEmptyString, wxEmptyString, false, false);
	}
}

// ///////////////////////////////////////////////////////////////////////////////////

void CategoryBox::OnCatSel(wxCommandEvent& event)
{
	if ( m_SubCatBox )
	{
		int curSel = event.GetSelection();
		// rebuild sub-categories according new category
		if ( m_nCurSel != curSel )
		{
			m_SubCatBox->BuildSubList(GetSelectedLParam(), false);
			m_nCurSel = curSel;
		}
	}

	event.Skip();
}

