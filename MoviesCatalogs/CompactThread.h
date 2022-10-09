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
#ifndef __COMPACT_THREAD_H__
#define __COMPACT_THREAD_H__
#pragma once

#include <wx/thread.h>
#include <wx/window.h>
#include "../lkSQLite3/lkSQL3Database.h"
#include "../lkSQLite3/lkSQL3TempDB.h"

class CompactThread : public wxThread
{
public:
	CompactThread(wxWindow* parent, const wxString& path);
	virtual ~CompactThread();

	void						SetErrString(const wxString&);
	void						SendInfoMain(const wxString& msg);
	void						SendInfoSec(const wxString& msg);
	void						SendInfoProcessing(const wxString&);
	void						SendInfoRewriting(const wxString&);
	// Informs caller tot initialise the progressCtrl
	void						InitProgressCtrl(int, wxUint64);
	void						StepProgressCtrl(int); // int=1:step in total records, int=2: step in current table, int=0:step both

protected:
	virtual ExitCode			Entry() wxOVERRIDE;

	bool						Initialize(void);

	void						EndInfo(int nResult = 0);

	 // create new tables in m_DstDB
	bool						CreateTables(void);
	 // validates m_SrcDB
	bool						ValidateDatabase(void);
	 // Counts total records to be processed
	wxUint64					CountRecords(void); // of all tables
	wxUint64					CountRecord(const wxString& table); // of given table

	 // Main Process
	bool						DoActual(void);

private:
	wxString					m_Error;
	wxUint64					m_TotalRecords;			// total records of all tables to process
	lkSQL3Database				m_SrcDB, m_TmpDB;
	lkSQL3TempDB				m_DstDB;
	wxString					m_Path;

	wxWindow*					m_pParent; // the window that will receive our CompactEvent's

	wxDECLARE_NO_COPY_CLASS(CompactThread);
};

#endif // !__COMPACT_THREAD_H__
