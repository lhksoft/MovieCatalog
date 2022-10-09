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
#ifndef __LK_IMAGELIST_H__
#define __LK_IMAGELIST_H__
#pragma once

#include <wx/imaglist.h>
#include <wx/image.h>

#include "lkTools.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkImageList
////
class lkImageList : public wxImageList
{
public:
	lkImageList();
	lkImageList(int width, int height, lkExpandFlags ExpandAlign=lkExpandFlags::EXPAND_Center, bool bFitScaled=true);
	virtual ~lkImageList();

	bool								Create(int width, int height, bool mask = true, int initialCount = 1);

	// Add image
	//////////////////////////////
	int									Add(const wxImage&); // Adds given lkImage as new image to the list, will get the transparancy colour dynamicly (if any), resizes image if required
	int									Add(const wxString&); // Adds an image based on wxImage from a File

	// Replace image
	//////////////////////////////
	bool								Replace(int index, const wxImage&); // same applies as with 'Add'
	bool								Replace(int index, const wxString&);

protected:
	wxBitmap							Prepare(const wxImage&); // the image will be resized if required and the mask (if transparant) will be set

// Attributes
protected:
	wxSize								m_ForceSize; // this will be taken from c'tor or create

	lkExpandFlags						m_ExpandAlign;
	bool								m_bFitScaled; // resizes the image (not icon) to best fit inside the forcing bounderies

	bool								m_bUseMask;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkImageList);
};
#endif // !__LK_IMAGELIST_H__
