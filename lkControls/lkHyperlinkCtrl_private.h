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
#ifndef __LK_HYPERLINK_CTRL_PRIVATE_H__
#define __LK_HYPERLINK_CTRL_PRIVATE_H__
#pragma once

#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
// lkHyperlinkDialog : used for Add/Replace from a Label/URL
///////////////////////////////////////////////////////////////////////////////

class lkHyperlinkDialog : public wxDialog
{
public:
	lkHyperlinkDialog();
	virtual ~lkHyperlinkDialog();

	// Create NOT called from C'tor !!
	bool			Create(wxWindow*);

	wxString		GetURL(void) const;
	void			SetURL(const wxString&);

	wxString		GetUriLabel(void) const;
	void			SetUriLabel(const wxString&);

protected:
	void			OnRightClick(wxMouseEvent& event);

	// Attributes
protected:
	wxString		m_URL, m_UriLabel;

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkHyperlinkDialog);
};


#endif // !__LK_HYPERLINK_CTRL_PRIVATE_H__

