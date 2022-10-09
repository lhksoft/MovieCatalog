/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
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
#ifndef __MOVIES_COPY_H__
#define __MOVIES_COPY_H__
#pragma once

#include <wx/string.h>

/////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesCopy
// ---------------
// This simple class will be used for copying data of (all) the controls currently active in
// CMovies and can be paste into an empty record when in Add modus.
// Copy/Paste will be done through the TransferData-routines
////
class MoviesCopy
{
public:
	MoviesCopy();
	virtual ~MoviesCopy();

	enum class What
	{
		Invalid = 0,
		ImgPath,
		Title,
		SubTitle,
		Alias,
		Episode,
		Cat,
		SubCat,
		Genres,
		Country,
		Date,
		Year,
		YrBox,
		Serie,
		URL,
		Judge,
		Ratio,
		About
	};
// Attributes
public:
	bool							IsInUse(void) const;
	void							SetInUse(bool);

	wxString						GetCover(void) const;
	void							SetCover(const wxString&);

	wxString						GetTitle(void) const;
	void							SetTitle(const wxString&);

	wxString						GetSubTitle(void) const;
	void							SetSubTitle(const wxString&);

	wxString						GetAlias(void) const;
	void							SetAlias(const wxString&);

	wxString						GetEpisode(void) const;
	void							SetEpisode(const wxString&);

	wxUint64						GetCategory(void) const;
	void							SetCategory(wxUint64);

	wxUint64						GetSubCat(void) const;
	void							SetSubCat(wxUint64);

	wxUint64						GetGenres(void) const;
	void							SetGenres(wxUint64);

	wxString						GetCountry(void) const;
	void							SetCountry(const wxString&);

	double							GetDate(void) const;
	void							SetDate(double);

	bool							GetYrBox(void) const;
	void							SetYrBox(bool);

	bool							GetSerie(void) const;
	void							SetSerie(bool);

	wxString						GetURL(void) const;
	void							SetURL(const wxString&);

	wxUint64						GetJudge(void) const;
	void							SetJudge(wxUint64);

	wxUint64						GetRatio(void) const;
	void							SetRatio(wxUint64);

	wxString						GetAbout(void) const;
	void							SetAbout(const wxString&);

protected:
	bool							m_bInUse;	// true if Tool-Button 'Copy' clicked and so if true TransferData should set/retrieve data from this class, if false TransferData should ignore this class
												// set to True to copy/set data from this class, if done, set to False for normal operation of the programm, when the data is required, set it to True again

	wxString						m_Cover;	// path to cover, empty if no image
	wxString						m_Title, m_SubTitle, m_Alias;
	wxString						m_Episode;
	wxUint64						m_Cat, m_SubCat;
	wxUint64						m_Genres;
	wxString						m_Country;
	double							m_Date;		// used by What::Date and What::Year
	bool							m_YrBox, m_Serie;
	wxString						m_URL;
	wxUint64						m_Judge, m_Ratio;
	wxString						m_About;

private:
	void							Init(void);


	wxDECLARE_NO_COPY_CLASS(MoviesCopy);
};

#endif // !__MOVIES_COPY_H__
