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
#ifndef __LK_STATICTEXT_H__
#define __LK_STATICTEXT_H__
#pragma once

#include <wx/stattext.h> // for the static definitions and othe basic's of wxStaticText
#include <wx/valgen.h> // for default Validator

// based at wxStaticTextGeneric/ Universal
class lkStaticText : public wxControl
{
public:
	lkStaticText();
	lkStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
				 long style = 0, const wxValidator& validator = wxDefaultValidator);
	virtual ~lkStaticText();

	bool						Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
									   long style = 0, const wxValidator& validator = wxDefaultValidator);

	// overridden base virtuals
	virtual bool				AcceptsFocus() const wxOVERRIDE;
	virtual bool				HasTransparentBackground() wxOVERRIDE;
	bool						IsEllipsized() const;

	// SetLabel first will remove any markup in the given label
	virtual void				SetLabel(const wxString& label) wxOVERRIDE;
	virtual bool				SetFont(const wxFont& font) wxOVERRIDE;

protected:
	// wrap the text of the control so that no line is longer than the given
	// width (if possible: this function won't break words)
	// This function will NOT modify the value returned by GetLabel(), instead, the modified label is stored inside m_label
	wxString					Wrap(const wxString&);

	// This funtion will return last visible line of a multiline label, according to control's clientsize, and the given label's textExtend -- called from GetEllipsizedLabel
	wxString					GetLastVisibleLine(const wxString&);

	virtual wxSize				DoGetBestClientSize() const wxOVERRIDE;
	virtual void				DoSetSize(int x, int y, int width, int height, int sizeFlags) wxOVERRIDE;

	// functions required for wxST_ELLIPSIZE_* support

	// choose the default border for this window
	virtual wxBorder			GetDefaultBorder() const wxOVERRIDE;

	// Calls Ellipsize() on the real label if necessary. Unlike GetLabelText(),
	// keeps the mnemonics instead of removing them.
	virtual wxString			GetEllipsizedLabel() const;

	// Replaces parts of the string with ellipsis according to the ellipsize
	// style. Shouldn't be called if we don't have any.
	wxString					Ellipsize(const wxString& label) const;


	// Note that even though ports with native support for ellipsization
	// (currently only wxGTK) don't need this stuff, we still need to define it
	// as it's used by wxGenericStaticText.

	// Must be called when the size or font changes to redo the ellipsization
	// for the new size. Calls WXSetVisibleLabel() to actually update the
	// display.
	void						UpdateLabel();

	// These functions are platform-specific and must be implemented in the
	// platform-specific code. They must not use or update m_labelOrig.

	// Returns the label currently displayed inside the control, with mnemonics
	// if any.
	virtual wxString			WXGetVisibleLabel() const;

	// Sets the real label currently displayed inside the control, _without_
	// invalidating the size. The text passed is always markup-free but may
	// contain the mnemonic characters.
	virtual void				WXSetVisibleLabel(const wxString& str);

	// Update the current size to match the best size unless wxST_NO_AUTORESIZE
	// style is explicitly used.
	void						AutoResizeIfNecessary();

protected:
	void						OnPaint(wxPaintEvent& event);
	void						DoDrawLabel(wxDC& dc, const wxRect& rect);

private:
	// These fields are only used if m_markupText == NULL.
	wxString					m_label;
	int							m_mnemonic;
	int							m_nLines; // if multiline, amount of lines of the label -- set inside Wrap

	void						Init(void);

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkStaticText);
};


class lkStaticTextValidator : public wxGenericValidator
{
public:
	lkStaticTextValidator(wxString*);
	lkStaticTextValidator(const lkStaticTextValidator&);
	virtual ~lkStaticTextValidator();

	virtual wxObject*			Clone() const wxOVERRIDE;

	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

private:
	wxDECLARE_CLASS(lkStaticTextValidator);
	wxDECLARE_NO_ASSIGN_CLASS(lkStaticTextValidator);
};

#endif // !__LK_STATICTEXT_H__
