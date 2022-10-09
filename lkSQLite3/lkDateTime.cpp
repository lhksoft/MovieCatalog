/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#include "lkDateTime.h"
#include "internal_Variant.h"
#include <wx/debug.h>
#include <math.h>

lkDateTime::lkDateTime() : wxDateTime()
{
}
lkDateTime::lkDateTime(const lkDateTime& other) : wxDateTime(other)
{
}
lkDateTime::lkDateTime(double dt) : wxDateTime()
{
	*this = dt;
}

lkDateTime& lkDateTime::operator=(double dt)
{
	*(dynamic_cast<wxDateTime*>(this)) = wxInvalidDateTime;

	if (dt != lkInvalidDateTime)
	{
		lkSYSTEMTIME sysTime = { 0 };
		if ( VariantTimeToSystemTime(dt, &sysTime) )
			Set((wxDateTime::wxDateTime_t)sysTime.wDay, static_cast<wxDateTime::Month>(wxDateTime::Jan + sysTime.wMonth - 1), sysTime.wYear,
				(wxDateTime::wxDateTime_t)sysTime.wHour, (wxDateTime::wxDateTime_t)sysTime.wMinute, (wxDateTime::wxDateTime_t)sysTime.wSecond, (wxDateTime::wxDateTime_t)sysTime.wMilliseconds);
	}

	return *this;
}

lkDateTime::operator double() const
{
	double dt = lkInvalidDateTime;

	if ( IsValid() )
	{
		lkSYSTEMTIME st = { 0 };

		st.wYear = GetYear();
		st.wMonth = (int)(GetMonth() - wxDateTime::Jan + 1);
		st.wDay = GetDay();
		st.wHour = GetHour();
		st.wMinute = GetMinute();
		st.wSecond = GetSecond();
		st.wMilliseconds = GetMillisecond();

		if ( !SystemTimeToVariantTime(st, &dt) )
			dt = lkInvalidDateTime; // static_cast<double>(wxINT64_MIN)
	}
	return dt;
}

void lkDateTime::SetInvalid()
{
	*(dynamic_cast<wxDateTime*>(this)) = wxInvalidDateTime;
}

wxString lkDateTime::FormatFullDate_Dutch(bool bLong) const
{
	if ( !IsValid() )
		return wxString();
	// else..

	if ( bLong )
	{
		static wxString d_months[12] = { wxT("januari"), wxT("februari"), wxT("maart"), wxT("april"), wxT("mei"), wxT("juni"),
			wxT("juli"), wxT("augustus"), wxT("september"), wxT("oktober"), wxT("november"), wxT("december") };
		static wxString d_days[7] = { wxT("zondag"), wxT("maandag"), wxT("dinsdag"), wxT("woensdag"), wxT("donderdag"), wxT("vrijdag"), wxT("zaterdag") };

		return wxString::Format(wxT("%s %d %s %u"), d_days[GetWeekDay()], GetDay(), d_months[GetMonth()], GetYear());
	}
	//else..
	static wxString d_mnths[12] = { wxT("jan"), wxT("feb"), wxT("mrt"), wxT("apr"), wxT("mei"), wxT("jun"),
		wxT("jul"), wxT("aug"), wxT("sep"), wxT("okt"), wxT("nov"), wxT("dec") };
	return wxString::Format(wxT("%02d-%s-%u"), GetDay(), d_mnths[GetMonth()], GetYear());
}
wxString lkDateTime::FormatFullDate_English(bool bLong) const
{
	if ( !IsValid() )
		return wxString();
	// else..

	if ( bLong )
	{
		static wxString e_months[12] = { wxT("January"), wxT("February"), wxT("Mars"), wxT("April"), wxT("May"), wxT("June"),
			wxT("July"), wxT("August"), wxT("September"), wxT("October"), wxT("November"), wxT("December") };
		static wxString e_days[7] = { wxT("Sunday"), wxT("Monday"), wxT("Tuesday"), wxT("Wednesday"), wxT("Thursday"), wxT("Friday"), wxT("Saturday") };

		return wxString::Format(wxT("%s, %s %02d %u"), e_days[GetWeekDay()], e_months[GetMonth()], GetDay(), GetYear());
	}
	//else..
	static wxString e_mnths[12] = { wxT("Jan"), wxT("Feb"), wxT("Mar"), wxT("Apr"), wxT("May"), wxT("Jun"),
		wxT("Jul"), wxT("Aug"), wxT("Sep"), wxT("Oct"), wxT("Nov"), wxT("Dec") };
	return wxString::Format(wxT("%02d-%s-%u"), GetDay(), e_mnths[GetMonth()], GetYear());
}

wxString lkDateTime::FormatYearOnly() const
{
	if ( !IsValid() )
		return wxString();
	// else..

	return wxString::Format(wxT("%u"), GetYear());
}


///////////////////////////////////////////////////////////////////////////////////////
// class lkDateTimeSpan
////

const double lkDateTimeSpan::DATETIME_HALFSECOND = 1.0 / (2.0 * (60.0 * 60.0 * 24.0));

lkDateTimeSpan::lkDateTimeSpan() : m_span(0), m_status(DateTimeSpanStatus::valid)
{
}

lkDateTimeSpan::lkDateTimeSpan(double dblSpanSrc) : m_span(dblSpanSrc), m_status(DateTimeSpanStatus::valid)
{
	CheckRange();
}

lkDateTimeSpan::lkDateTimeSpan(const lkDateTimeSpan& inDT) : m_span(inDT.m_span), m_status(inDT.m_status)
{
	CheckRange();
}

lkDateTimeSpan::lkDateTimeSpan(long lDays, int nHours, int nMins, int nSecs)
{
	SetDateTimeSpan(lDays, nHours, nMins, nSecs);
}

lkDateTimeSpan::~lkDateTimeSpan()
{
}

////////////////////////////////////////////////
// Attributes

bool lkDateTimeSpan::IsValid() const
{
	return (GetStatus() == DateTimeSpanStatus::valid);
}
bool lkDateTimeSpan::IsNull() const
{
	return (GetStatus() == DateTimeSpanStatus::null);
}

void lkDateTimeSpan::SetValid()
{
	SetStatus(DateTimeSpanStatus::valid);
}
void lkDateTimeSpan::SetInvalid()
{
	SetStatus(DateTimeSpanStatus::invalid);
}
void lkDateTimeSpan::SetNull()
{
	SetStatus(DateTimeSpanStatus::null);
}

void lkDateTimeSpan::SetStatus(DateTimeSpanStatus status)
{
	m_status = status;
}

lkDateTimeSpan::DateTimeSpanStatus lkDateTimeSpan::GetStatus() const
{
	return m_status;
}

double lkDateTimeSpan::GetTotalDays() const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	return (double)wxInt64(m_span + (m_span < 0.0 ? -DATETIME_HALFSECOND : DATETIME_HALFSECOND));
}

double lkDateTimeSpan::GetTotalHours() const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	return (double)wxInt64((m_span + (m_span < 0.0 ? -DATETIME_HALFSECOND : DATETIME_HALFSECOND)) * 24);
}

double lkDateTimeSpan::GetTotalMinutes() const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	return (double)wxInt64((m_span + (m_span < 0.0 ? -DATETIME_HALFSECOND : DATETIME_HALFSECOND)) * (24 * 60));
}

double lkDateTimeSpan::GetTotalSeconds() const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	return (double)wxInt64((m_span + (m_span < 0.0 ? -DATETIME_HALFSECOND : DATETIME_HALFSECOND)) * (24 * 60 * 60));
}

long lkDateTimeSpan::GetDays() const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	return long(GetTotalDays());
}

long lkDateTimeSpan::GetHours() const
{
	double dPartialDayHours = GetTotalHours() - (GetTotalDays() * 24.0);
	return long(dPartialDayHours) % 24;
}

long lkDateTimeSpan::GetMinutes() const
{
	double dPartialHourMinutes = GetTotalMinutes() - (GetTotalHours() * 60.0);
	return long(dPartialHourMinutes) % 60;
}

long lkDateTimeSpan::GetSeconds() const
{
	double dPartialMinuteSeconds = GetTotalSeconds() - (GetTotalMinutes() * 60.0);
	return long(dPartialMinuteSeconds) % 60;
}

////////////////////////////////////////////////
// Operations

lkDateTimeSpan& lkDateTimeSpan::operator=(double dblSpanSrc)
{
	m_span = dblSpanSrc;
	m_status = DateTimeSpanStatus::valid;
	CheckRange();
	return *this;
}

lkDateTimeSpan& lkDateTimeSpan::operator=(const lkDateTimeSpan& SpanSrc)
{
	m_span = SpanSrc.m_span;
	m_status = SpanSrc.m_status;
	CheckRange();
	return *this;
}

bool lkDateTimeSpan::operator==(const lkDateTimeSpan& dateSpan) const
{
	if ( GetStatus() == dateSpan.GetStatus() )
	{
		if ( GetStatus() == DateTimeSpanStatus::valid )
		{
			// it has to be in precision range to say that it as equal
			return ((m_span + DATETIME_HALFSECOND > dateSpan.m_span) && (m_span - DATETIME_HALFSECOND < dateSpan.m_span));
		}

		return (GetStatus() == DateTimeSpanStatus::null);
	}

	return false;
}

bool lkDateTimeSpan::operator!=(const lkDateTimeSpan& dateSpan) const
{
	return !operator==(dateSpan);
}

bool lkDateTimeSpan::operator<(const lkDateTimeSpan& dateSpan) const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	wxASSERT(dateSpan.GetStatus() == DateTimeSpanStatus::valid);
	if ( (GetStatus() == DateTimeSpanStatus::valid) && (GetStatus() == dateSpan.GetStatus()) )
		return (m_span < dateSpan.m_span);

	return false;
}

bool lkDateTimeSpan::operator>(const lkDateTimeSpan& dateSpan) const
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	wxASSERT(dateSpan.GetStatus() == DateTimeSpanStatus::valid);
	if ( (GetStatus() == DateTimeSpanStatus::valid) && (GetStatus() == dateSpan.GetStatus()) )
		return (m_span > dateSpan.m_span);

	return false;
}

bool lkDateTimeSpan::operator<=(const lkDateTimeSpan& dateSpan) const
{
	return (operator<(dateSpan) || operator==(dateSpan));
}

bool lkDateTimeSpan::operator>=(const lkDateTimeSpan& dateSpan) const
{
	return (operator>(dateSpan) || operator==(dateSpan));
}

lkDateTimeSpan lkDateTimeSpan::operator+(const lkDateTimeSpan& dateSpan) const
{
	lkDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if ( (GetStatus() == DateTimeSpanStatus::null) || (dateSpan.GetStatus() == DateTimeSpanStatus::null ))
	{
		dateSpanTemp.SetStatus(DateTimeSpanStatus::null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (( GetStatus() == DateTimeSpanStatus::invalid) || (dateSpan.GetStatus() == DateTimeSpanStatus::invalid ))
	{
		dateSpanTemp.SetStatus(DateTimeSpanStatus::invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.m_span = m_span + dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

lkDateTimeSpan lkDateTimeSpan::operator-(const lkDateTimeSpan& dateSpan) const
{
	lkDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if ( (GetStatus() == DateTimeSpanStatus::null) || (dateSpan.GetStatus() == DateTimeSpanStatus::null) )
	{
		dateSpanTemp.SetStatus(DateTimeSpanStatus::null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if ( (GetStatus() == DateTimeSpanStatus::invalid) || (dateSpan.GetStatus() == DateTimeSpanStatus::invalid) )
	{
		dateSpanTemp.SetStatus(DateTimeSpanStatus::invalid);
		return dateSpanTemp;
	}

	// Subtract spans and validate within legal range
	dateSpanTemp.m_span = m_span - dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

lkDateTimeSpan& lkDateTimeSpan::operator+=(const lkDateTimeSpan dateSpan)
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	wxASSERT(dateSpan.GetStatus() == DateTimeSpanStatus::valid);
	*this = *this + dateSpan;
	CheckRange();
	return *this;
}

lkDateTimeSpan& lkDateTimeSpan::operator-=(const lkDateTimeSpan dateSpan)
{
	wxASSERT(GetStatus() == DateTimeSpanStatus::valid);
	wxASSERT(dateSpan.GetStatus() == DateTimeSpanStatus::valid);
	*this = *this - dateSpan;
	CheckRange();
	return *this;
}

lkDateTimeSpan lkDateTimeSpan::operator-() const
{
	return -this->m_span;
}

lkDateTimeSpan::operator double() const
{
	return m_span;
}

void lkDateTimeSpan::SetDateTimeSpan(long lDays, int nHours, int nMins, int nSecs)
{
	// Set date span by breaking into fractional days (all input ranges valid)
	m_span = ((double)lDays) + ((double)nHours) / 24 + ((double)nMins) / (24 * 60) +
		((double)nSecs) / (24 * 60 * 60);
	m_status = DateTimeSpanStatus::valid;
	CheckRange();
}

void lkDateTimeSpan::CheckRange()
{
	if ( (m_span < -maxDaysInSpan) || (m_span > maxDaysInSpan ))
	{
		m_status = DateTimeSpanStatus::invalid;
	}
}

wxString lkDateTimeSpan::Format(const wxString& format) const
{
	wxString sRet;
	if ( GetStatus() == DateTimeSpanStatus::valid )
	{
		wxTimeSpan ts((long)GetTotalHours(), GetMinutes(), (wxLongLong)GetSeconds());
		sRet = ts.Format(format);
	}
	return sRet;
}


