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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "internal_Variant.h"
#include <wx/defs.h>
#include <math.h>

#ifndef DATE_MAX

#define DATE_MAX        2958465
#define DATE_MIN        -657434

#endif // !DATE_MAX

#ifndef VAR_TIMEVALUEONLY

#define VAR_TIMEVALUEONLY   0x001 /* Ignore date portion of VT_DATE */
#define VAR_DATEVALUEONLY   0x002 /* Ignore time portion of VT_DATE */

#endif // !VAR_TIMEVALUEONLY


/* Date Conversions */
#ifndef IsLeapYear
#define IsLeapYear(y)   (((y % 4) == 0) && (((y % 100) != 0) || ((y % 400) == 0)))
#endif // !IsLeapYear

/* Convert a VT_DATE value to a Julian Date */
static inline int VARIANT_JulianFromDate(int dateIn)
{
    int julianDays = dateIn;

    julianDays -= DATE_MIN; /* Convert to + days from 1 Jan 100 AD */
    julianDays += 1757585;  /* Convert to + days from 23 Nov 4713 BC (Julian) */
    return julianDays;
}

/* Convert a Julian Date to a VT_DATE value */
static inline int VARIANT_DateFromJulian(int dateIn)
{
    int julianDays = dateIn;

    julianDays -= 1757585;  /* Convert to + days from 1 Jan 100 AD */
    julianDays += DATE_MIN; /* Convert to +/- days from 1 Jan 1899 AD */
    return julianDays;
}

/* Convert Day/Month/Year to a Julian date - from PostgreSQL */
static inline double VARIANT_JulianFromDMY(wxUint16 year, wxUint16 month, wxUint16 day)
{
    int m12 = (month - 14) / 12;

    return ((1461 * (year + 4800 + m12)) / 4 + (367 * (month - 2 - 12 * m12)) / 12 - (3 * ((year + 4900 + m12) / 100)) / 4 + day - 32075);
}

/* Convert a Julian date to Day/Month/Year - from PostgreSQL */
static inline void VARIANT_DMYFromJulian(int jd, int* year, wxUint16* month, wxUint16* day)
{
    int j, i, l, n;

    l = jd + 68569;
    n = l * 4 / 146097;
    l -= (n * 146097 + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l += 31 - (i * 1461) / 4;
    j = (l * 80) / 2447;
    *day = l - (j * 2447) / 80;
    l = j / 11;
    *month = (j + 2) - (12 * l);
    *year = 100 * (n - 49) + i + l;
}

/* Roll a date forwards or backwards to correct it */
static bool VARIANT_RollUdate(lkUDATE* lpUd)
{
    static const wxUint8 days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    wxInt16 iYear, iMonth, iDay, iHour, iMinute, iSecond;

    /* interpret values signed */
    iYear = lpUd->st.wYear;
    iMonth = lpUd->st.wMonth;
    iDay = lpUd->st.wDay;
    iHour = lpUd->st.wHour;
    iMinute = lpUd->st.wMinute;
    iSecond = lpUd->st.wSecond;

    if ( (iYear > 9999) || (iYear < -9999) )
        return false; /* Invalid value */

    /* Years 0 to 49 are treated as 2000 + year, see also VARIANT_MakeDate() */
    if ( (0 <= iYear) && (iYear <= 49) )
        iYear += 2000;
    /* Remaining years 50 to 99 are treated as 1900 + year */
    else if ( (50 <= iYear) && (iYear <= 99) )
        iYear += 1900;

    iMinute += iSecond / 60;
    iSecond = iSecond % 60;
    iHour += iMinute / 60;
    iMinute = iMinute % 60;
    iDay += iHour / 24;
    iHour = iHour % 24;
    iYear += iMonth / 12;
    iMonth = iMonth % 12;
    if ( iMonth <= 0 )
    {
        iMonth += 12;
        iYear--;
    }

    while ( iDay > days[iMonth] )
    {
        if ( (iMonth == 2) && IsLeapYear(iYear) )
            iDay -= 29;
        else
            iDay -= days[iMonth];
        iMonth++;
        iYear += iMonth / 12;
        iMonth = iMonth % 12;
    }

    while ( iDay <= 0 )
    {
        iMonth--;
        if ( iMonth <= 0 )
        {
            iMonth += 12;
            iYear--;
        }
        if ( (iMonth == 2) && IsLeapYear(iYear) )
            iDay += 29;
        else
            iDay += days[iMonth];
    }

    if ( iSecond < 0 )
    {
        iSecond += 60;
        iMinute--;
    }
    if ( iMinute < 0 )
    {
        iMinute += 60;
        iHour--;
    }
    if ( iHour < 0 )
    {
        iHour += 24;
        iDay--;
    }
    if ( iYear <= 0 )
        iYear += 2000;

    lpUd->st.wYear = iYear;
    lpUd->st.wMonth = iMonth;
    lpUd->st.wDay = iDay;
    lpUd->st.wHour = iHour;
    lpUd->st.wMinute = iMinute;
    lpUd->st.wSecond = iSecond;

    return true;
}

/***********************************************************************
 *              VarUdateFromDate [OLEAUT32.331]
 *
 * Convert a variant VT_DATE into an unpacked format date and time.
 *
 * PARAMS
 *  datein    [I] Variant VT_DATE format date
 *  dwFlags   [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  lpUdate   [O] Destination for unpacked format date and time
 *
 * RETURNS
 *  Success: S_OK. *lpUdate contains the converted value.
 *  Failure: E_INVALIDARG, if dateIn is too large or small.
 */
bool VarUdateFromDate(double dateIn, wxUint32 dwFlags, lkUDATE* lpUdate)
{
    /* Cumulative totals of days per month */
    static const wxUint16 cumulativeDays[] = {
        0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    double datePart, timePart;
    int julianDays;

    if ( dateIn <= (DATE_MIN - 1.0) || dateIn >= (DATE_MAX + 1.0) )
        return false;

    datePart = (dateIn < 0.0) ? ceil(dateIn) : floor(dateIn);

    /* Compensate for int truncation (always downwards) */
    timePart = fabs(dateIn - datePart) + 0.00000000001;
    if ( timePart >= 1.0 )
        timePart -= 0.00000000001;

    /* Date */
    julianDays = VARIANT_JulianFromDate(dateIn);
    VARIANT_DMYFromJulian(julianDays, &lpUdate->st.wYear, &lpUdate->st.wMonth, &lpUdate->st.wDay);

    datePart = (datePart + 1.5) / 7.0;
    lpUdate->st.wDayOfWeek = (datePart - floor(datePart)) * 7;
    if ( lpUdate->st.wDayOfWeek == 0 )
        lpUdate->st.wDayOfWeek = 5;
    else if ( lpUdate->st.wDayOfWeek == 1 )
        lpUdate->st.wDayOfWeek = 6;
    else
        lpUdate->st.wDayOfWeek -= 2;

    if ( (lpUdate->st.wMonth > 2) && (IsLeapYear(lpUdate->st.wYear)) )
        lpUdate->wDayOfYear = 1; /* After February, in a leap year */
    else
        lpUdate->wDayOfYear = 0;

    lpUdate->wDayOfYear += cumulativeDays[lpUdate->st.wMonth];
    lpUdate->wDayOfYear += lpUdate->st.wDay;

    /* Time */
    timePart *= 24.0;
    lpUdate->st.wHour = timePart;
    timePart -= lpUdate->st.wHour;
    timePart *= 60.0;
    lpUdate->st.wMinute = timePart;
    timePart -= lpUdate->st.wMinute;
    timePart *= 60.0;
    lpUdate->st.wSecond = timePart;
    timePart -= lpUdate->st.wSecond;
    lpUdate->st.wMilliseconds = 0;
    if ( timePart > 0.5 )
    {
        /* Round the milliseconds, adjusting the time/date forward if needed */
        if ( lpUdate->st.wSecond < 59 )
            lpUdate->st.wSecond++;
        else
        {
            lpUdate->st.wSecond = 0;
            if ( lpUdate->st.wMinute < 59 )
                lpUdate->st.wMinute++;
            else
            {
                lpUdate->st.wMinute = 0;
                if ( lpUdate->st.wHour < 23 )
                    lpUdate->st.wHour++;
                else
                {
                    lpUdate->st.wHour = 0;
                    /* Roll over a whole day */
                    if ( ++lpUdate->st.wDay > 28 )
                        VARIANT_RollUdate(lpUdate);
                }
            }
        }
    }

    return true;
}



/***********************************************************************
 *              VarDateFromUdateEx [OLEAUT32.319]
 *
 * Convert an unpacked format date and time to a variant VT_DATE.
 *
 * PARAMS
 *  pUdateIn [I] Unpacked format date and time to convert
 *  lcid     [I] Locale identifier for the conversion
 *  dwFlags  [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  pDateOut [O] Destination for variant VT_DATE.
 *
 * RETURNS
 *  Success: S_OK. *pDateOut contains the converted value.
 *  Failure: E_INVALIDARG, if pUdateIn cannot be represented in VT_DATE format.
 */
bool VarDateFromUdateEx(lkUDATE* pUdateIn, wxUint32 dwFlags, double* pDateOut)
{
    double dateVal = 0;
    lkUDATE ud = *pUdateIn;

    if ( !VARIANT_RollUdate(&ud) )
        return false;

    /* Date */
    if ( !(dwFlags & VAR_TIMEVALUEONLY) )
		dateVal = VARIANT_DateFromJulian(VARIANT_JulianFromDMY(ud.st.wYear, ud.st.wMonth, ud.st.wDay));

    if ( (dwFlags & VAR_TIMEVALUEONLY) || !(dwFlags & VAR_DATEVALUEONLY) )
    {
        double dateSign = (dateVal < 0.0) ? -1.0 : 1.0;

        /* Time */
        dateVal += ud.st.wHour / 24.0 * dateSign;
        dateVal += ud.st.wMinute / 1440.0 * dateSign;
        dateVal += ud.st.wSecond / 86400.0 * dateSign;
    }

    *pDateOut = dateVal;
    return true;
}


/***********************************************************************
 *              VarDateFromUdate [OLEAUT32.330]
 *
 * Convert an unpacked format date and time to a variant VT_DATE.
 *
 * PARAMS
 *  pUdateIn [I] Unpacked format date and time to convert
 *  dwFlags  [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  pDateOut [O] Destination for variant VT_DATE.
 *
 * RETURNS
 *  Success: S_OK. *pDateOut contains the converted value.
 *  Failure: E_INVALIDARG, if pUdateIn cannot be represented in VT_DATE format.
 *
 * NOTES
 *  This function uses the United States English locale for the conversion. Use
 *  VarDateFromUdateEx() for alternate locales.
 */
bool VarDateFromUdate(lkUDATE* pUdateIn, wxUint32 dwFlags, double* pDateOut)
{
    return VarDateFromUdateEx(pUdateIn, dwFlags, pDateOut);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////


bool VariantTimeToSystemTime(double dateIn, lkSYSTEMTIME* lpSt)
{
    lkUDATE ud;

    if ( !VarUdateFromDate(dateIn, 0, &ud) )
        return FALSE;

    *lpSt = ud.st;
    return true;
}


bool SystemTimeToVariantTime(const lkSYSTEMTIME& lpSt, double* pDateOut)
{
    lkUDATE ud;

    if ( (lpSt.wMonth > 12) || (lpSt.wDay > 31) || (lpSt.wYear < 0) )
        return false;

    ud.st = lpSt;
    return VarDateFromUdate(&ud, 0, pDateOut);
}

