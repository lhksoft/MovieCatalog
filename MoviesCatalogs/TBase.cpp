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
#include "TBase.h"
#include "TMovies.h"	// required for FindFilter and in Compact, needs table-name
#include "TStorage.h"
#include "TGroup.h"

#include "TQuality.h"	// required in Compact, needs table-name
#include "TOrigine.h"	// required in Compact, needs table-name
#include "TScreen.h"	// required in Compact, needs table-name
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3FindTable.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3BaseFlds[] = {
	{ t3Base_MOVIE,   sqlFieldType::sqlInt,  true,  false, 0 },
	{ t3Base_QUALITY, sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Base_ORIGINE, sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Base_SCREEN,  sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Base_AUDIO,	 sqlFieldType::sqlText, false, false,  0 },
	{ t3Base_SUBS,	 sqlFieldType::sqlText, false, false,  0 },
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3BaseTbl = { t3Base, t3BaseFlds, NULL };

///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(TBase, lkSQL3TableSet)

TBase::TBase() : lkSQL3TableSet(), m_sFltFrom()
{
	Init();
}
TBase::TBase(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sFltFrom()
{
	Init();
}

void TBase::Init()
{
	m_pFldMovies = NULL;
	m_pFldQuality = NULL;
	m_pFldOrigine = NULL;
	m_pFldScreen = NULL;
	m_pFldAudio = NULL;
	m_pFldSubs = NULL;
}

void TBase::SetFltFrom(const wxString& q)
{
	m_sFltFrom = q;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
wxString TBase::GetQuery(bool _rowid, bool _filter, bool _order)
{
	wxString qry;
	if ( _rowid && m_bFilterActive && !m_sFltFrom.IsEmpty() )
	{
		wxString s;
		qry.Printf(wxT("SELECT [%s].%s FROM %s"), t3Base, m_Rowid, m_sFltFrom);

		wxString whr;
		if ( _filter && !m_Filter.IsEmpty() )
		{
			if ( !m_sFltFilter.IsEmpty() )
				whr.Printf(wxT("(%s AND %s)"), m_sFltFilter, m_Filter);
			else
				whr = m_Filter;
		}
		else
			whr = m_sFltFilter;
		if ( !whr.IsEmpty() )
		{
			s = whr;
			whr.Printf(wxT(" HAVING %s"), s);
		}
		qry += whr;

		if ( _order )
		{
			s.Empty();
			if ( !m_sFltOrder.IsEmpty() )
				s.Printf(wxT(" ORDER BY %s"), m_sFltOrder);
			qry += s;
		}
	}
	else
		qry = lkSQL3TableSet::GetQuery(_rowid, _filter, _order);

	return qry;
}

//virtual
bool TBase::Open()
{
	lkInitTable msInit(&t3BaseTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Base_MOVIE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldMovies));
	msInit.Append(new lkInitField(t3Base_QUALITY, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldQuality));
	msInit.Append(new lkInitField(t3Base_ORIGINE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldOrigine));
	msInit.Append(new lkInitField(t3Base_SCREEN, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldScreen));
	msInit.Append(new lkInitField(t3Base_AUDIO, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldAudio));
	msInit.Append(new lkInitField(t3Base_SUBS, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldSubs));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Base);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TBase::CanDelete()
{
	bool bCanDel = lkSQL3TableSet::CanDelete();
	if ( bCanDel )
		bCanDel = !(TGroup::HasBase(m_DB, GetActualCurRow()));
	return bCanDel;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
////

wxUint64 TBase::GetMoviesID() const
{
	wxASSERT(m_pFldMovies);
	return m_pFldMovies->GetValue2();
}
void TBase::SetMoviesID(wxUint64 m)
{
	wxASSERT(m_pFldMovies);
	m_pFldMovies->SetValue2(m);
}

wxUint64 TBase::GetQualityID() const
{
	wxASSERT(m_pFldQuality);
	return m_pFldQuality->GetValue2();
}
void TBase::SetQualityID(wxUint64 q)
{
	wxASSERT(m_pFldQuality);
	m_pFldQuality->SetValue2(q);
}

wxUint64 TBase::GetOrigineID() const
{
	wxASSERT(m_pFldOrigine);
	return m_pFldOrigine->GetValue2();
}
void TBase::SetOrigineID(wxUint64 o)
{
	wxASSERT(m_pFldOrigine);
	m_pFldOrigine->SetValue2(o);
}

wxUint64 TBase::GetScreenID() const
{
	wxASSERT(m_pFldScreen);
	return m_pFldScreen->GetValue2();
}
void TBase::SetScreenID(wxUint64 s)
{
	wxASSERT(m_pFldScreen);
	m_pFldScreen->SetValue2(s);
}

wxString TBase::GetAudioValue() const
{
	wxASSERT(m_pFldAudio);
	return m_pFldAudio->GetValue2();
}
void TBase::SetAudioValue(const wxString& a)
{
	wxASSERT(m_pFldAudio);
	m_pFldAudio->SetValue2(a);
}

wxString TBase::GetSubsValue() const
{
	wxASSERT(m_pFldSubs);
	return m_pFldSubs->GetValue2();
}
void TBase::SetSubsValue(const wxString& s)
{
	wxASSERT(m_pFldSubs);
	m_pFldSubs->SetValue2(s);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////

//virtual
wxUint64 TBase::FindInit(wxClassInfo* rc, const wxString& search_on, bool bForward)
{
	ResetFind();

	if ( IsEmpty() ) return 0;
	wxASSERT(!search_on.IsEmpty());

	wxUint64 ret = 0;
	wxString s, _qry;
	if ( !m_bFilterActive )
	{
		_qry = GetQuery(true, 0, 0);

		if ( !m_Filter.IsEmpty() )
			s.Printf(wxT(" WHERE ((%s) AND (%s))"), m_Filter, search_on);
		else
			s.Printf(_T(" WHERE (%s)"), search_on);
		_qry += s;
	}
	else
	{
		_qry.Printf(wxT("SELECT [%s].%s FROM %s"), t3Base, m_Rowid, m_sFltFrom);

		wxString whr;
		if ( !m_Filter.IsEmpty() )
		{
			if ( !m_sFltFilter.IsEmpty() )
				s.Printf(wxT("(%s AND %s)"), m_sFltFilter, m_Filter);
			else
				s = m_Filter;
		}
		else
			s = m_sFltFilter;
		whr.Printf(wxT(" HAVING (%s AND %s)"), s, search_on);
		_qry += whr;
	}

	s.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), _qry);

	TBase* rsOrig = NULL;
	lkSQL3FindTable* rsFind = NULL;
	try
	{
		if ( ((int)m_DB->ExecuteSQLAndGet(s)) > 0 )
		{
			m_FindDB = new lkSQL3Database();
			m_FindDB->Open(wxT(":memory:"), lkSQL3Database::enOpenFlags::ofCreate | lkSQL3Database::enOpenFlags::ofMemory);
			lkSQL3FindTable::CreateTable(m_FindDB);

			rsFind = new lkSQL3FindTable(m_FindDB);
			rsFind->Open();

			rsOrig = new TBase(m_DB);
			if ( !m_bFilterActive )
			{
				if ( !m_Filter.IsEmpty() )
					s.Printf(wxT("(%s) AND (%s)"), m_Filter, search_on);
				else
					s.Printf(wxT("(%s)"), search_on);
				rsOrig->SetFilter(s);
				if ( !m_Sort.IsEmpty() )
					s.Printf(wxT("(%s)"), m_Sort);
				else
					s = wxT("[ROWID]");
				rsOrig->SetOrder(s);
			}
			else
			{
				wxASSERT(!m_sFltFrom.IsEmpty());
				rsOrig->SetFltFrom(m_sFltFrom);
				rsOrig->SetFltOrder(m_sFltOrder);
				rsOrig->SetFilterActive(true);

				if ( !m_sFltFilter.IsEmpty() )
					s.Printf(wxT("(%s AND %s)"), m_sFltFilter, search_on);
				else
					s = search_on;
				rsOrig->SetFltFilter(s);
			}

			rsOrig->Open();
			rsOrig->MoveFirst();
			while ( !rsOrig->IsEOF() )
			{
				rsFind->AddNew();
				rsFind->SetOrigID(rsOrig->GetActualCurRow());
				rsFind->Update();
				rsOrig->MoveNext();
			}
			delete rsOrig;
			rsOrig = NULL;

			ret = (bForward) ? rsFind->FindFirst(m_FindRowid) : rsFind->FindLast(m_FindRowid);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		if ( rsOrig ) delete rsOrig;
		if ( rsFind ) delete rsFind;
		if ( m_FindDB ) delete m_FindDB; m_FindDB = NULL;
		return 0;
	}

	if ( rsOrig ) delete rsOrig;
	if ( rsFind ) delete rsFind;

	if ( ret > 0 ) // find index of this row
	{
		SetFindBools();

		if ( bForward )
		{
			for ( wxUint64 i = 0; i < m_RowCount; i++ )
				if ( m_pRowIDs[i] == ret )
					return ++i;
		}
		else
		{
			for ( wxUint64 i = (m_RowCount - 1); i >= 0; i-- )
				if ( m_pRowIDs[i] == ret )
					return ++i;
		}
	}

	return 0;
}


//virtual
void TBase::CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection)
{
	Filter.Empty();
	if ( Field.IsEmpty() )
		return;

	wxString qry, w;
	// we can search using Movies or Storage data
	if ( (Field[0] == 'm') || (Field[0] == 'M') )
	{
		w = TMovies::CreateMoviesFindFilter(Search, findDirection);
		qry.Printf(wxT("SELECT [%s].[ROWID] FROM [%s] LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID] WHERE %s"), 
				   t3Base, t3Base, t3Movies, t3Base, t3Base_MOVIE, t3Movies, w);
	}
	else if ( (Field[0] == 's') || (Field[0] == 'S') )
	{
		w = TStorage::CreateStorageFindFilter(Search, findDirection);
		qry.Printf(wxT("SELECT [%s].[ROWID] FROM [%s] LEFT JOIN ([%s] LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID]) ON [%s].[ROWID] = [%s].[%s] WHERE %s"),
				   t3Base, t3Base, t3Group, t3Storage, t3Group, t3Group_STORAGE, t3Storage, t3Base, t3Group, t3Group_BASE, w);
	}

	if ( !m_bFilterActive )
	{
		if ( !qry.IsEmpty() )
			Filter.Printf(wxT("([ROWID] IN (%s))"), qry);
	}
	else
		Filter = w;
}

//virtual
bool TBase::TestFind(const wxString& search_on, bool forward)
{
	if ( IsEmpty() ) return false;
	if ( search_on.IsEmpty() ) return false;

	bool bOK = false;

	wxString s, _qry, sOrder;
	if ( !m_bFilterActive )
	{
		if ( !m_Table.IsEmpty() )
			_qry.Printf(wxT("SELECT %s, %s FROM [%s]"), m_Rowid, m_Query, m_Table);
		else
		{
			if ( !m_View.IsEmpty() )
				_qry.Printf(wxT("SELECT %s, %s FROM (%s)"), m_Rowid, m_Query, m_View);
		}

		if ( !m_Filter.IsEmpty() )
			s.Printf(wxT(" WHERE ((%s) AND (%s))"), m_Filter, search_on);
		else
			s.Printf(wxT(" WHERE (%s)"), search_on);

		if ( !_qry.IsEmpty() )
			_qry += s;

		if ( !m_Sort.IsEmpty() )
		{
			wxString _sort = m_Sort;
			_sort.Replace(wxT("ROWID"), wxT("ID"), true);
			sOrder.Printf(wxT(" ORDER BY %s%s"), _sort, (!forward) ? _T(" DESC") : wxT(""));
		}
		else
			sOrder.Printf(wxT(" ORDER BY [ID]%s"), (!forward) ? _T(" DESC") : wxT(""));

		if ( !_qry.IsEmpty() )
		{
			s = _qry;
			_qry.Printf(wxT("SELECT * FROM (%s)%s"), s, sOrder);
		}
	}
	else
	{
		wxASSERT(!m_sFltFrom.IsEmpty());
		_qry.Printf(wxT("SELECT [%s].%s FROM %s"), t3Base, m_Rowid, m_sFltFrom);

		wxString whr;
		if ( !m_Filter.IsEmpty() )
		{
			if ( !m_sFltFilter.IsEmpty() )
				s.Printf(wxT("(%s AND %s)"), m_sFltFilter, m_Filter);
			else
				s = m_Filter;
		}
		else
			s = m_sFltFilter;
		whr.Printf(wxT(" HAVING (%s AND %s)"), s, search_on);
		_qry += whr;

		sOrder.Printf(wxT(" ORDER BY [ID]%s"), (!forward) ? wxT(" DESC") : wxT(""));
		_qry += sOrder;
	}

	if ( !_qry.IsEmpty() )
	{
		s.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), _qry);
		try
		{
			bOK = ((int)(m_DB->ExecuteSQLAndGet(s)) > 0);
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			bOK = false;
		}
	}
	return bOK;
}


//static
bool TBase::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3BaseTbl);
}

//static
bool TBase::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Base) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3BaseTbl);

	return bRet;
}


//static
bool TBase::HasCountry(lkSQL3Database* pDB, const wxString& iso, bool bAudio)
{
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() || iso.IsEmpty() )
		return bRet;

	wxString sql, qry, s, what;

	if ( bAudio )
		what = t3Base_AUDIO;
	else
		what = t3Base_SUBS;

	s = wxT("([%s] = '%s') OR ([%s] LIKE '%s %%') OR ([%s] LIKE '%% %s') OR ([%s] LIKE '%% %s %%')");
	qry.Printf(s, what, iso, what, iso, what, iso, what, iso);

	sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE (%s);"), t3Base, qry);
	try
	{
		bRet = ((wxUint64)pDB->ExecuteSQLAndGet(sql) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TBase::HasAudio(lkSQL3Database* pDB, const wxString& iso)
{
	return TBase::HasCountry(pDB, iso, true);
}
//static
bool TBase::HasSubtitle(lkSQL3Database* pDB, const wxString& iso)
{
	return TBase::HasCountry(pDB, iso, false);
}

//static
bool TBase::HasMovie(lkSQL3Database* pDB, wxUint64 movie)
{ // TRUE if at least 1 Movie match has been found
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Base, t3Base_MOVIE, movie);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TBase::HasQuality(lkSQL3Database* pDB, wxUint64 qlt)
{
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Base, t3Base_QUALITY, qlt);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TBase::HasOrigine(lkSQL3Database* pDB, wxUint64 orig)
{
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Base, t3Base_ORIGINE, orig);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TBase::HasScreen(lkSQL3Database* pDB, wxUint64 scr)
{
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Base, t3Base_SCREEN, scr);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TBase::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TBase rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	wxUint64 oldID, newID, oldBaseID;
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Base);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Base);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			oldBaseID = rsSrc.GetActualCurRow();
			
			if ( TGroup::HasBase(dbSrc, oldBaseID) && 
				((oldID = rsSrc.GetMoviesID()) > 0) && ((newID = TTemp::FindNewID(dbTmp, t3Movies, oldID)) > 0) )
			{ // Add only if linked in TGroup             ^^ if it happens where newID==0, this table properly is corrupt !!
				rsDest.AddNew();

				// Movies ID
				rsDest.SetMoviesID(newID);
				// Quality ID
				if ( (oldID = rsSrc.GetQualityID()) > 0 )
					if ( (newID = TTemp::FindNewID(dbTmp, t3Quality, oldID)) > 0 )
						rsDest.SetQualityID(newID);
				// Origine ID
				if ( (oldID = rsSrc.GetOrigineID()) > 0 )
					if ( (newID = TTemp::FindNewID(dbTmp, t3Orig, oldID)) > 0 )
						rsDest.SetOrigineID(newID);
				// Screen ID
				if ( (oldID = rsSrc.GetScreenID()) > 0 )
					if ( (newID = TTemp::FindNewID(dbTmp, t3Screen, oldID)) > 0 )
						rsDest.SetScreenID(newID);
				// Audio
				if ( !(s = rsSrc.GetAudioValue()).IsEmpty() )
					rsDest.SetAudioValue(s);
				// Subs
				if ( !(s = rsSrc.GetSubsValue()).IsEmpty() )
					rsDest.SetSubsValue(s);

				rsDest.Update();

				rsTmp.AddNew();
				rsTmp.SetOld(oldBaseID);
				rsTmp.SetNew(rsDest.GetActualCurRow());
				rsTmp.Update();
			} // else, it woudn't make any sence to add an empty Base again

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TBase::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	// BOOL bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	// 18-01-2020 : mismatch, rsDest could be empty if no base was linked in rsSrc
	return !(thr->TestDestroy());
}
