/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3VERIFY_H__
#define __LK_SQL3VERIFY_H__
#pragma once

#include "lkSQL3Common.h"

#include <wx/object.h>
#include <wx/list.h>

class lkSQL3Verify : public wxObject
{
public:
	lkSQL3Verify();
	virtual ~lkSQL3Verify();

	// Data
	sqlFieldType		m_nType;
	int					m_nIdx;
	wxString			m_sName;
	bool				m_bCanbeNull;
	bool				m_bUsingDefault;
	wxString			m_sDefault;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3Verify);
};

// ///////////////////////////////////////////////////////////////////////////////

WX_DECLARE_LIST(lkSQL3Verify, lkSQL3VerifyListBase);

// --

class lkSQL3VerifyList : public lkSQL3VerifyListBase
{
public:
	lkSQL3VerifyList();
	virtual ~lkSQL3VerifyList();

	lkSQL3Verify*	GetField(const wxString&);
};

#endif // !__LK_SQL3VERIFY_H__
