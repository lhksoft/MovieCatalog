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
#include "TStorage.h"
#include "TMedium.h"
#include "TGroup.h"
#include "TLocation.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3StorageFlds[] = {
	{ t3Storage_NAME,     sqlFieldType::sqlText,  true, false,  0 },
	{ t3Storage_LOCATION,  sqlFieldType::sqlInt,  true,  true, wxT("1") },
	{ t3Storage_MEDIUM,    sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Storage_CREATION, sqlFieldType::sqlReal, false, false,  0 },
	{ t3Storage_INFO,      sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ 0 }
};

// NEW definitions for setting the fields of new table
static sqlTblField_t t3StorageFldsNEW[] = {
	{ t3Storage_NAME,     sqlFieldType::sqlText,  true, false,  0 },
	{ t3Storage_LOCATION,  sqlFieldType::sqlInt,  true,  true, wxT("1") },
	{ t3Storage_MEDIUM,    sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Storage_CREATION, sqlFieldType::sqlReal, false, false,  0 },
	{ t3Storage_INFO,      sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Storage_LINK,      sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ 0 }
};


// Fields of Index-definitions
static sqlFldIndex_t t3StorageIDX_1[] = {
	{ t3Storage_NAME, sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3StorageIdx[] = {
	{ wxT("idxStorage_Storage"), t3StorageIDX_1, true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3StorageTbl = { t3Storage, t3StorageFlds, t3StorageIdx };
static sqlTblDef_t t3StorageTblNEW = { t3Storage, t3StorageFldsNEW, t3StorageIdx };



wxIMPLEMENT_DYNAMIC_CLASS(TStorage, lkSQL3TableSet)

TStorage::TStorage() : lkSQL3TableSet(),
	m_sFltFilterPrev(), m_sFltOrderPrev()
{
	Init();
}
TStorage::TStorage(lkSQL3Database* pDB) : lkSQL3TableSet(pDB),
	m_sFltFilterPrev(), m_sFltOrderPrev()
{
	Init();
}

void TStorage::Init()
{
	m_pFldStorage = NULL;
	m_pFldLocation = NULL;
	m_pFldMedium = NULL;
	m_pFldCreation = NULL;
	m_pFldInfo = NULL;
	m_pFldLinked = NULL;
}

bool TStorage::TestNewTable() // tests whether this is a new table
{
	return lkSQL3TableSet::VerifyTable(m_DB, t3StorageTblNEW);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TStorage::Open()
{
	bool bIsNew = TestNewTable();

	lkInitTable msInit((bIsNew) ? &t3StorageTblNEW : &t3StorageTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Storage_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldStorage));
	msInit.Append(new lkInitField(t3Storage_LOCATION, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldLocation));
	msInit.Append(new lkInitField(t3Storage_MEDIUM, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldMedium));
	msInit.Append(new lkInitField(t3Storage_CREATION, wxCLASSINFO(lkSQL3FieldData_Real), (lkSQL3FieldData**)&m_pFldCreation));
	msInit.Append(new lkInitField(t3Storage_INFO, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldInfo));

	if ( bIsNew )
		msInit.Append(new lkInitField(t3Storage_LINK, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldLinked));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Storage);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TStorage::CanDelete()
{
	bool bCan = lkSQL3TableSet::CanDelete();
	if ( bCan )
		bCan = !(TGroup::HasStorage(m_DB, GetActualCurRow()));
	return bCan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TStorage::GetStorageValue() const
{
	wxASSERT(m_pFldStorage);
	return m_pFldStorage->GetValue2();
}
void TStorage::SetStorageValue(const wxString& _stor)
{
	wxASSERT(m_pFldStorage);
	m_pFldStorage->SetValue2(_stor);
}

wxUint32 TStorage::GetLocationValue() const
{
	wxASSERT(m_pFldLocation);
	return static_cast<wxUint32>(m_pFldLocation->GetValue2());
}
void TStorage::SetLocationValue(wxUint32 _loc)
{
	wxASSERT(m_pFldLocation);
	m_pFldLocation->SetValue2(static_cast<wxInt64>(_loc));
}

wxUint32 TStorage::GetMediumValue() const
{
	wxASSERT(m_pFldMedium);
	return static_cast<wxUint32>(m_pFldMedium->GetValue2());
}
void TStorage::SetMediumValue(wxUint32 _med)
{
	wxASSERT(m_pFldMedium);
	m_pFldMedium->SetValue2(static_cast<wxInt64>(_med));
}

double TStorage::GetCreationValue() const
{
	wxASSERT(m_pFldCreation);
	return m_pFldCreation->GetValue2();
}
void TStorage::SetCreationValue(double _crea)
{
	wxASSERT(m_pFldCreation);
	m_pFldCreation->SetValue2(_crea);
}

wxUint32 TStorage::GetInfoValue() const
{
	wxASSERT(m_pFldInfo);
	return static_cast<wxUint32>(m_pFldInfo->GetValue2());
}
void TStorage::SetInfoValue(wxUint32 _info)
{
	wxASSERT(m_pFldInfo);
	m_pFldInfo->SetValue2(static_cast<wxInt64>(_info));
}

bool TStorage::GetLinkekValue(void) const
{
	bool b = false;
	if ( m_pFldLinked != NULL )
		b = m_pFldLinked->Getbool();
	return b;
}
void TStorage::SetLinkedValue(bool linked)
{
	if ( m_pFldLinked != NULL )
		m_pFldLinked->Setbool(linked);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//virtual
void TStorage::SetFilterActive(bool a)
{
	wxASSERT(!m_sFltFilter.IsEmpty());
	if ( m_sFltFilter.IsEmpty() )
		return;

	bool b = m_bFilterActive;
	m_bFilterActive = a;
	if ( !a && b )
	{
		m_Filter = m_sFltFilterPrev;
		m_Sort = m_sFltOrderPrev;

		m_sFltFilterPrev.Empty();
		m_sFltOrderPrev.Empty();
	}
	else if ( a && !b )
	{
		m_sFltFilterPrev = m_Filter;
		m_sFltOrderPrev = m_Sort;

		m_Filter = m_sFltFilter;
		m_Sort = m_sFltOrder;
	}
}

//virtual
void TStorage::CreateFindFilter(wxString& Filter, const wxString& WXUNUSED(Field), const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection)
{
	Filter = TStorage::CreateStorageFindFilter(Search, findDirection);
}

//static
wxString TStorage::CreateStorageFindFilter(const wxString& Search, FIND_DIRECTION findDirection)
{
	wxString Filter;
	Filter = lkSQL3RecordSet::CreateFindFilter(t3Storage_NAME, Search, findDirection, t3Storage);
	return Filter;
}

//static
bool TStorage::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3StorageTbl);
}

//static
bool TStorage::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Storage) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3StorageTblNEW);

	return bRet;
}

//static
bool TStorage::IsUniqueStorage(lkSQL3Database* _DB, const wxString& orig, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Storage, t3Storage_NAME, orig);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
bool TStorage::HasLocation(lkSQL3Database* pDB, wxUint32 location)
{
	if ( !pDB || !pDB->IsOpen() ) return true;
	wxString sql;
	bool bRet = false;
	sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %u);"), t3Storage, t3Storage_LOCATION, location);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(sql).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = false;
	}

	return bRet;
}

//static
bool TStorage::HasMedium(lkSQL3Database* pDB, wxUint32 medium)
{
	if ( !pDB || !pDB->IsOpen() ) return true;
	wxString sql;
	bool bRet = false;
	sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %u);"), t3Storage, t3Storage_MEDIUM, medium);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(sql).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = false;
	}

	return bRet;
}

//static
wxString TStorage::GetNextLabel(lkSQL3Database* pDB, wxUint32 _medium)
{ // returns the next available Storage-Label available for given Medium -- '' on error
	if ( !pDB || !pDB->IsOpen() ) return wxString();

	wxString	qry, flt, last, lbl;
	TMedium		rsMedium(pDB);
	bool		bEmpty = true;
	wxUint32	nNwID = 0;

	try
	{
		qry.Printf(wxT("([ROWID] = %u)"), _medium);
		rsMedium.SetFilter(qry);
		rsMedium.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return wxString();
	}

	wxString _Likes = rsMedium.GetStorageLikes();
	wxString _notLikes = rsMedium.GetStorageNotLikes();

	if ( !_Likes.IsEmpty() )
	{
		wxString s;
		s.Printf(wxT("([%s] LIKE '%s%%')"), t3Storage_NAME, _Likes);
		if ( !_notLikes.IsEmpty() )
		{
			flt = s;
			s.Printf(wxT("(%s AND ([%s] NOT LIKE '%s%%'))"), s, t3Storage_NAME, _notLikes);
		}
		flt.Printf(wxT(" WHERE %s"), s);
	}
	else // it's numeric only
		flt.Printf(wxT(" WHERE ([%s] GLOB '[0-9]*')"), t3Storage_NAME);

	// Count Rows affected
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s]%s;"), t3Storage, flt);
	try
	{
		bEmpty = (pDB->ExecuteSQLAndGet(qry).GetInt() == 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return wxString();
	}

	// if not empty, get the last used label
	if ( !bEmpty )
	{
		qry.Printf(wxT("SELECT MAX([%s]) FROM [%s]%s;"), t3Storage_NAME, t3Storage, flt);
		try
		{
			last = pDB->ExecuteSQLAndGet(qry).GetText();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return wxString();
		}
		wxString _str;
		if ( !_Likes.IsEmpty() )
			_str = last.Mid(_Likes.Len());
		else
			_str = last;
		unsigned long ul;
		if ( !_str.ToULong(&ul) )
			ul = 0;
		nNwID = static_cast<wxUint32>(ul);
	}

	// must be like "0000"
	lbl.Printf(wxT("%s%04u"), _Likes, ++nNwID);

	// verify if it's realy unique
	if ( !TStorage::IsUniqueStorage(pDB, lbl, NULL) )
		lbl = wxT("");
	return lbl;
}

//static
bool TStorage::TestFilter(lkSQL3Database* pDB, const wxString& filter, const wxString& order)
{
	wxASSERT(!filter.IsEmpty());
	if ( filter.IsEmpty() || !pDB || !pDB->IsOpen() ) return true;
	bool bRet = true;

	wxString qry, s;
	if ( !order.IsEmpty() )
		s.Printf(wxT(" ORDER BY %s"), order);
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE %s%s;"), t3Storage, filter, s);
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
bool TStorage::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	TStorage rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	//	rsSrc.SetOrder(wxT("[ROWID]"));
	{
		// rsSrc.SetOrder(_T("[MediumID], [Storage]"));
		wxString order;
		order.Printf(wxT("[%s], [%s]"), t3Storage_MEDIUM, t3Storage_NAME);
		rsSrc.SetOrder(order);
	}
	wxUint32 old32, new32;
	double dbl;
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Storage);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Storage);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Storage
			rsDest.SetStorageValue(rsSrc.GetStorageValue());
			// Location
			if ( (old32 = rsSrc.GetLocationValue()) > 0 )
				if ( (new32 = (wxUint32)(TTemp::FindNewID(dbTmp, t3Location, old32))) > 0 )
					rsDest.SetLocationValue(new32);
			// Medium
			if ( (old32 = rsSrc.GetMediumValue()) > 0 )
				if ( (new32 = (wxUint32)(TTemp::FindNewID(dbTmp, t3Medium, old32))) > 0 )
					rsDest.SetMediumValue(new32);
			// Creation
			if ( (dbl = rsSrc.GetCreationValue()) != 0.0 )
				rsDest.SetCreationValue(dbl);
			// Info
			if ( (new32 = rsSrc.GetInfoValue()) != 0 )
				rsDest.SetInfoValue(new32);
			// add since 29/08/22
			 // LinkedValue will be set in TGroup::Compact

			rsDest.Update();

			rsTmp.AddNew();
			rsTmp.SetOld(rsSrc.GetActualCurRow());
			rsTmp.SetNew(rsDest.GetActualCurRow());
			rsTmp.Update();

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TStorage::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}

//static
bool TStorage::SetLinked(lkSQL3Database* dbDest, wxUint64 newStorageID) // called from inside TGroup::Compact; sets Linked-field to true if newStorageID is found
{
	if ( !dbDest || !dbDest->IsOpen() || (newStorageID == 0) )
		return false;

	TStorage rs(dbDest);
	wxString whr;
	whr.Printf(wxT("([ROWID] = %I64u)"), newStorageID);
	rs.SetFilter(whr);

	try
	{
		rs.Open();
		wxASSERT(rs.GetRowCount() == 1);

		rs.SetLinkedValue(true);
		rs.Update();
	}
	catch ( const lkSQL3Exception& )
	{
		throw;
		return false;
	}

	return true;
}
