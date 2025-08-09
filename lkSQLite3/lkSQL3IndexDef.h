/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3INDEXDEF_H__
#define __LK_SQL3INDEXDEF_H__
#pragma once

#include "lkSQL3Common.h"

#include <wx/object.h>
#include <wx/list.h>

typedef struct sqlFldIndex_s
{
	const wxChar*	fName;		// FieldName
	sqlSortOrder	nOrder;
} sqlFldIndex_t, * sqlFldIndex_p;

typedef struct sqlTblIndex_s
{
	const wxChar*	sName;		// Name for this index
	sqlFldIndex_p	pIdxFields;	// Array of Fieldnames to include in thise index
	bool			bUnique;
} sqlTblIndex_t, * sqlTblIndex_p;

//////////////////////////////////////////////////////

class lkIdxFieldDef : public wxObject
{
public:
	lkIdxFieldDef();
	lkIdxFieldDef(const wxString&, sqlSortOrder);
	lkIdxFieldDef(const lkIdxFieldDef&);
	lkIdxFieldDef(const sqlFldIndex_t&);
	virtual ~lkIdxFieldDef() { }

	// Members
protected:
	wxString			m_fName;
	sqlSortOrder		m_Order;

public:
	wxString			GetDefinition(void) const;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkIdxFieldDef);
};

// this macro declares and partly implements MyList class
WX_DECLARE_LIST(lkIdxFieldDef, lkIdxFieldDefList);

//////////////////////////////////////////////////////

class lkSQL3IndexDef : public wxObject
{
public:
	lkSQL3IndexDef();
	lkSQL3IndexDef(const lkSQL3IndexDef&);
	lkSQL3IndexDef(const sqlTblIndex_t&);
	virtual ~lkSQL3IndexDef();

	// Members
protected:
	lkIdxFieldDefList	m_Indexes; // containing 1..more lkIdxFieldDef
	wxString			m_Name; // name of this Index
	bool				m_bUnique; // Unique Index or not

public:
	wxString			GetDefinition(const wxString& _table) const;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3IndexDef);
};

// ///////////////////////////////////////////////////////////////////////////////

// This list used in lkSQL3TableDef
WX_DECLARE_LIST(lkSQL3IndexDef, lkSQL3IndexDefList);

#endif // !__LK_SQL3INDEXDEF_H__
