/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2025 by Laurens Koehoorn (lhksoft)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#include "TMovies_private.h"
#include "../lkSQLite3/internal_tools.h" // for 'lkMakeAnsi'

#include <wx/filename.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/dir.h>
// Following for memcmp
#ifdef __WXMSW__
#include <iostream>
#else
#include <string.h>
#endif

wxString tmCorrectPath(const wxString& value)
{
	wxString v = value;

	if (wxFileName::GetPathSeparator() == '\\')
	{
		if (v.Find('/') != wxNOT_FOUND)
			v.Replace(wxT("/"), wxT("\\"), true);
	}
	else if (wxFileName::GetPathSeparator() == '/')
	{
		if (v.Find('\\') != wxNOT_FOUND)
			v.Replace(wxT("\\"), wxT("/"), true);
	}

	return v; // might be empty i value was empty
}

wxString tmMakeFullPath(const wxString& sPrefix, const wxString& partial) // empty if NULL
{
	if (sPrefix.IsEmpty())
		return partial;

	wxString value = partial;
	if (!value.IsEmpty())
	{
		value = tmCorrectPath(value);

		if (!wxFileExists(value))
		{ // no full path
			wxString sCovers = sPrefix;
			sCovers += value;
			value = sCovers;
			if (value.Find('\\') != wxNOT_FOUND)
				value.Replace(wxT("\\"), wxT("/"), true);
		}
	}
	return value;
}

wxString tmExtractFirstPath(const wxString& full, const wxString& eWhat)
{
	if (full.IsEmpty())
		return wxT("");

	wxString fS = full;
	if (fS.Find('\\') != wxNOT_FOUND)
		fS.Replace(wxT("\\"), wxT("/"), true);
	if (!eWhat.IsEmpty())
	{
		wxString eS = eWhat;
		if (eS.Find('\\') != wxNOT_FOUND)
			eS.Replace(wxT("\\"), wxT("/"), true);

		if (((int)fS.Find(eS)) == 0)
			fS = fS.Mid(eS.Len());
	}
	return fS;
}

wxUniChar tmFirstPathInRange(const wxString& range, const wxString& path)
{
	wxUniChar uc;
	if (range.IsEmpty() || path.IsEmpty())
		return uc;

	wxString s = path.Upper(), r = range.Upper();

	if (s.Find('\\') != wxNOT_FOUND)
		s.Replace(wxT("\\"), wxT("/"), true);
	if (s.Len() < 2 || s[1] != '/')
		return uc;

	for (int p = 0; p < r.Len(); p++)
	{
		if (s[0] == r[p])
		{
			uc = r[p];
			break;
		}
	}

	return uc;
}

wxString tmPrefixCover(const wxString& cFilename)
{
	if (cFilename.IsEmpty())
		return wxT("");

	wxUniChar uc;
	wxString s, sF, sRange = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	sF = lkMakeAnsi(cFilename); // preferable not to have unicode in a filename
	s = sF;
	s.MakeUpper();
	for (int p = 0; p < sRange.Len(); p++)
	{
		if (s[0] == sRange[p])
		{
			uc = sRange[p];
			break;
		}
	}

	if (uc.GetValue() == 0)
		uc = '0';

	s = wxT("");
	s += uc;
	s += wxT("/");
	s += sF;

	return s;
}

bool tmCompareFiles(const wxString& sSrc, const wxString& sDst)
{
	if (sSrc.IsEmpty() || sDst.IsEmpty())
		return false;
	if (!wxFileExists(sSrc) || !wxFileExists(sDst))
		return false;

	wxULongLong	sz1 = wxFileName::GetSize(sSrc);
	wxULongLong sz2 = wxFileName::GetSize(sDst);

	if ((sz1 == wxInvalidSize) || (sz2 == wxInvalidSize))
		return false;
	if (sz1 != sz2)
		return false;

	bool ret = false;
	wxFile f1(sSrc), f2(sDst);
	if (f1.IsOpened() && f2.IsOpened())
	{
		wxFileOffset o = f1.SeekEnd(); f1.Seek(0);
		size_t s2, s1 = sizeof(wxByte) * static_cast<size_t>(o);
		wxByte* b2 = NULL, *b1 = new wxByte[s1];
		bool ok2, ok1 = (f1.Read(b1, s1) == s1);
		if (ok1)
		{ // no need to continue if previous Read somehow failed
			o = f2.SeekEnd(); f2.Seek(0);
			s2 = sizeof(wxByte) * static_cast<size_t>(o);
			b2 = new wxByte[s2];
			ok2 = (f2.Read(b2, s2) == s2);
		}
		if (ret = (ok1 && ok2) ? (s1 == s2) : false)
#ifdef __WXMSW__
			ret = (std::memcmp(b1, b2, s1) == 0);
#else
			ret = (memcmp(b1, b2, s1) == 0);
#endif
		if (b2) delete b2;
		if (b1) delete b1;
	}
	return ret;
}

bool tmCopyFile(const wxString& sSrc, const wxString& sDst)
{
	if (sSrc.IsEmpty() || sDst.IsEmpty())
		return false;
	if (!wxFileExists(sSrc))
		return false;

	wxString path = wxPathOnly(sDst);
	if (!path.IsEmpty() && !wxDirExists(path))
	{
		wxFileName dir(sDst);
		if (!dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) // wxPOSIX_USER_READ | wxPOSIX_USER_WRITE
			return false;
	}
	if (!wxCopyFile(sSrc, sDst))
		return false;

	wxFileName f1(sSrc), f2(sDst);
	wxDateTime dtA, dtM, dtC;
	if (f1.GetTimes(&dtA, &dtM, &dtC))
	{
		if (dtM < dtC)
			dtC = dtM;
		f2.SetTimes(&dtA, &dtM, &dtC);
	}
	return true;
}

wxString tmMakeLog(const wxUint64 rowID, const wxString& movieTitle)
{
	wxString s;
	s.Printf(wxT("%llu;%s;"), rowID, movieTitle);
	return s;
}

static void tmTraverseFiles(const wxString& sDir, wxDateTime& dM)
{
	if (sDir.IsEmpty())
		return;

	wxString sD = tmCorrectPath(sDir);
	if (sD.Right(1) != wxFileName::GetPathSeparator())
		sD += wxFileName::GetPathSeparator();

	if (wxDirExists(sD))
	{
		wxDateTime dtM;
		wxDir dir(sD);
		if (dir.IsOpened())
		{
			wxString f, realPath, sDir;
			wxFileName fn;
			bool bFind = dir.GetFirst(&f, wxEmptyString, wxDIR_DEFAULT | wxDIR_NO_FOLLOW);
			while (bFind)
			{
				realPath = sD + f;
				fn = realPath;
				if (fn.FileExists())
				{
					wxDateTime m;
					if (fn.GetTimes(NULL, &m, NULL))
					{
						if (dM.IsValid())
						{
							if (m > dM)
								dM = m;
						}
						else
							dM = m;
					}
				}
				else
				{
					sDir = realPath;
					if (sDir.Right(1) != wxFileName::GetPathSeparator())
						sDir += wxFileName::GetPathSeparator();
					fn = sDir;
					wxASSERT(wxDirExists(sDir));
					// must be a Dir
					dtM = (time_t)-1;
					tmTraverseFiles(realPath, dtM);
					if (dtM.IsValid())
						fn.SetTimes(NULL, &dtM, NULL);
					if (dM.IsValid())
					{
						if (dtM > dM)
							dM = dtM;
					}
					else
						dM = dtM;
				}

				bFind = dir.GetNext(&f);
			}
		}
	}
}
void tmResetDirTimes(const wxString& sNewPath)
{
	if (sNewPath.IsEmpty())
		return;

	wxString sD = tmCorrectPath(sNewPath);

	if (wxDirExists(sD))
	{
		if (sD.Right(1) == wxFileName::GetPathSeparator())
			sD = sD.Left(sD.Len() - 1);

		wxDateTime dtM((time_t)-1);
		tmTraverseFiles(sD, dtM);
		if (dtM.IsValid())
		{
			if (sD.Right(1) != wxFileName::GetPathSeparator())
				sD += wxFileName::GetPathSeparator();
			wxFileName fn(sD);
			fn.SetTimes(NULL, &dtM, NULL);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxString tmMoveFirst(const wxString& sOldPath, const wxString& sOldPrefix)
{
	const wxString sRange = wxT("0ABCDEFGHIJKLMNOPQRSTUVWXYZ"), sFilms = wxT("Films/"), sSeries = wxT("Series/");
	const wxString sBond = wxT("007 - James Bond/"), sHK = wxT("HongKong/");
	wxString sOlder, sNewPath, s1, s2;
	wxUniChar c;

	sNewPath = wxT("");
	// extract the old part
	sOlder = tmExtractFirstPath(sOldPath, sOldPrefix);
	// Checking if we're dealing with Films/Series cover
	if (!(s1 = tmExtractFirstPath(sOlder, sFilms)).IsEmpty() && s1.Len() < sOlder.Len())
	{ // dealing with a Film
		sNewPath += sFilms;

		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			sNewPath += s1; // as is as
		else if (s1.Find(sBond) == 0)
		{
			s1 = tmExtractFirstPath(s1, sBond);
			sNewPath += wxT("0/007_");
			sNewPath += s1;
		}
		else if (s1.Find(sHK) == 0)
		{
			s1 = tmExtractFirstPath(s1, sHK);
			sNewPath += tmPrefixCover(s1);
		}
		else if (s1.Find(wxT("Band of Brothers")) == 0)
		{
			sNewPath = sSeries;
			sNewPath += tmPrefixCover(s1);
		}
		else if (s1.Find(wxT("/")) != wxNOT_FOUND)
		{
			if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			{
				s1 = s1.Mid(2);
				sNewPath += tmPrefixCover(s1);
			}
			else
				sNewPath += s1;
		}
		else
			sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, sSeries)).IsEmpty() && s1.Len() < sOlder.Len())
	{
		sNewPath += sSeries;
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			sNewPath += s1; // as is as
		else if (s1.Find(wxT("/")) != wxNOT_FOUND)
		{
			if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			{
				s1 = s1.Mid(2);
				sNewPath += tmPrefixCover(s1);
			}
			else
				sNewPath += s1;
		}
		else
			sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Diverse/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{ // resort what's in "Diverse"
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		if ((s1.Find(wxT("2 U")) == 0) || (s1.Find(wxT("Anathema")) == 0) || (s1.Find(wxT("Bart")) == 0) || (s1.Find(wxT("Cocks")) == 0) ||
			(s1.Find(wxT("Doors")) == 0) || (s1.Find(wxT("Faith")) == 0) || (s1.Find(wxT("Front")) == 0) || (s1.Find(wxT("Gothic")) == 0) ||
			(s1.Find(wxT("Judas")) == 0) || (s1.Find(wxT("Opeth")) == 0) || (s1.Find(wxT("Pink")) == 0) || (s1.Find(wxT("Rockbitch")) == 0) ||
			(s1.Find(wxT("Sash")) == 0) || (s1.Find(wxT("TAMI")) == 0) || (s1.Find(wxT("The C")) == 0))
		{
			sNewPath += wxT("Music/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("Aftermath")) == 0) || (s1.Find(wxT("Alien")) == 0) || (s1.Find(wxT("Autisme")) == 0) || (s1.Find(wxT("BelPop")) == 0) ||
			(s1.Find(wxT("Black hole")) == 0) || (s1.Find(wxT("Hond")) == 0) || (s1.Find(wxT("Inside")) == 0) || (s1.Find(wxT("Mijn")) == 0) ||
			(s1.Find(wxT("NGC")) == 0) || (s1.Find(wxT("Pixar")) == 0) || (s1.Find(wxT("Rotterdam")) == 0) || (s1.Find(wxT("Voetba")) == 0))
		{
			sNewPath += wxT("Documentaires/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("Buiten")) == 0) || (s1.Find(wxT("Chef")) == 0) ||
			(s1.Find(wxT("CSI")) == 0) || (s1.Find(wxT("JAG")) == 0) || (s1.Find(wxT("Waking")) == 0) ||
			(s1.Find(wxT("Wexford")) == 0) || (s1.Find(wxT("Willem")) == 0))
		{
			sNewPath += sSeries;
			sNewPath += tmPrefixCover(s1);
		}
		else if (s1.Find(wxT("CreekS")) == 0)
		{
			sNewPath += wxT("Creek Jonathan/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("Bumba")) == 0) || (s1.Find(wxT("De Wielen")) == 0) || (s1.Find(wxT("Sesamstraat")) == 0) || (s1.Find(wxT("Totally")) == 0))
		{
			sNewPath += wxT("Kids/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("Moods")) == 0) || (s1.Find(wxT("Karaoke")) == 0))
		{
			sNewPath += wxT("Diverse/");
			sNewPath += s1;
		}
		else
			sNewPath += s1;
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Monarch of the Glen/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += sSeries;
		s1 = wxT("Monarch_ot_Glen-") + s1;
		sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Space1999/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += sSeries;
		s1 = wxT("Space1999-") + s1;
		sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Shin-Chan/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += sSeries;
		s1 = wxT("ShinChan-") + s1;
		sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Smurfen/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += wxT("Kids/");
		sNewPath += s1;
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Teletubbies/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += wxT("Kids/Teletubbies-");
		sNewPath += s1;
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("WW2/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += wxT("Documentaires/WW2-");
		sNewPath += s1;
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("st-Ent/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += sSeries;
		sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, wxT("Galactica/"))).IsEmpty() && s1.Len() < sOlder.Len())
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += sSeries;
		sNewPath += tmPrefixCover(s1);
	}
	else if (s1.Find(wxT("/")) == wxNOT_FOUND)
	{ // resort nameless files
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		if ((s1.Find(wxT("911")) == 0) || (s1.Find(wxT("BlackHoles")) == 0) || (s1.Find(wxT("GreatYear")) == 0) || (s1.Find(wxT("Hitler")) == 0) ||
			(s1.Find(wxT("horizon")) == 0) || (s1.Find(wxT("Hot spring")) == 0) || (s1.Find(wxT("Hubble")) == 0) || (s1.Find(wxT("latest")) == 0) ||
			(s1.Find(wxT("ngc")) == 0) || (s1.Find(wxT("OverLeven")) == 0) || (s1.Find(wxT("Pim")) == 0) || (s1.Find(wxT("Space")) == 0) ||
			(s1.Find(wxT("tinain")) == 0) || (s1.Find(wxT("trefoil")) == 0) || (s1.Find(wxT("Paleo")) == 0))
		{
			sNewPath += wxT("Documentaires/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("Best Dance")) == 0) || (s1.Find(wxT("Eurovision")) == 0) || (s1.Find(wxT("GrH")) == 0) || (s1.Find(wxT("Kim Wilde")) == 0) ||
			(s1.Find(wxT("Rammstein")) == 0) || (s1.Find(wxT("Remember")) == 0) || (s1.Find(wxT("Tina T")) == 0) || (s1.Find(wxT("Totp")) == 0))
		{
			sNewPath += wxT("Music/");
			sNewPath += s1;
		}
		else if ((s1.Find(wxT("A Case of Coin")) == 0) || (s1.Find(wxT("A Certain")) == 0) || (s1.Find(wxT("Baantj")) == 0) || (s1.Find(wxT("Bleach")) == 0) ||
			(s1.Find(wxT("dp")) == 0) || (s1.Find(wxT("Dahziel")) == 0) || (s1.Find(wxT("DangerField")) == 0) || (s1.Find(wxT("DogWoman")) == 0) ||
			(s1.Find(wxT("Frost")) == 0) || (s1.Find(wxT("Gadget")) == 0) || (s1.Find(wxT("Hercule")) == 0) || (s1.Find(wxT("Inspector")) == 0) ||
			(s1.Find(wxT("Jjd")) == 0) || (s1.Find(wxT("Kavi")) == 0) || (s1.Find(wxT("Kuifje")) == 0) || (s1.Find(wxT("Lynley")) == 0) ||
			(s1.Find(wxT("Mind")) == 0) || (s1.Find(wxT("Morse")) == 0) || (s1.Find(wxT("murder")) == 0) || (s1.Find(wxT("Murder")) == 0) || (s1.Find(wxT("morse")) == 0) ||
			(s1.Find(wxT("Ruth")) == 0) || (s1.Find(wxT("Sandokan")) == 0) || (s1.Find(wxT("Silent")) == 0) || (s1.Find(wxT("Simpsons")) == 0) ||
			(s1.Find(wxT("SixM")) == 0) || (s1.Find(wxT("Spangen")) == 0) || (s1.Find(wxT("whistle")) == 0))
		{
			sNewPath += sSeries;
			sNewPath += tmPrefixCover(s1);
		}
		else if (s1.Find(wxT("Smurfs")) == 0)
		{
			sNewPath += sFilms;
			sNewPath += tmPrefixCover(s1);
		}
		else
			sNewPath += s1;
	}
	else
	{
		s1 = lkCharChanger(s1, wxT("&',"), wxT(""));
		sNewPath += s1;
	}

	return sNewPath;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxString tmCoverResort(const wxString& sOldPath, const wxString& sOldPrefix)
{
	const wxString sRange = wxT("0ABCDEFGHIJKLMNOPQRSTUVWXYZ"), sFilms = wxT("Films/"), sSeries = wxT("Series/");
	wxString sOlder, sNewPath, s1, s2;
	wxUniChar c;

	sNewPath = wxT("");
	// extract the old part
	sOlder = tmExtractFirstPath(sOldPath, sOldPrefix);
	sOlder = lkCharChanger(sOlder, wxT("&',"), wxT(""));
	// Checking if we're dealing with Films/Series cover
	if (!(s1 = tmExtractFirstPath(sOlder, sFilms)).IsEmpty() && s1.Len() < sOlder.Len())
	{ // dealing with a Film
		sNewPath += sFilms;
		if (s1.Find(wxT("/")) != wxNOT_FOUND)
		{
			if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			{
				s1 = s1.Mid(2);
				sNewPath += tmPrefixCover(s1);
			}
			else
				sNewPath += s1;
		}
		else
			sNewPath += tmPrefixCover(s1);
	}
	else if (!(s1 = tmExtractFirstPath(sOlder, sSeries)).IsEmpty() && s1.Len() < sOlder.Len())
	{
		sNewPath += sSeries;
		if (s1.Find(wxT("/")) != wxNOT_FOUND)
		{
			if ((c = tmFirstPathInRange(sRange, s1)).GetValue() != 0)
			{
				s1 = s1.Mid(2);
				sNewPath += tmPrefixCover(s1);
			}
			else
				sNewPath += s1;
		}
		else
			sNewPath += tmPrefixCover(s1);
	}
	else if (s1.Find(wxT("/")) == wxNOT_FOUND)
	{ // resort nameless files
		sNewPath += s1;
	}
	else
	{
		sNewPath += s1;
	}

	return sNewPath;

}