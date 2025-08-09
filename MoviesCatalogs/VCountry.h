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
#ifndef __V_COUNTRY_H__
#define __V_COUNTRY_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
#include "TCountry.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryDocument
////
class CountryDocument : public lkSQL3RecordDocument, public TCountry
{
public:
	CountryDocument(wxDocument* pParent);

// Attributes
protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

public:
	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE;
	virtual void					BindButtons(void) wxOVERRIDE;

// Events
protected:
	void							OnSearch(wxCommandEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(CountryDocument);
	wxDECLARE_NO_COPY_CLASS(CountryDocument);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryView
////
class CountryView : public lkSQL3RecordView
{
public:
	CountryView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					OnUpdate(wxView* sender, wxObject* hint = NULL) wxOVERRIDE;

// Overrides
protected:
	// Required for setting config-options
	virtual wxString				GetConfigPath(void) const wxOVERRIDE;

protected:

	wxDECLARE_EVENT_TABLE();

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(CountryView);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryTemplate
////
class CountryTemplate : public lkChildDocTemplate
{
public:
	CountryTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument*				DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate*			CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(CountryTemplate);
	wxDECLARE_NO_COPY_CLASS(CountryTemplate);
};

#endif // !__V_COUNTRY_H__
