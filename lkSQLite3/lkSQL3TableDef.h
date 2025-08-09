/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3TABLEDEF_H__
#define __LK_SQL3TABLEDEF_H__
#pragma once

#include "lkSQL3FieldDef.h"
#include "lkSQL3IndexDef.h"
#include "lkSQL3Database.h"

#include <wx/object.h>

typedef struct sqlTblDef_s
{
	const wxChar*	sName;		// Name for this table
	sqlTblField_p	pFields;	// Field Infos
	sqlTblIndex_p	pIndex;		// optional Indexes (might be NULL)
} sqlTblDef_t, * sqlTblDef_p;


class lkSQL3TableDef : public wxObject
{
public:
	lkSQL3TableDef();
	lkSQL3TableDef(const sqlTblDef_t&);
	lkSQL3TableDef(const lkSQL3TableDef&);
	virtual ~lkSQL3TableDef();

protected:
	lkSQL3FieldDefList			m_Fields; // containing lkSql3FieldDef's
	lkSQL3IndexDefList			m_Indexes; // containing lkSql3IndexDef's

	wxString					m_Name; // name of this table

	wxString					GetTableDefinition(bool _Temp) const; // Definition for only the table (when creating)

public:
	wxString					GetName(void) const;
	void						SetName(const wxString&);

	const lkSQL3FieldDefList*	GetFields(void) const;
	bool						CreateTable(lkSQL3Database&, bool _Temp = false); // Creates the table and Indexes -- false on error or empty table definition

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3TableDef);
};

#endif // !__LK_SQL3TABLEDEF_H__
