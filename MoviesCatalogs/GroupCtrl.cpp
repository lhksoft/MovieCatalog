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
#include "GroupCtrl.h"
#include "TMedium.h"
#include "QGroup.h"

#include <wx/listimpl.cpp>
#include "../lkControls/lkImageList.h"
#include "../lkSQLite3/lkDateTime.h"
#include "../lkSQLite3/lkSQL3Exception.h"

WX_DEFINE_LIST(GrpImgDataList);

///////////////////////////////////////////////////////////
// class GrpImgData
////
wxIMPLEMENT_CLASS(GrpImgData, wxObject)

GrpImgData::GrpImgData() : wxObject()
{
	m_RowId = 0;
	m_Index = wxNOT_FOUND;
}
GrpImgData::GrpImgData(wxUint64 rowid, int idx) : wxObject()
{
	m_RowId = rowid;
	m_Index = idx;
}



///////////////////////////////////////////////////////////
// class GroupCtrl
////
wxIMPLEMENT_CLASS(GroupCtrl, wxListCtrl)

GroupCtrl::GroupCtrl() : wxListCtrl(), m_ImgData()
{
	m_ImgData.DeleteContents(true);
	m_pDB = NULL;
}
GroupCtrl::GroupCtrl(lkSQL3Database* pDB) : wxListCtrl(), m_ImgData()
{
	m_ImgData.DeleteContents(true);
	m_pDB = pDB;
}
GroupCtrl::GroupCtrl(lkSQL3Database* pDB, wxWindow* parent, wxWindowID id, const wxSize& size, const wxValidator& validator) : wxListCtrl(), m_ImgData()
{
	m_ImgData.DeleteContents(true);
	m_pDB = pDB;

	(void)GroupCtrl::Create(parent, id, size, validator);
}

GroupCtrl::~GroupCtrl()
{
}

bool GroupCtrl::Create(wxWindow* parent, wxWindowID id, const wxSize& size, const wxValidator& validator)
{
	if ( !wxListCtrl::Create(parent, id, wxDefaultPosition, size, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VRULES, validator) )
		return false;

	// Create columns
	AppendColumn(wxT("Storage"), wxLIST_FORMAT_LEFT, 105);
	AppendColumn(wxT("Part"), wxLIST_FORMAT_CENTER, 40);
	AppendColumn(wxT("Duration"), wxLIST_FORMAT_RIGHT, 60);
	AppendColumn(wxT("Location"), wxLIST_FORMAT_LEFT, 170);
	AppendColumn(wxT("Info"), wxLIST_FORMAT_CENTER, 50);

	return true;
}

bool GroupCtrl::FillImageList()
{
	if ( !m_pDB || !m_pDB->IsOpen() )
		return false;

	lkImageList* imgList = new lkImageList(16, 16);

	if ( !m_ImgData.IsEmpty() ) m_ImgData.Clear();

	TMedium rs(m_pDB);
	rs.SetOrder(wxT("[ROWID]"));
	try
	{
		rs.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	wxImage img;
	int idx;
	while ( !rs.IsEOF() )
	{
		if ( (img = rs.GetImage()).IsOk() && ((idx = imgList->Add(img)) != wxNOT_FOUND) )
			m_ImgData.Append(new GrpImgData(rs.GetActualCurRow(), idx));
		rs.MoveNext();
	}
	AssignImageList(imgList, wxIMAGE_LIST_SMALL);
	return true;
}


void GroupCtrl::SetData(wxUint64 nBaseID)
{ // (re-)builds GroupCtrl
	wxASSERT(m_pDB != NULL);
	// start over
	DeleteAllItems();

	QGroup RS(m_pDB);
	try
	{
		RS.SetBaseFilter(nBaseID);
		RS.Open();
		if ( !RS.IsEmpty() )
			RS.MoveFirst();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return;
	}

	if ( !RS.IsEmpty() )
	{
		lkDateTimeSpan dt;
		int idx, i = 0, info;
		long ds, hs;
		double d;
		wxString s;

		while ( !RS.IsEOF() )
		{
			wxListItem item;
			// 1st column: Storage and Icon
			item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
			item.SetId(i++);
			item.SetColumn(0);

			item.SetText(RS.GetStorageValue());
			item.SetImage(FindMedia(RS.GetMediumValue()));

			if ( (idx = InsertItem(item)) != -1 )
			{ // insert sub-columns too
			  // Part
				item.SetMask(wxLIST_MASK_TEXT);
				item.SetId(idx);
				item.SetColumn(1);
				item.SetText(RS.GetPartValue());
				SetItem(item);

				// Duration/PlayingTime
				if ( (d = RS.GetTimeValue()) != 0.0 )
				{
					dt = d;
					ds = dt.GetDays();
					hs = dt.GetHours();
					if ( ds > 0 )
						hs += (ds * 24);
					if ( hs > 0 )
						s.Printf(wxT("%ld:%02ld:%02ld"), hs, dt.GetMinutes(), dt.GetSeconds());
					else
						s.Printf(wxT("%ld:%02ld"), dt.GetMinutes(), dt.GetSeconds());

					item.SetMask(wxLIST_MASK_TEXT);
					item.SetId(idx);
					item.SetColumn(2);
					item.SetText(s);
					SetItem(item);
				}

				// Location
				item.SetMask(wxLIST_MASK_TEXT);
				item.SetId(idx);
				item.SetColumn(3);
				item.SetText(RS.GetLocationValue());
				SetItem(item);

				if ( (info = RS.GetInfoValue()) != 0 )
				{ // Extra
					s.Printf(wxT("%d"), info);

					item.SetMask(wxLIST_MASK_TEXT);
					item.SetId(idx);
					item.SetColumn(4);
					item.SetText(s);
					SetItem(item);
				}
			}

			RS.MoveNext();
		}
	}
	Refresh(true);
}

int GroupCtrl::FindMedia(wxUint64 m)
{
	GrpImgDataList::iterator iter;
	for ( iter = m_ImgData.begin(); iter != m_ImgData.end(); iter++ )
	{
		GrpImgData* current = *iter;
		if ( current->m_RowId == m ) return current->m_Index;
	}
	return wxNOT_FOUND;
}
