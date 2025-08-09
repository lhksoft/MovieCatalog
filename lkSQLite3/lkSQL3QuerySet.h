/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3QUERYSET_H__
#define __LK_SQL3QUERYSET_H__
#pragma once

#include "lkSQL3TableSet.h"

class lkSQL3QuerySet : public lkSQL3TableSet
{
public:
	lkSQL3QuerySet();
	lkSQL3QuerySet(lkSQL3Database* pDB);
	virtual ~lkSQL3QuerySet();

// Operations
protected:
	virtual bool						Open(lkInitTable*, const wxString& qry); // nothrow
	virtual bool						Open(lkInitTable*) wxOVERRIDE; // nothrow

	virtual void						InitColumns(const wxString&) wxOVERRIDE; // param = view : actual custom query to use
	virtual bool						InitColumns(lkSQL3TblInit&, const sqlTblDef_t&, const wxString& tName = wxEmptyString) wxOVERRIDE; // tName should hold entire custom query to use

public:
	virtual bool						CanAdd(void) wxOVERRIDE;
	virtual bool						CanDelete(void) wxOVERRIDE;
	virtual bool						CanUpdate(void) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3QuerySet);
};
#endif // !__LK_SQL3QUERYSET_H__
