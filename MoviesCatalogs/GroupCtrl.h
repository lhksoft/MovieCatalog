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
#ifndef __GROUP_CTRL_H__
#define __GROUP_CTRL_H__
#pragma once

#include <wx/listctrl.h>
#include <wx/list.h>

#include "../lkSQLite3/lkSQL3Database.h"

///////////////////////////////////////////////////////////
// class GrpImgData
// For each row in TMedia, it will hold which id in the ImageList
// is the corresponding img. -1 would mean there's no image
// TMedia is small, not giving much overhead
class GrpImgData : public wxObject
{
public:
	GrpImgData();
	GrpImgData(wxUint64, int);

	wxUint64						m_RowId;
	int								m_Index;
private:
	wxDECLARE_CLASS(GrpImgData);
	wxDECLARE_NO_COPY_CLASS(GrpImgData);
};

WX_DECLARE_LIST(GrpImgData, GrpImgDataList);


///////////////////////////////////////////////////////////
// class GroupCtrl
////
class GroupCtrl : public wxListCtrl
{
public:
	GroupCtrl();
	GroupCtrl(lkSQL3Database*);
	GroupCtrl(lkSQL3Database*, wxWindow* parent, wxWindowID id, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);
	virtual ~GroupCtrl();

	bool							Create(wxWindow* parent, wxWindowID id, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

protected:
	lkSQL3Database*					m_pDB;
	GrpImgDataList					m_ImgData;

public:
	bool							FillImageList(void); // fills m_ImgList using TMedium
	void							SetData(wxUint64 nBaseID);

protected:
	int								FindMedia(wxUint64);

private:
	wxDECLARE_CLASS(GroupCtrl);
	wxDECLARE_NO_COPY_CLASS(GroupCtrl);
};

#endif // !__GROUP_CTRL_H__
