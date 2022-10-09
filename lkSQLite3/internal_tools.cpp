/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "internal_tools.h"
#include <string>

// //////////////////////////////////////////////////////////////////////////////////////////////

wxString lkTrimRight(const wxString& str, const wxString& trim)
{
	wxString result;
	if ( str.IsEmpty() || trim.IsEmpty() )
		return result;

	std::string s = str.ToStdString();
	s.erase(s.find_last_not_of(trim) + 1);

	result = s;
	return result;
}
wxString lkTrimLeft(const wxString& str, const wxString& trim)
{
	wxString result;
	if ( str.IsEmpty() || trim.IsEmpty() )
		return result;

	std::string s = str.ToStdString();
	s.erase(0, s.find_first_not_of(trim));

	result = s;
	return result;
}
wxString lkTrim(const wxString& str, const wxString& trim)
{
	return lkTrimRight(lkTrimLeft(str, trim), trim);
}

// //////////////////////////////////////////////////////////////////////////////////////////////

wxString _CharChanger(const wxString& src, const wxString& findOneOf, const wxString& replaceBy)
{
	if ( src.IsEmpty() || findOneOf.IsEmpty() )
		return src;

//	wxWCharBuffer data = findOneOf.wc_str();
//	int maxFind = findOneOf.Len();
	wxString dest = src;

	for ( int i = 0; i < findOneOf.Len(); i++ )
		dest.Replace(findOneOf[i], replaceBy);

	return dest;
}

wxString lkMakeAnsi(const wxString& src)
{
	if ( src.IsEmpty() ) return src;

	wxString dest = src;

	dest = _CharChanger(dest, wxT("àáâãäåāăąǻ"), wxT("a"));
	dest = _CharChanger(dest, wxT("ÀÁÂÃÄÅĀĂĄǺ"), wxT("A"));
	dest = _CharChanger(dest, wxT("çćĉċč"), wxT("c"));
	dest = _CharChanger(dest, wxT("ÇĆĈĊČ"), wxT("C"));
	dest = _CharChanger(dest, wxT("èéêëēĕėęě"), wxT("e"));
	dest = _CharChanger(dest, wxT("ÈÉÊËĒĔĖĘĚ"), wxT("E"));
	dest = _CharChanger(dest, wxT("ìíîïĩīĭįı"), wxT("i"));
	dest = _CharChanger(dest, wxT("ÌÍÎÏĨĪĬĮİ"), wxT("I"));
	dest = _CharChanger(dest, wxT("ðďđ"), wxT("d"));
	dest = _CharChanger(dest, wxT("ÐĎĐ"), wxT("d"));
	dest = _CharChanger(dest, wxT("ñńņňŉ"), wxT("n"));
	dest = _CharChanger(dest, wxT("ÑŃŅŇŊ"), wxT("N"));
	dest = _CharChanger(dest, wxT("òóôõöōŏő"), wxT("o"));
	dest = _CharChanger(dest, wxT("ÒÓÔÕÖŌŎŐ"), wxT("O"));
	dest = _CharChanger(dest, wxT("ùúûüũūŭůűų"), wxT("u"));
	dest = _CharChanger(dest, wxT("ÙÚÛÜŨŪŬŮŰŲ"), wxT("U"));
	dest = _CharChanger(dest, wxT("ýŷ"), wxT("y"));
	dest = _CharChanger(dest, wxT("ÝŶ"), wxT("Y"));
	dest = _CharChanger(dest, wxT("ß"), wxT("ss"));
	dest = _CharChanger(dest, wxT("æ"), wxT("ae"));
	dest = _CharChanger(dest, wxT("Æ"), wxT("AE"));
	dest = _CharChanger(dest, wxT("Œ"), wxT("OE"));
	dest = _CharChanger(dest, wxT("œ"), wxT("oe"));
	dest = _CharChanger(dest, wxT("Ĳ"), wxT("IJ"));
	dest = _CharChanger(dest, wxT("ÿ"), wxT("ij"));
	dest = _CharChanger(dest, wxT("ĜĞĠĢ"), wxT("G"));
	dest = _CharChanger(dest, wxT("ĝğġģ"), wxT("g"));
	dest = _CharChanger(dest, wxT("ĤĦ"), wxT("H"));
	dest = _CharChanger(dest, wxT("ĥħ"), wxT("h"));
	dest = _CharChanger(dest, wxT("Ĵ"), wxT("J"));
	dest = _CharChanger(dest, wxT("ĵ"), wxT("j"));
	dest = _CharChanger(dest, wxT("Ķ"), wxT("K"));
	dest = _CharChanger(dest, wxT("ķĸ"), wxT("k"));
	dest = _CharChanger(dest, wxT("ĹĻĽĿŁ"), wxT("L"));
	dest = _CharChanger(dest, wxT("ĺļľŀł"), wxT("l"));
	dest = _CharChanger(dest, wxT("ŔŖŘ"), wxT("R"));
	dest = _CharChanger(dest, wxT("ŕŗř"), wxT("r"));
	dest = _CharChanger(dest, wxT("ŚŜŞŠ"), wxT("S"));
	dest = _CharChanger(dest, wxT("śŝşš"), wxT("s"));
	dest = _CharChanger(dest, wxT("ŢŤŦ"), wxT("T"));
	dest = _CharChanger(dest, wxT("ţťŧ"), wxT("t"));
	dest = _CharChanger(dest, wxT("Ŵ"), wxT("W"));
	dest = _CharChanger(dest, wxT("ŵ"), wxT("w"));
	dest = _CharChanger(dest, wxT("ŹŻŽ"), wxT("Z"));
	dest = _CharChanger(dest, wxT("źżž"), wxT("z"));

	dest = _CharChanger(dest, wxT("—"), wxT("-"));
	dest = _CharChanger(dest, wxT("’"), wxT("'"));
	dest = _CharChanger(dest, wxT("`"), wxT("'"));

	return dest;
}

