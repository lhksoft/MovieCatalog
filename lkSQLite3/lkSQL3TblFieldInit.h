/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3TBLFIELDINIT_H__
#define __LK_SQL3TBLFIELDINIT_H__
#pragma once

#include "lkSQL3FieldData.h"

#include <wx/object.h>
#include <wx/list.h>

class lkSQL3TblFieldInit : public wxObject
{
public:
	lkSQL3TblFieldInit();
	lkSQL3TblFieldInit(const wxString&, const wxClassInfo*, lkSQL3FieldData**);
	virtual ~lkSQL3TblFieldInit();

	const wxClassInfo*		m_pClass; // what type should it be
	lkSQL3FieldData**		m_ppField;
	wxString				m_fldName;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3TblFieldInit);
};

// ///////////////////////////////////////////////////////////////////////////////

// this macro declares and partly implements MyList class
WX_DECLARE_LIST(lkSQL3TblFieldInit, lkSQL3TblInitBase);

// ///////////////////////////////////////////////////////////////////////////////

class lkSQL3TblInit : public lkSQL3TblInitBase
{
public:
	lkSQL3TblInit();
	lkSQL3TblInit(const wxString&);
	virtual ~lkSQL3TblInit();

	wxString				m_TableName;

	void					AddFldInit(const wxString&, const wxClassInfo*, lkSQL3FieldData**);
	lkSQL3TblFieldInit*		FindByName(const wxString&) const;
};

#endif // !__LK_SQL3TBLFIELDINIT_H__
