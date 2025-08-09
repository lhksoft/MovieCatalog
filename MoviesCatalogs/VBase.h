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
#ifndef __V_BASE_H__
#define __V_BASE_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
#include "TBase.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseDocument
////
class BaseDocument : public lkSQL3RecordDocument, public TBase
{
public:
	BaseDocument(wxDocument* pParent);

	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()

	bool							IsCountryChanged(void) const;
	void							SetCountryChanged(bool);

	bool							IsQualityChanged(void) const;
	void							SetQualityChanged(bool);

	bool							IsOrigineChanged(void) const;
	void							SetOrigineChanged(bool);

	bool							IsScreenChanged(void) const;
	void							SetScreenChanged(bool);

	bool							IsMediumChanged(void) const;
	void							SetMediumChanged(bool);

	virtual void					BindButtons(void) wxOVERRIDE;

// Attributes
protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

	bool							m_bCountryChanged;
	bool							m_bQualityChanged;
	bool							m_bOrigineChanged;
	bool							m_bScreenChanged;
	bool							m_bMediumChanged;

	void							SetFiltering(const wxString& from, const wxString& filter, const wxString& order);
	void							SetFiltering(bool);

// Events
protected:
	void							OnUpdateFilter(wxUpdateUIEvent& event);
	void							OnUpdateFilterToggle(wxUpdateUIEvent& event);

	void							OnSearch(wxCommandEvent& event);
	void							OnFilter(wxCommandEvent& event);
	void							OnFilterToggle(wxCommandEvent& event);
	void							OnSelectMovie(wxCommandEvent& event);
	void							OnGrouping(wxCommandEvent& event);

	void							OnUpdateGrouping(wxUpdateUIEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(BaseDocument);
	wxDECLARE_NO_COPY_CLASS(BaseDocument);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseView
////
class BaseView : public lkSQL3RecordView
{
public:
	BaseView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					InitialUpdate(void) wxOVERRIDE;

protected:
	virtual void					ProcessRecordset(lkSQL3RecordSet*) wxOVERRIDE;

// Overrides
protected:
	// Required for setting config-options
	virtual wxString				GetConfigPath(void) const wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(BaseView);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseTemplate
////
class BaseTemplate : public lkChildDocTemplate
{
public:
	BaseTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument*				DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate*			CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(BaseTemplate);
	wxDECLARE_NO_COPY_CLASS(BaseTemplate);
};

#endif // !__V_BASE_H__
