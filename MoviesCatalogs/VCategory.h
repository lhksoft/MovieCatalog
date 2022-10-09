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
#ifndef __V_CATEGORY_H__
#define __V_CATEGORY_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
//#include "lkSQL3RecordView.h"

#include "TCategory.h"

class CategoryDocument : public lkSQL3RecordDocument, public TCategory
{
public:
	CategoryDocument(wxDocument* pParent);

// Attributes
public:
	wxUint32						m_curCat; // required by SubCategory listbox

protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual bool					LoadData(void) wxOVERRIDE; // reads current row into m_Col
	virtual void					DoThingsBeforeUpdate(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

public:
	virtual wxString				GetDefaultFilter(void) const wxOVERRIDE;
	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(CategoryDocument);
	wxDECLARE_NO_COPY_CLASS(CategoryDocument);
};

class CategoryView : public lkSQL3RecordView
{
public:
	CategoryView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					OnUpdate(wxView* sender, wxObject* hint = NULL) wxOVERRIDE;

// Events
protected:
	void							OnSubAdd(wxCommandEvent& event);
	void							OnSubEdit(wxCommandEvent& event);
	void							OnSubDel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(CategoryView);
};

// /////////////////////////////////////////////////////////////////////////////////////////////

class CategoryTemplate : public lkChildDocTemplate
{
public:
	CategoryTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument*				DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate*			CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(CategoryTemplate);
	wxDECLARE_NO_COPY_CLASS(CategoryTemplate);
};

#endif // !__V_CATEGORY_H__
