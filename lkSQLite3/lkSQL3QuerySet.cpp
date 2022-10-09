/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#include "lkSQL3QuerySet.h"


wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3QuerySet, lkSQL3TableSet)

lkSQL3QuerySet::lkSQL3QuerySet() : lkSQL3TableSet()
{
}
lkSQL3QuerySet::lkSQL3QuerySet(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
}
lkSQL3QuerySet::~lkSQL3QuerySet()
{
}

////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
bool lkSQL3QuerySet::Open(lkInitTable* pTblInit, const wxString& qry)
{
	wxASSERT(!qry.IsEmpty());
	if ( IsOpen() ) return true;
	if ( pTblInit == NULL ) return false;
	if ( (pTblInit->m_pTblDef == NULL) || pTblInit->IsEmpty() ) return false;

	bool bRet = lkSQL3TableSet::Open();

	if ( bRet )
	{
		// using EMPTY name for Table, as Query's exists with pieces of many tables
		lkSQL3TblInit init(wxT(""));

		lkInitTable::iterator Iter;
		for ( int i = 0; pTblInit->m_pTblDef->pFields[i].sName && pTblInit->m_pTblDef->pFields[i].sName[0]; i++ )
			for ( Iter = pTblInit->begin(); Iter != pTblInit->end(); Iter++ )
			{
				if ( (*Iter)->m_szField.CmpNoCase(pTblInit->m_pTblDef->pFields[i].sName) == 0 )
				{
					init.AddFldInit(pTblInit->m_pTblDef->pFields[i].sName, (*Iter)->m_rcField, (*Iter)->m_ppField);
					break;
				}
			}

		if ( bRet = InitColumns(init, *(pTblInit->m_pTblDef), qry) )
			for ( Iter = pTblInit->begin(); Iter != pTblInit->end(); Iter++ )
			{
				if ( bRet )
					bRet = (*((*Iter)->m_ppField) != NULL);
				else
					break;
			}
	}

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool lkSQL3QuerySet::Open(lkInitTable* WXUNUSED(pTblInit)) // nothrow
{
	wxASSERT_MSG(false, wxT("Should use 'lkSQL3QuerySet::Open(lkInitTable* pTblInit, const wxString& qry)' instead."));
	return false;
}

//virtual
void lkSQL3QuerySet::InitColumns(const wxString& qry)
{
	wxASSERT(!qry.IsEmpty());
	m_View = qry;
}

//virtual
bool lkSQL3QuerySet::InitColumns(lkSQL3TblInit& init, const sqlTblDef_t& def, const wxString& tName)
{
	wxASSERT(m_DB && m_DB->IsOpen());
	wxASSERT(!init.IsEmpty());
	lkSQL3TableDef _tDef(def);
	wxASSERT_MSG(m_Col.IsEmpty(), wxT("lkSQL3QuerySet::InitColumns should be called only once")); // do this only once
	m_Query.Empty();

	// setting the query (if any) into its proper field
	lkSQL3QuerySet::InitColumns(tName);

	const lkSQL3FieldDefList* pFldsDef = _tDef.GetFields();
	wxASSERT(!pFldsDef->IsEmpty());
	if ( pFldsDef->IsEmpty() )
		return false; // ERR

	bool bRet = true;

	lkSQL3FieldDefList::const_iterator Iter;
	for ( Iter = pFldsDef->begin(); Iter != pFldsDef->end(); Iter++ )
		m_Col.AddField((*Iter)->GetName(), (*Iter)->GetType());

	m_Query = wxT("*");
	Query();

	lkSQL3FieldData* fld = NULL;
	lkSQL3TblFieldInit* finit = NULL;
	int idx = 0;
	for ( Iter = pFldsDef->begin(); Iter != pFldsDef->end(); Iter++ )
	{
		if ( bRet = (((fld = m_Col.GetField((*Iter)->GetName())) != NULL) &&
					 ((finit = init.FindByName((*Iter)->GetName())) != NULL)) )
		{
			fld->SetDDVmin((*Iter)->GetDDVmin());
			fld->SetDDVmax((*Iter)->GetDDVmax());
			fld->SetIndex(idx++);

			if ( bRet = finit->m_pClass->IsKindOf(fld->GetClassInfo()) ) // fld->IsKindOf(finit->m_pClass)
				*(finit->m_ppField) = fld;
		}
		if ( !bRet )
			break;
	}

	return bRet;
}

//virtual
bool lkSQL3QuerySet::CanAdd()
{
	return false;
}
//virtual
bool lkSQL3QuerySet::CanDelete()
{
	return false;
}
//virtual
bool lkSQL3QuerySet::CanUpdate()
{
	return false;
}
