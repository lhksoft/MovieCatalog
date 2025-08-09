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
#include "MainDocument.h"
#include "MainDocManager.h"

#include "TBase.h"
#include "TCategory.h"
#include "TCountry.h"
#include "TGenre.h"
#include "TJudge.h"
#include "TLocation.h"
#include "TMedium.h"
#include "TMovies.h"
#include "TOrigine.h"
#include "TQuality.h"
#include "TRating.h"
#include "TScreen.h"
#include "TStorage.h"
#include "TGroup.h"

#include <wx/config.h>
#include "../lkSQLite3/lkSQL3Exception.h"
#include <wx/msgdlg.h>


// /////////////////////////////////////////////////////////////////////////////////////////////
// class lkStringObject
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS(lkStringObject, wxObject)

lkStringObject::lkStringObject(const wxString& s) : wxObject(), m_String(s)
{
}
wxString lkStringObject::GetString() const
{
	return m_String;
}
/*
// /////////////////////////////////////////////////////////////////////////////////////////////
// class MainDocManager
// /////////////////////////////////////////////////////////////////////////////////////////////

MainDocManager::MainDocManager(long flags, bool initialize) : wxDocManager(flags, initialize)
{
}

//virtual
wxDocument* MainDocManager::CreateDocument(const wxString& path, long flags)
{
	return wxDocManager::CreateDocument(path, flags);
}

//virtual
void MainDocManager::FileHistoryLoad(const wxConfigBase& config)
{
	if ( m_fileHistory )
	{
		wxConfigBase* pConfig = (wxConfigBase*)&config;
		pConfig->SetPath(wxT("/MRU"));
		m_fileHistory->Load(config);
		pConfig->SetPath(wxT("/"));
	}
}

//virtual
void MainDocManager::FileHistorySave(wxConfigBase& config)
{
	if ( m_fileHistory )
	{
		config.SetPath(wxT("/MRU"));
		m_fileHistory->Save(config);
		config.SetPath(wxT("/"));
	}
}

MainDocument* MainDocManager::GetMainDocument()
{ // returns the Parent-Doc for all child-doc's
	wxList::compatibility_iterator node = m_docs.GetFirst();
	while ( node )
	{
		wxDocument* doc = (wxDocument*)node->GetData();
		wxList::compatibility_iterator next = node->GetNext();

		if ( doc && doc->IsKindOf(wxCLASSINFO(MainDocument)) )
			return dynamic_cast<MainDocument*>(doc);

		node = next;
	}
	return NULL;
}
*/
// /////////////////////////////////////////////////////////////////////////////////////////////
// class MainDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(MainDocument, lkDocument)

MainDocument::MainDocument() : lkDocument(), m_DB()
{
	m_bValidDB = false;
}
MainDocument::~MainDocument()
{
	CloseDataBase();
}

lkSQL3Database* MainDocument::GetDB()
{
	return &m_DB;
}

// ////////////////////////////////////////////////////////
// Operations
// ////////////////////////////////////////////////////////

bool MainDocument::CreateNewDatabase(const wxString& path)
{
	if ( path.IsEmpty() )
		return false;

	// remove the old file before creating the new one
	if ( wxFileExists(path) )
		wxRemoveFile(path);

	bool bRet = true;
	try
	{
		m_DB.Open(path, lkSQL3Database::ofCreate);
		if ( !(m_bValidDB = CreateTables()) )
			bRet = false;
		else
		{
			m_DB.Close();
			m_DB.Open(path);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		m_bValidDB = false;
		bRet = false;
	}

	if ( !bRet )
	{
		if ( m_DB.IsOpen() )
			m_DB.Close();
		if ( wxFileExists(path) )
			wxRemoveFile(path);
	}

	return bRet;
}

void MainDocument::CloseDataBase()
{
	// to close all recordset
	try
	{
		if ( m_DB.IsOpen() ) m_DB.Close();
		m_bValidDB = false;
	}
	catch ( const lkSQL3Exception& )
	{
		// should not throw anything, might be called from inside destructor
	}
}

bool MainDocument::IsOk() const
{
	return (m_DB.IsOpen() && m_bValidDB);
}


// ////////////////////////////////////////////////////////
// used internally
// ////////////////////////////////////////////////////////

bool MainDocument::CreateTables()
{
	if ( !m_DB.IsOpen() ) return 0;

	bool bRet = true;
	if ( bRet ) bRet = TBase::CreateTable(&m_DB);
	if ( bRet ) bRet = TCategory::CreateTable(&m_DB);
	if ( bRet ) bRet = TCountry::CreateTable(&m_DB);
	if ( bRet ) bRet = TGenre::CreateTable(&m_DB);
	if ( bRet ) bRet = TGroup::CreateTable(&m_DB);
	if ( bRet ) bRet = TJudge::CreateTable(&m_DB);
	if ( bRet ) bRet = TLocation::CreateTable(&m_DB);
	if ( bRet ) bRet = TMedium::CreateTable(&m_DB);
	if ( bRet ) bRet = TMovies::CreateTable(&m_DB);
	if ( bRet ) bRet = TOrigine::CreateTable(&m_DB);
	if ( bRet ) bRet = TQuality::CreateTable(&m_DB);
	if ( bRet ) bRet = TRating::CreateTable(&m_DB);
	if ( bRet ) bRet = TScreen::CreateTable(&m_DB);
	if ( bRet ) bRet = TStorage::CreateTable(&m_DB);

	return bRet;
}

bool MainDocument::ValidateDatabase()
{
	if ( !m_DB.IsOpen() ) return 0;

	bool bRet = true;
	if ( bRet ) bRet = TBase::VerifyTable(&m_DB);
	if ( bRet ) bRet = TCategory::VerifyTable(&m_DB);
	if ( bRet ) bRet = TCountry::VerifyTable(&m_DB);
	if ( bRet ) bRet = TGenre::VerifyTable(&m_DB);
	if ( bRet ) bRet = TGroup::VerifyTable(&m_DB);
	if ( bRet ) bRet = TJudge::VerifyTable(&m_DB);
	if ( bRet ) bRet = TLocation::VerifyTable(&m_DB);
	if ( bRet ) bRet = TMedium::VerifyTable(&m_DB);
	if ( bRet ) bRet = TMovies::VerifyTable(&m_DB);
	if ( bRet ) bRet = TOrigine::VerifyTable(&m_DB);
	if ( bRet ) bRet = TQuality::VerifyTable(&m_DB);
	if ( bRet ) bRet = TRating::VerifyTable(&m_DB);
	if ( bRet ) bRet = TScreen::VerifyTable(&m_DB);
	if ( bRet ) bRet = TStorage::VerifyTable(&m_DB);

	return bRet;
}

// ////////////////////////////////////////////////////////
// Overrides
// ////////////////////////////////////////////////////////

//virtual
bool MainDocument::OnNewDocument(const wxString& sPath)
{
	wxASSERT(!sPath.IsEmpty());

	if ( CreateNewDatabase(GetFilename()) )
	{
		SetDocumentSaved(true);

		SetTitle(wxFileNameFromPath(sPath));
		SetFilename(sPath, true);

		// this one required to set initial data in Main View
		lkStringObject strObj(sPath);
		UpdateAllViews(NULL, &strObj);
		return true;
	}
	return false;
}

bool MainDocument::OnOpenDocument(const wxString& file)
{
	try
	{
		m_DB.Open(file);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( !(m_bValidDB = ValidateDatabase()) )
	{
		CloseDataBase();
		wxMessageBox(wxT("This is not a valid database."), wxT("Invalid file"), wxICON_WARNING | wxOK);
		return false;
	}
	// else...

	// calling parent will call DoOpenDocument (ours) which will inform the main view of the just yet opened database
	lkDocument::OnOpenDocument(file);

	return m_bValidDB;
}

//virtual
bool MainDocument::OnCloseDocument()
{
	if ( lkDocument::OnCloseDocument() )
	{
		CloseDataBase();
		return true;
	}

	return false;
}

//virtual
bool MainDocument::DoSaveDocument(const wxString& file)
{
	return true;
}

//virtual
bool MainDocument::DoOpenDocument(const wxString& file)
{
	// this one required to set initial data in Main View
	lkStringObject strObj(file);
	UpdateAllViews(NULL, &strObj);
	return true;
}

