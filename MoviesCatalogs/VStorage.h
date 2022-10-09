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
#ifndef __V_STORAGE_H__
#define __V_STORAGE_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
#include "TStorage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageDocument
////
class StorageDocument : public lkSQL3RecordDocument, public TStorage
{
public:
	StorageDocument(wxDocument* pParent);

	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()

	bool							MediumChanged(void) const;
	void							SetMediumChanged(bool);

	bool							LocationChanged(void) const;
	void							SetLocationChanged(bool);

// Attributes
protected:
	bool							m_bMedium, m_bLocation; // set to true in OnUpdate

	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

public:
	virtual void					BindButtons(void) wxOVERRIDE;

// Events
protected:
	void							OnUpdateFilter(wxUpdateUIEvent& event);
	void							OnUpdateFilterToggle(wxUpdateUIEvent& event);

	void							OnSearch(wxCommandEvent& event);
	void							OnFilter(wxCommandEvent& event);
	void							OnFilterToggle(wxCommandEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(StorageDocument);
	wxDECLARE_NO_COPY_CLASS(StorageDocument);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageView
////
class StorageView : public lkSQL3RecordView
{
public:
	StorageView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					InitialUpdate(void) wxOVERRIDE;

protected:
	virtual void					ProcessRecordset(lkSQL3RecordSet*) wxOVERRIDE;

	void							OnUpdateNextID(wxUpdateUIEvent& event);
	void							OnNextID(wxCommandEvent& event);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(StorageView);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenTemplate
////
class StorageTemplate : public lkChildDocTemplate
{
public:
	StorageTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument* DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate* CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(StorageTemplate);
	wxDECLARE_NO_COPY_CLASS(StorageTemplate);
};

#endif // !__V_STORAGE_H__
