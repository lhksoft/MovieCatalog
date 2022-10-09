/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_DATETIME_H__
#define __LK_DATETIME_H__
#pragma once

#include <wx/datetime.h>

#define lkInvalidDateTime	(static_cast<double>(wxINT64_MIN))

class lkDateTime : public wxDateTime
{
public:
	lkDateTime();
	lkDateTime(const lkDateTime&);
	lkDateTime(double);

	lkDateTime&			operator=(double);
	operator			double() const;

	void				SetInvalid(void); // resets internal date/time to Invalid

	wxString			FormatFullDate_Dutch(bool bLong = true) const;
	wxString			FormatFullDate_English(bool bLong = true) const;
	wxString			FormatYearOnly(void) const;
};

///////////////////////////////////////////////////////////////////////////////////////
// class lkDateTimeSpan
// --------------------
// Mostly based at MFC's-ATL COleDateTimeSpan
////
class lkDateTimeSpan
{
private:
	static const long				maxDaysInSpan = 3615897L;

// Constructors
public:
	lkDateTimeSpan();
	lkDateTimeSpan(const lkDateTimeSpan&);
	lkDateTimeSpan(double dblSpanSrc);
	lkDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);
	virtual ~lkDateTimeSpan();

// Attributes
public:
	enum class DateTimeSpanStatus
	{
		valid = 0,
		invalid = 1,    // Invalid span (out of range, etc.)
		null = 2,       // Literally has no value
	};

	static const double				DATETIME_HALFSECOND;

protected:
	double							m_span;							// this is what will be written in the DB
	DateTimeSpanStatus				m_status;

public:
	bool							IsValid(void) const;
	bool							IsNull(void) const;

	void							SetValid(void);
	void							SetInvalid(void);
	void							SetNull(void);

	void							SetStatus(DateTimeSpanStatus status);
	DateTimeSpanStatus				GetStatus(void) const;

	double							GetTotalDays(void) const;		// span in days (about -3.65e6 to 3.65e6)
	double							GetTotalHours(void) const;		// span in hours (about -8.77e7 to 8.77e6)
	double							GetTotalMinutes(void) const;	// span in minutes (about -5.26e9 to 5.26e9)
	double							GetTotalSeconds(void) const;	// span in seconds (about -3.16e11 to 3.16e11)

	long							GetDays(void) const;			// component days in span
	long							GetHours(void) const;			// component hours in span (-23 to 23)
	long							GetMinutes(void) const;			// component minutes in span (-59 to 59)
	long							GetSeconds(void) const;			// component seconds in span (-59 to 59)

// Operations
public:
	lkDateTimeSpan&					operator=(double dblSpanSrc);
	lkDateTimeSpan&					operator=(const lkDateTimeSpan& SpanSrc);

	bool							operator==(const lkDateTimeSpan& dateSpan) const;
	bool							operator!=(const lkDateTimeSpan& dateSpan) const;
	bool							operator<(const lkDateTimeSpan& dateSpan) const;
	bool							operator>(const lkDateTimeSpan& dateSpan) const;
	bool							operator<=(const lkDateTimeSpan& dateSpan) const;
	bool							operator>=(const lkDateTimeSpan& dateSpan) const;

	// DateTimeSpan math
	lkDateTimeSpan					operator+(const lkDateTimeSpan& dateSpan) const;
	lkDateTimeSpan					operator-(const lkDateTimeSpan& dateSpan) const;
	lkDateTimeSpan&					operator+=(const lkDateTimeSpan dateSpan);
	lkDateTimeSpan&					operator-=(const lkDateTimeSpan dateSpan);
	lkDateTimeSpan					operator-() const;

	operator double() const;

	void							SetDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);

// Implementation
public:
	void							CheckRange(void);

	// conversion to text
	// ------------------------------------------------------------------------
		// this function accepts strftime()-like format string (default argument corresponds to the preferred date and time representation
		// for the current locale) and returns the string containing the resulting text representation. Notice that only some of format
		// specifiers valid for wxDateTime are valid for wxTimeSpan: hours, minutes and seconds make sense, but not "PM/AM" string for example.
	wxString						Format(const wxString& format = wxASCII_STR(wxDefaultTimeSpanFormat)) const;
};
#endif // !__LK_DATETIME_H__
