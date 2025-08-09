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

wxString lkCharChanger(const wxString& src, const wxString& findOneOf, const wxString& replaceBy)
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

	dest = lkCharChanger(dest, wxT("àáâãäåāăąǻ"), wxT("a"));
	dest = lkCharChanger(dest, wxT("ÀÁÂÃÄÅĀĂĄǺ"), wxT("A"));
	dest = lkCharChanger(dest, wxT("çćĉċč"), wxT("c"));
	dest = lkCharChanger(dest, wxT("ÇĆĈĊČ"), wxT("C"));
	dest = lkCharChanger(dest, wxT("èéêëēĕėęě"), wxT("e"));
	dest = lkCharChanger(dest, wxT("ÈÉÊËĒĔĖĘĚ"), wxT("E"));
	dest = lkCharChanger(dest, wxT("ìíîïĩīĭįı"), wxT("i"));
	dest = lkCharChanger(dest, wxT("ÌÍÎÏĨĪĬĮİ"), wxT("I"));
	dest = lkCharChanger(dest, wxT("ðďđ"), wxT("d"));
	dest = lkCharChanger(dest, wxT("ÐĎĐ"), wxT("d"));
	dest = lkCharChanger(dest, wxT("ñńņňŉ"), wxT("n"));
	dest = lkCharChanger(dest, wxT("ÑŃŅŇŊ"), wxT("N"));
	dest = lkCharChanger(dest, wxT("òóôõöōŏő"), wxT("o"));
	dest = lkCharChanger(dest, wxT("ÒÓÔÕÖŌŎŐ"), wxT("O"));
	dest = lkCharChanger(dest, wxT("ùúûüũūŭůűų"), wxT("u"));
	dest = lkCharChanger(dest, wxT("ÙÚÛÜŨŪŬŮŰŲ"), wxT("U"));
	dest = lkCharChanger(dest, wxT("ýŷ"), wxT("y"));
	dest = lkCharChanger(dest, wxT("ÝŶ"), wxT("Y"));
	dest = lkCharChanger(dest, wxT("ß"), wxT("ss"));
	dest = lkCharChanger(dest, wxT("æ"), wxT("ae"));
	dest = lkCharChanger(dest, wxT("Æ"), wxT("AE"));
	dest = lkCharChanger(dest, wxT("Œ"), wxT("OE"));
	dest = lkCharChanger(dest, wxT("œ"), wxT("oe"));
	dest = lkCharChanger(dest, wxT("Ĳ"), wxT("IJ"));
	dest = lkCharChanger(dest, wxT("ÿ"), wxT("ij"));
	dest = lkCharChanger(dest, wxT("ĜĞĠĢ"), wxT("G"));
	dest = lkCharChanger(dest, wxT("ĝğġģ"), wxT("g"));
	dest = lkCharChanger(dest, wxT("ĤĦ"), wxT("H"));
	dest = lkCharChanger(dest, wxT("ĥħ"), wxT("h"));
	dest = lkCharChanger(dest, wxT("Ĵ"), wxT("J"));
	dest = lkCharChanger(dest, wxT("ĵ"), wxT("j"));
	dest = lkCharChanger(dest, wxT("Ķ"), wxT("K"));
	dest = lkCharChanger(dest, wxT("ķĸ"), wxT("k"));
	dest = lkCharChanger(dest, wxT("ĹĻĽĿŁ"), wxT("L"));
	dest = lkCharChanger(dest, wxT("ĺļľŀł"), wxT("l"));
	dest = lkCharChanger(dest, wxT("ŔŖŘ"), wxT("R"));
	dest = lkCharChanger(dest, wxT("ŕŗř"), wxT("r"));
	dest = lkCharChanger(dest, wxT("ŚŜŞŠ"), wxT("S"));
	dest = lkCharChanger(dest, wxT("śŝşš"), wxT("s"));
	dest = lkCharChanger(dest, wxT("ŢŤŦ"), wxT("T"));
	dest = lkCharChanger(dest, wxT("ţťŧ"), wxT("t"));
	dest = lkCharChanger(dest, wxT("Ŵ"), wxT("W"));
	dest = lkCharChanger(dest, wxT("ŵ"), wxT("w"));
	dest = lkCharChanger(dest, wxT("ŹŻŽ"), wxT("Z"));
	dest = lkCharChanger(dest, wxT("źżž"), wxT("z"));

	dest = lkCharChanger(dest, wxT("—"), wxT("-"));
	dest = lkCharChanger(dest, wxT("’"), wxT("'"));
	dest = lkCharChanger(dest, wxT("`"), wxT("'"));

	return dest;
}

