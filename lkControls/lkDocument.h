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
#ifndef __LK_DOCUMENT_H__
#define __LK_DOCUMENT_H__
#pragma once

#include <wx/docview.h>

class lkDocument : public wxDocument
{
public:
	lkDocument(wxDocument* pParent = NULL);
	virtual ~lkDocument();

	virtual bool					OnNewDocument(const wxString& sPath);

	virtual void					UpdateAllViews(wxView* sender = NULL, wxObject* hint = NULL) wxOVERRIDE;
	virtual void					NotifyClosing() wxOVERRIDE;

	void							AddChildView(wxView*);
	virtual bool					AddView(wxView* view) wxOVERRIDE;

	void							RemoveChildView(wxView*);
	virtual bool					RemoveView(wxView* view) wxOVERRIDE;

	wxView*							FindViewByClass(const wxClassInfo*);

	// UpdateData(true) will be called on the active view, if that somehow fails, OnSaveModified will return false
	virtual bool					OnSaveModified() wxOVERRIDE;

	bool							HasChildren(void); // whether there are any open child-views

protected:
	// while we have no access to the child-documents in wxWidgets (private), we must keep a copy of any child-document views to have UpdateAllViews keep working
	wxList							m_ChildViews;

private:
	wxDECLARE_ABSTRACT_CLASS(lkDocument);
	wxDECLARE_NO_COPY_CLASS(lkDocument);
};

#endif // !__LK_DOCUMENT_H__
