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
/* *************************************************************************************
 * class lkImageBox : base (sub-)class for custom wxOwnerDrawCombo- and -ListBox
 * partially based at <wx/bmpcbox.h> and <wx/common/bmpcboxcmn.cpp>
 * ************************************************************************************* */
#ifndef __LKIMAGEBOX_H__
#define __LKIMAGEBOX_H__
#pragma once

#include <wx/defs.h>
#include <wx/window.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/clntdata.h>

#include "lkBoxInit.h"
#include "lkTools.h"

#if defined(__UNIX__)
typedef unsigned long   WXCOLORREF;
#endif

#ifndef lk_ICONSIZE
 // default width/height for a small icon
#define lk_ICONSIZE		16
#endif // !lk_ICONSIZE

#ifndef lkCX_BORDER
// Margins for images cx for left/right, cy for top/bottom
#define lkCX_BORDER		3
#define lkCY_BORDER		1
#define lkDEFAULT_ITEM_HEIGHT	(lk_ICONSIZE)

#endif // !lkCX_BORDER


class lkImageBox
{
public:
	lkImageBox();
	virtual ~lkImageBox();

// Attributes
protected:
	int							m_nImageCount; // amount of images to process in Populate
	int							m_nBestHeight; // CY_BORDER + max(image.height, charheight)

	wxColour					m_colSelBack, m_colSelFore; // custom back/fore Selected Colours (default = wxNullColour = Not Using)

private:
	lkBoxInitList*				m_initList; // will get destroyed after 'Populate' has been called

	wxSize						m_nImageSize; // cy: height, cx: width, default cx=cy=ICO_SIZE=16

	lkExpandFlags				m_ExpandAlign;
	bool						m_bFitScaled; // resizes the image (not icon) to best fit inside the forcing bounderies

	int							m_imgAreaWidth;  // Width of area next to text field (imgage.width + lkCX_BORDER)
	int							m_fontHeight;

	void						Init(void);
	void						KillInitList(void);

	wxColour					m_defcSelBack, m_defcSelFore;

public:
	void						SetForcedHeight(int cy);
	void						SetForcedWidth(int cx);

	void						SetImageExpand(lkExpandFlags);
	void						SetImgScaled(bool); // modifies 'm_bFitScaled'

	wxCoord						GetImageHeight(void) const;
	wxCoord						GetImageWidth(void) const;

	// Returns size of the image used in list
	wxSize						GetBitmapSize(void) const;

	int							GetAreaWidth(void) const;
	int							GetBestHeight(void) const;

	void						SetSelectedBackColour(WXCOLORREF);
	void						SetSelectedForeColour(WXCOLORREF);

public:
	// following (2) prototypes should be used when populating with images of different size

	// returns false if entered list was invalid (like NULL or not containing any 'lkBoxInit' objects)
	// a local copy of the entered list will be created having copies of contained classes in 'lkBoxInitList',
	// if 'calcWidth'==FALSE, 'expAlign' will be ignored
	// best width will be calculated if 'calcWidth'==TRUE (by auto-calling 'SetForcedWidth')
	// -- usually, you should call this prototype BEFORE Creation of the Control
	bool						InitializeFromList(lkBoxInitList*, bool calcWidth = false, lkExpandFlags expAlign = lkExpandFlags::EXPAND_Left);

	// Populates a lkImageList with the images calculated via 'InitializeFromList'
	// It will call 'lkImageList::CreateCustom' with the width/height calculated in 'InitializeFromList'
	// returns false in case of error or empty initList
	// lkBoxInitList will not be destroyed, while adding items into a controll can't be done via this class
	// Should be called in Populate-funcion of derived class, like 'wxOwnerDrawnComboBox' or such
	//
	// if selLparam!=wxNOT_FOUND, will select the item with it's lParam==selLparam
	bool						Populate();

protected:
	// must be overrided in derived class
	// on success must return index of appended string, 
	// will be called inside 'Populate'
	virtual int					AddItem(lkBoxInit*, wxBitmap*) = 0;

	// Returns pointer of ClientObject for selected item, to be overriden in the class that stores our internal lkImgClientData objects
	virtual wxClientData*		GetItemObject(unsigned int) const = 0;

	// Return pointer to the owner-drawn control
	virtual wxWindow*			GetControl(void) = 0;

	// Reset contents of the control, deleting all containing objects -- be sure if derived class overwrite this function, you also call lkImageBox::StartOver
	virtual void				StartOver(void);

	// this will update m_fontHeight, call only after control has been created
	virtual void				UpdateInternals(void);

	// Recalculates amount of empty space needed in front of text in control itself. Returns number that can be passed to
	// wxOwnerDrawnComboBox::SetCustomPaintWidth() and similar functions.
	virtual int					DetermineIndent(void);

	// Returns the image of the item with the given index.
	wxBitmap					GetItemBitmap(int n) const;

private:
	wxBitmap					Prepare(const wxImage&);	// prepares given image for addition/replacement -- returns copy of given image


protected:
	void						DrawBackground(wxDC& dc, const wxRect& rect, int item, int flags) const;
	void						DrawItem(wxDC& dc, const wxRect& rect, int item, const wxString& text, int flags) const;
	wxCoord						MeasureItem(size_t item) const;
};

#endif // !__LKIMAGEBOX_H__
