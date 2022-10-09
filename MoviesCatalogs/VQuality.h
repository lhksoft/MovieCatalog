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
#ifndef __V_QUALITY_H__
#define __V_QUALITY_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
#include "TQuality.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityDocument
////
class QualityDocument : public lkSQL3RecordDocument, public TQuality
{
public:
	QualityDocument(wxDocument* pParent);

// Attributes
protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

public:
	virtual void					BindButtons(void) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(QualityDocument);
	wxDECLARE_NO_COPY_CLASS(QualityDocument);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityView
////
class QualityView : public lkSQL3RecordView
{
public:
	QualityView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					OnUpdate(wxView* sender, wxObject* hint = NULL) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(QualityView);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityTemplate
////
class QualityTemplate : public lkChildDocTemplate
{
public:
	QualityTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument* DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate* CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(QualityTemplate);
	wxDECLARE_NO_COPY_CLASS(QualityTemplate);
};

#endif // !__V_QUALITY_H__
