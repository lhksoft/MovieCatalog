/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __LKSTATICIMAGE_H__
#define __LKSTATICIMAGE_H__
#pragma once

#include <wx/control.h>
#include <wx/image.h>
#include <wx/pen.h>

#include "lkTools.h"


class lkStaticImage : public wxControl
{
public:
    lkStaticImage();
	lkStaticImage(wxWindow*, const wxSize&, wxWindowID id = wxID_ANY, const wxValidator& validator = wxDefaultValidator);
	virtual ~lkStaticImage();

	bool					Create(wxWindow*, const wxSize&, wxWindowID id = wxID_ANY, const wxValidator& validator = wxDefaultValidator);

// Overrides
public:
	virtual bool			HasTransparentBackground() wxOVERRIDE;

// Implementation
public:
	void					SetDrawMode(lkResizeOptions opt = lkResizeOptions::FIT_SCALED);

	virtual void			SetEmpty(bool bRefresh = true);
	bool					IsEmpty(void) const;
	// replaces current image with a File
	virtual void			SetImage(const wxString& strPath);
	// replaces current image from an Image
	virtual void			SetImage(const wxImage& iImage);

	wxImage					GetImage(void) const;

// Data Members
protected:
	wxImage					m_Image;
	lkResizeOptions			m_drawMode;
	wxPen					m_Pen; // used when no image is set

// Events
protected:
	void					OnPaint(wxPaintEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkStaticImage);
};


#endif // !__LKSTATICIMAGE_H__
