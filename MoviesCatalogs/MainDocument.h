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
#ifndef __MAIN_DOCUMENT_H__
#define __MAIN_DOCUMENT_H__
#pragma once

#include <wx/docview.h>
#include "../lkSQLite3/lkSQL3Database.h"
#include "../lkControls/lkDocument.h"

class MainDocument : public lkDocument
{
	wxDECLARE_DYNAMIC_CLASS(MainDocument);
protected:
	// create from serialization only
	MainDocument();

public:
	virtual ~MainDocument();

// Attributes
protected:
	lkSQL3Database				m_DB;
	bool						m_bValidDB; // TRUE as soon the DB gets validated

public:
	lkSQL3Database*				GetDB(void);

// Operations
public:
	bool						CreateNewDatabase(const wxString& path);
	void						CloseDataBase(); // called in 'OnCloseDocument' and 'DeleteContents'

	bool						IsOk(void) const; // true if IsOpen && m_bValidDB==true
	

// used internally
protected:
	bool						CreateTables(void); // called in 'CreateNewDatabase'
	bool						ValidateDatabase(void); // called in 'OnFileOpen'

// Overrides
public:
//	virtual bool 				OnNewDocument(void) wxOVERRIDE;
	virtual bool 				OnNewDocument(const wxString& sPath) wxOVERRIDE;
	virtual bool 				OnOpenDocument(const wxString& filename) wxOVERRIDE;
//	virtual bool				OnSaveDocument(const wxString& filename) wxOVERRIDE;
	virtual bool 				OnCloseDocument(void) wxOVERRIDE;

	// Nothing should be done in here .. just return true
	virtual bool				DoSaveDocument(const wxString& file) wxOVERRIDE;
	virtual bool				DoOpenDocument(const wxString& file) wxOVERRIDE;

};


// /////////////////////////////////////////////////////////////////////////////////////////////
// class lkStringObject
// -- a Helper class, used in MainDocument and in lkMainView
// /////////////////////////////////////////////////////////////////////////////////////////////

class lkStringObject : public wxObject
{
	wxDECLARE_ABSTRACT_CLASS(lkStringObject);

public:
	lkStringObject(const wxString&);

	wxString GetString(void) const;

private:
	wxString m_String;
};



#endif // !__MAIN_DOCUMENT_H__
