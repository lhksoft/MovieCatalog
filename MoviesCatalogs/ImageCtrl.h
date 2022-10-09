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
#ifndef __IMAGECTRL_H__
#define __IMAGECTRL_H__
#pragma once

#include "../lkControls/lkStaticImage.h"
#include "../lkControls/lkView.h"
//#include <wx/confbase.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class ImageCtrl : using a wxImage
////
// This class will create a popup menu, allowing one to select an image through an OpenImageDialog
class ImageCtrl : public lkStaticImage
{
public:
	ImageCtrl();
	ImageCtrl(wxWindow*, const wxSize&, wxWindowID id = wxID_ANY, const wxValidator& validator = wxDefaultValidator);
	virtual ~ImageCtrl();

// Attributes
protected:
	wxString							m_imgName; // retrieved from the DB, or the filename when setting new image from disk
	lkView*								m_pView;
	wxString							m_ConfigSection;

public:
	wxString							GetImageName(void) const;
	void								SetImageName(const wxString&);

	void								SetView(lkView*);
	void								SetConfigSection(const wxString&);

// Implementation
private:
	void								Init(void);

protected:
	void								NewImage(void);
	void								ClearImage(void);
	bool								SaveImage(void);

	int									ConfirmRemoval(void); // shows a messagebox asking whether the image has to be safed or not before removal, returns wxYES, wxNO or wxCANCEL

public:
	static bool							DoSaveImage(wxWindow* pParent, const wxImage&, const wxString& iName, const wxString& confSection, const wxString& confKey = wxEmptyString);

private:
	enum class menu_ids
	{
		ID_New = wxID_HIGHEST + 1,
		ID_Clear,
		ID_Save
	};

// Events
protected:
	void								OnRightClick(wxMouseEvent&);
	void								OnMenu(wxCommandEvent&);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(ImageCtrl);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class ImgPathCtrl : using a wxString as the path to the Image
////
// This class will create a popup menu, allowing one to select an image through an OpenImageDialog
class ImgPathCtrl : public lkStaticImage
{
public:
	ImgPathCtrl();
	ImgPathCtrl(wxWindow*, const wxSize&, wxWindowID id = wxID_ANY, const wxValidator& validator = wxDefaultValidator);
	virtual ~ImgPathCtrl();

// Attributes
protected:
	wxString							m_sFullPath; // retrieved from the DB, or the filename when setting new image from disk
	lkView*								m_pView;
	wxString							m_ConfigSection;

private:
	enum class menu_ids
	{
		ID_New = wxID_HIGHEST + 1,
		ID_Clear
	};

public:
	void								SetView(lkView*);
	void								SetConfigSection(const wxString&);

// Implementation
private:
	void								Init(void);

public:
	virtual void						SetEmpty(bool bRefresh = true) wxOVERRIDE;
	// replaces current image with a File
	virtual void						SetImage(const wxString& strPath) wxOVERRIDE;

	wxString							GetImage(void) const; // returns m_sFullPath

protected:
	void								NewImage(void);

// Event(s)
protected:
	void								OnRightClick(wxMouseEvent&);
	void								OnMenu(wxCommandEvent&);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(ImgPathCtrl);
};


#endif // !__IMAGECTRL_H__
