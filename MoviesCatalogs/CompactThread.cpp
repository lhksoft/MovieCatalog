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
#include "CompactThread.h"
#include "CompactEvent.h"

#include "Compact.h"

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
#include "TTemp.h"

#include "DlgBaseFilter.h"
#include "DlgStorageFilter.h"
#include "DlgMoviesFilter.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#include "../lkControls/lkConfigTools.h"

CompactThread::CompactThread(wxWindow* parent, const wxString& path) : wxThread(wxTHREAD_DETACHED), m_SrcDB(), m_TmpDB(), m_DstDB(), m_Path(), m_Error()
{
	wxASSERT(parent && parent->IsKindOf(wxCLASSINFO(CompactFrame)));

	m_Path = path.Clone();
	m_TotalRecords = 0;
	m_pParent = parent;

	m_bDryRun = (GetConfigInt(conf_COMPACT_PATH, conf_COMPACT_DRY) == 1) ? true : false;
}
CompactThread::~CompactThread()
{
	m_SrcDB.Close();
	// not calling close of m_DstDB, when we come here and not especially called Commit, then m_DstDB will be Discarded in its destructor
	CompactFrame* f = dynamic_cast<CompactFrame*>(m_pParent);
	if ( f )
	{
		wxCriticalSectionLocker enter(f->m_pThreadCS);
		// the thread is being destroyed; make sure not to leave dangling pointers around
		f->m_pThread = NULL;
	}
}

void CompactThread::SetErrString(const wxString& err)
{
	m_Error = err;
}

//virtual
wxThread::ExitCode CompactThread::Entry()
{
	SetName("CompactThread");

	if ( !Initialize() )
	{
		EndInfo(TestDestroy() ? 2 : 1); // error
		return wxThread::ExitCode(1);
	}

	if ( !DoActual() )
	{
		EndInfo(TestDestroy() ? 2 : 1); // error
		return wxThread::ExitCode(1);
	}

	if ( TestDestroy() )
	{
		EndInfo(2); // error
		return wxThread::ExitCode(1);
	}
	// else
	bool bOK = m_SrcDB.Close();
	if ( !m_bDryRun && bOK )
		bOK = m_DstDB.Commit();
	m_TmpDB.Close();

	if ( !bOK )
	{
		EndInfo(3);
		return wxThread::ExitCode(1);
	}
	// else
	EndInfo(0);
	return wxThread::ExitCode(0);
}

bool CompactThread::Initialize()
{
	SendInfoMain(wxT("Initializing.."));
	SendInfoSec(wxEmptyString);

	wxUint64 totalRecords = 0;
	try
	{
		m_SrcDB.Open(m_Path);

		// Validate SourceDB
		if (!TestDestroy())
			if (!ValidateDatabase())
				return false;

		// Count amount of records to process
		if (!TestDestroy())
			if ((totalRecords = CountRecords()) == 0)
				return false;

		if (!m_bDryRun)
		{
			m_DstDB.OpenAsTemp(m_Path);
			m_TmpDB.Open(wxT(":memory:"), lkSQL3Database::ofCreate | lkSQL3Database::ofMemory | lkSQL3Database::ofDelOnClose);

			// Create new tables in DestDB
			if (!TestDestroy())
				if (!CreateTables())
					return false;
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		m_Error = ((lkSQL3Exception*)&e)->GetError();
		return false;
	}

	if ( TestDestroy() )
		return false;

	m_TotalRecords = totalRecords;
	InitProgressCtrl(1, totalRecords+7); // 7: 6 actions to perform at the end for correcting the Config file + 1 for rewriting the covers

	return true;
}

void CompactThread::SendInfoMain(const wxString& msg)
{
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_information_Main);
	event->SetString(msg);
	wxQueueEvent(m_pParent, event);
}
void CompactThread::SendInfoSec(const wxString& msg)
{
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_information_Sec);
	event->SetString(msg);
	wxQueueEvent(m_pParent, event);
}

void CompactThread::EndInfo(int nResult)
{
	wxString Msg;
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_threadCompleted);
	if ( nResult != 0 )
		Msg = m_Error;

	event->SetString(Msg);
	event->SetInt(nResult);
	if ( m_pParent )
		wxQueueEvent(m_pParent, event);
}

void CompactThread::SendInfoProcessing(const wxString& s)
{
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_infoProcessing);
	event->SetString(s);
	wxQueueEvent(m_pParent, event);
}
void CompactThread::SendInfoRewriting(const wxString& s)
{
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_infoRewriting);
	event->SetString(s);
	wxQueueEvent(m_pParent, event);
}

void CompactThread::InitProgressCtrl(int i, wxUint64 t)
{
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_progressInit);
	event->SetInt(i);

	event->SetLongLong(t);
	wxQueueEvent(m_pParent, event);
}
void CompactThread::StepProgressCtrl(int i)
{ // int=1:step in total records, int=2: step in current table, int=0:step both
	CompactEvent* event = new CompactEvent(CompactEvent::EventIDs::ID_progressStep);
	event->SetInt(i);
	wxQueueEvent(m_pParent, event);
}

bool CompactThread::CreateTables()
{
	wxASSERT(!m_bDryRun); // be sure not to come here if in dry mode
	if ( !m_DstDB.IsOpen() ) return false;

	bool bRet = true;
	if ( bRet ) bRet = TBase::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TCategory::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TCountry::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TGenre::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TGroup::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TJudge::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TLocation::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TMedium::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TMovies::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TOrigine::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TQuality::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TRating::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TScreen::CreateTable(&m_DstDB);
	if ( bRet ) bRet = TStorage::CreateTable(&m_DstDB);

	if ( !bRet )
		m_Error = wxT("Failure while creating tables in Destination.");

	return bRet;
}

bool CompactThread::ValidateDatabase()
{
	if ( !m_SrcDB.IsOpen() ) return 0;

	bool bRet = true;
	if ( bRet ) bRet = TBase::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TCategory::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TCountry::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TGenre::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TGroup::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TJudge::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TLocation::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TMedium::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TMovies::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TOrigine::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TQuality::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TRating::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TScreen::VerifyTable(&m_SrcDB);
	if ( bRet ) bRet = TStorage::VerifyTable(&m_SrcDB);

	if ( !bRet )
		m_Error = wxT("Failure while validating Source");

	return bRet;
}

wxUint64 CompactThread::CountRecords()
{
	static wxString tables[] = { t3Base, t3Category, t3Country, t3Genre, t3Group, t3Judge, t3Location, t3Medium, t3Movies, t3Orig, t3Quality, t3Rating, t3Screen, t3Storage, wxEmptyString };

	wxUint64 total = 0;

	for ( int i = 0; !tables[i].IsEmpty(); i++ )
	{
		if ( TestDestroy() )
			break;

		total += (CountRecord(tables[i]));
	}
	return total;
}

wxUint64 CompactThread::CountRecord(const wxString& table)
{
	if ( table.IsEmpty() )
		return 0;

	wxString qry;
	wxUint64 total = 0;

	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s];"), table);
	total = (wxUint64)m_SrcDB.ExecuteSQLAndGet(qry).GetInt();

	return total;
}

bool CompactThread::DoActual()
{
	bool bOK = true;
	if (!m_bDryRun)
	{ // be sure not to do following if in dry mode
		SendInfoMain(wxT("Resorting Covers"));
		if (bOK) bOK = TMovies::RewriteCovers(this, &m_SrcDB);
		if (bOK) bOK = !TestDestroy();
		// Country
		SendInfoMain(wxT("Initializing.."));
		if (bOK) bOK = TCountry::Compact(this, &m_SrcDB, &m_DstDB);
		if (!bOK || TestDestroy()) return false;
		// == Requirements for TMovies
		SendInfoProcessing(t3Movies);
		if (bOK) bOK = TCategory::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TGenre::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TJudge::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TRating::Compact(this, &m_SrcDB, &m_DstDB);
		if (!bOK || TestDestroy()) return false;
		// == Requirements for TBase
		SendInfoProcessing(t3Base);
		if (bOK) bOK = TMovies::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TOrigine::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TQuality::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TScreen::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		// == Requirements for TStorage
		SendInfoProcessing(t3Storage);
		if (bOK) bOK = TLocation::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TMedium::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		// == Requirements for TGroup
		SendInfoProcessing(t3Group);
		if (bOK) bOK = TBase::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		if (bOK) bOK = TStorage::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		SendInfoMain(wxEmptyString);
		if (bOK) bOK = TGroup::Compact(this, &m_SrcDB, &m_DstDB, &m_TmpDB);
		if (!bOK || TestDestroy()) return false;
		SendInfoMain(wxT("Correcting Configuration File"));
		SendInfoSec(wxEmptyString);
		InitProgressCtrl(0, 6);
		try
		{
			TOrigine::CorrectConfig(&m_TmpDB);
			if (TestDestroy()) return false;
			StepProgressCtrl(0);
			TQuality::CorrectConfig(&m_TmpDB);
			if (TestDestroy()) return false;
			StepProgressCtrl(0);
			TScreen::CorrectConfig(&m_TmpDB);
			if (TestDestroy()) return false;
			StepProgressCtrl(0);
			StorageFilterDlg::CorrectConfig(&m_TmpDB);
			if (TestDestroy()) return false;
			StepProgressCtrl(0);
			MoviesFilterDlg::CorrectConfig(&m_TmpDB);
			if (TestDestroy()) return false;
			StepProgressCtrl(0);
			BaseFilterDlg::CorrectConfig(&m_TmpDB);
			StepProgressCtrl(0);
		}
		catch (const lkSQL3Exception& e)
		{
			SetErrString(((lkSQL3Exception*)&e)->GetError());
			return false;
		}
	}
	else
	{
		// do our testing what needs to be done when doing "dry-run"
		InitProgressCtrl(1, 1);
		SendInfoMain(wxT("Resorting Covers"));
		if (bOK) bOK = TMovies::RewriteCovers(this, &m_SrcDB);
		if (bOK) bOK = !TestDestroy();
	}
	return bOK;
}
