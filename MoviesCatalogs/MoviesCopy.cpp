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
#include "MoviesCopy.h"

MoviesCopy::MoviesCopy() : m_Cover(), m_Title(), m_SubTitle(), m_Alias(), m_Episode(), m_Country(), m_URL(), m_About()
{
	Init();
}
MoviesCopy::~MoviesCopy()
{
}

void MoviesCopy::Init()
{
	m_bInUse = false;
	m_Cat = m_SubCat = 0;
	m_Genres = 0;
	m_Date = 0.0;
	m_YrBox = false;
	m_Serie = false;
	m_Judge = 0;
	m_Ratio = 0;
}

bool MoviesCopy::IsInUse() const
{
	return m_bInUse;
}
void MoviesCopy::SetInUse(bool b)
{
	m_bInUse = b;
}

wxString MoviesCopy::GetCover() const
{
	return m_Cover;
}
void MoviesCopy::SetCover(const wxString& c)
{
	m_Cover = c;
}

wxString MoviesCopy::GetTitle() const
{
	return m_Title;
}
void MoviesCopy::SetTitle(const wxString& t)
{
	m_Title = t;
}

wxString MoviesCopy::GetSubTitle() const
{
	return m_SubTitle;
}
void MoviesCopy::SetSubTitle(const wxString& s)
{
	m_SubTitle = s;
}

wxString MoviesCopy::GetAlias() const
{
	return m_Alias;
}
void MoviesCopy::SetAlias(const wxString& a)
{
	m_Alias = a;
}

wxString MoviesCopy::GetEpisode() const
{
	return m_Episode;
}
void MoviesCopy::SetEpisode(const wxString& e)
{
	m_Episode = e;
}

wxUint64 MoviesCopy::GetCategory() const
{
	return m_Cat;
}
void MoviesCopy::SetCategory(wxUint64 c)
{
	m_Cat = c;
}

wxUint64 MoviesCopy::GetSubCat() const
{
	return m_SubCat;
}
void MoviesCopy::SetSubCat(wxUint64 s)
{
	m_SubCat = s;
}

wxUint64 MoviesCopy::GetGenres() const
{
	return m_Genres;
}
void MoviesCopy::SetGenres(wxUint64 g)
{
	m_Genres = g;
}

wxString MoviesCopy::GetCountry() const
{
	return m_Country;
}
void MoviesCopy::SetCountry(const wxString& c)
{
	m_Country = c;
}

double MoviesCopy::GetDate() const
{
	return m_Date;
}
void MoviesCopy::SetDate(double d)
{
	m_Date = d;
}

bool MoviesCopy::GetYrBox() const
{
	return m_YrBox;
}
void MoviesCopy::SetYrBox(bool y)
{
	m_YrBox = y;
}

bool MoviesCopy::GetSerie() const
{
	return m_Serie;
}
void MoviesCopy::SetSerie(bool s)
{
	m_Serie = s;
}

wxString MoviesCopy::GetURL() const
{
	return m_URL;
}
void MoviesCopy::SetURL(const wxString& u)
{
	m_URL = u;
}

wxUint64 MoviesCopy::GetJudge() const
{
	return m_Judge;
}
void MoviesCopy::SetJudge(wxUint64 j)
{
	m_Judge = j;
}

wxUint64 MoviesCopy::GetRatio() const
{
	return m_Ratio;
}
void MoviesCopy::SetRatio(wxUint64 r)
{
	m_Ratio = r;
}

wxString MoviesCopy::GetAbout() const
{
	return m_About;
}
void MoviesCopy::SetAbout(const wxString& a)
{
	m_About = a;
}
