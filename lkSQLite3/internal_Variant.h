/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************
 * Based at code found in Wine : VARIANT
 * at <https://source.winehq.org/source/dlls/oleaut32/variant.c>
 *
 * Copyright 1998 Jean-Claude Cote
 * Copyright 2003 Jon Griffiths
 * Copyright 2005 Daniel Remenak
 * Copyright 2006 Google (Benjamin Arai)
 *
 * The algorithm for conversion from Julian days to day/month/year is based on
 * that devised by Henry Fliegel, as implemented in PostgreSQL, which is
 * Copyright 1994-7 Regents of the University of California
 ***********************************************************/

/***********************************************************
* I only need the posibilty to convert a Variant DATE (which is a double)
* to a SystemTime structure (which is used in wxDateTime in the MSW port).
* 
* For storage into SQL databases, the VT_DATE datatype of Microsoft
* still is the best way for doing so. 
* But I've not found any possibility in GTK nor in wxWidgets
* to deal with a VT_DATE type.
* 
* Sometimes one need to reinvend the wheel to get things done.
 ***********************************************************/
#ifndef __INTERNAL_VARIANT_H__
#define __INTERNAL_VARIANT_H__
#pragma once

#include "lkDateTime.h"

 /* Same as SYSTEMTIME from Microsoft, but we need it here too and it's not available anywhere else but at MSW */
typedef struct lkSYSTEMTIME
{
	int		        wYear;
	wxUint16        wMonth;
	wxUint16        wDayOfWeek;
	wxUint16        wDay;
	wxUint16        wHour;
	wxUint16        wMinute;
	wxUint16        wSecond;
	wxUint16        wMilliseconds;
} lkSYSTEMTIME;

typedef struct lkUDate
{
	lkSYSTEMTIME	st;
	wxUint16		wDayOfYear;
} lkUDATE;


 /***********************************************************************
 *              VariantTimeToSystemTime [OLEAUT32.185]
 *
 * Convert a variant VT_DATE into a System format date and time.
 *
 * PARAMS
 *  datein [I] Variant VT_DATE format date
 *  lpSt   [O] Destination for System format date and time
 *
 * RETURNS
 *  Success: TRUE. *lpSt contains the converted value.
 *  Failure: FALSE, if dateIn is too large or small.
 */
bool VariantTimeToSystemTime(double dateIn, lkSYSTEMTIME* lpSt);


/***********************************************************************
 *              SystemTimeToVariantTime [OLEAUT32.184]
 *
 * Convert a System format date and time into variant VT_DATE format.
 *
 * PARAMS
 *  lpSt     [I] System format date and time
 *  pDateOut [O] Destination for VT_DATE format date
 *
 * RETURNS
 *  Success: TRUE. *pDateOut contains the converted value.
 *  Failure: FALSE, if lpSt cannot be represented in VT_DATE format.
 */
bool SystemTimeToVariantTime(const lkSYSTEMTIME& lpSt, double* pDateOut);


#endif // !__INTERNAL_VARIANT_H__
