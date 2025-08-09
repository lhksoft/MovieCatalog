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
#ifndef __SIMPLELISTBOX_H__
#define __SIMPLELISTBOX_H__
#pragma once

#include "../lkControls/lkChkImgListbox.h"
#include "../lkSQLite3/lkSQL3Database.h"
#include "lkSQL3RecordView.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  PreviewListbox
////
class PreviewListbox : public lkChkImgListbox
{
public:
	PreviewListbox();
	PreviewListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc = NULL);

	virtual bool			BuildList(void) = 0;

protected:
	void					OnDblClick(wxMouseEvent&);

	virtual wxString		GetOrder(void) const; // will get it from the Document (if any was set)

protected:
	lkSQL3Database*			m_pDB;
	lkSQL3RecordDocument*	m_pDoc;

private:
	wxDECLARE_EVENT_TABLE();

	wxDECLARE_ABSTRACT_CLASS(PreviewListbox);
	wxDECLARE_NO_COPY_CLASS(PreviewListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  SubCatListbox
////
class SubCatListbox : public lkChkImgListbox
{
public:
	SubCatListbox();
	SubCatListbox(lkSQL3Database* pDB, wxUint32* nCat);
	virtual ~SubCatListbox();

	bool				BuildList(void);

private:
	lkSQL3Database*		m_pDB;
	wxUint32*			m_pCat;

	wxDECLARE_CLASS(SubCatListbox);
	wxDECLARE_NO_COPY_CLASS(SubCatListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryListbox
////
class CountryListbox : public PreviewListbox
{
public:
	CountryListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

protected:
	lkExpandFlags		m_ExpandFlag;

private:
	wxDECLARE_CLASS(CountryListbox);
	wxDECLARE_NO_COPY_CLASS(CountryListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  GenreListbox
////
class GenreListbox : public PreviewListbox
{
public:
	GenreListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(GenreListbox);
	wxDECLARE_NO_COPY_CLASS(GenreListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  JudgeListbox
////
class JudgeListbox : public PreviewListbox
{
public:
	JudgeListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(JudgeListbox);
	wxDECLARE_NO_COPY_CLASS(JudgeListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class LocationListbox
////
class LocationListbox : public PreviewListbox
{
public:
	LocationListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(LocationListbox);
	wxDECLARE_NO_COPY_CLASS(LocationListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MediumListbox
////
class MediumListbox : public PreviewListbox
{
public:
	MediumListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(MediumListbox);
	wxDECLARE_NO_COPY_CLASS(MediumListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class OrigineListbox
////
class OrigineListbox : public PreviewListbox
{
public:
	OrigineListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(OrigineListbox);
	wxDECLARE_NO_COPY_CLASS(OrigineListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityListbox
////
class QualityListbox : public PreviewListbox
{
public:
	QualityListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(QualityListbox);
	wxDECLARE_NO_COPY_CLASS(QualityListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RatingListbox
////
class RatingListbox : public PreviewListbox
{
public:
	RatingListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(RatingListbox);
	wxDECLARE_NO_COPY_CLASS(RatingListbox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenListbox
////
class ScreenListbox : public PreviewListbox
{
public:
	ScreenListbox(lkSQL3Database* pDB, lkSQL3RecordDocument*);

	virtual bool		BuildList(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(ScreenListbox);
	wxDECLARE_NO_COPY_CLASS(ScreenListbox);
};

#endif // !__SIMPLELISTBOX_H__