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
#include "lkDocument.h"
#include "lkView.h"

wxIMPLEMENT_ABSTRACT_CLASS(lkDocument, wxDocument)

lkDocument::lkDocument(wxDocument* pParent) : wxDocument(pParent), m_ChildViews()
{
	m_ChildViews.DeleteContents(false); // we keep copies of the original view(s) which are maintained in any child-documents
}
lkDocument::~lkDocument()
{
}

bool lkDocument::OnNewDocument(const wxString& sPath)
{
	wxASSERT(!sPath.IsEmpty());

	SetDocumentSaved(false);

	SetTitle(wxFileNameFromPath(sPath));
	SetFilename(sPath, true);

	return true;
}

//virtual
void lkDocument::UpdateAllViews(wxView* sender, wxObject* hint)
{
	wxList::compatibility_iterator node = m_documentViews.GetFirst();
	while ( node )
	{
		wxView* view = (wxView*)node->GetData();
		if ( view != sender )
			view->OnUpdate(sender, hint);
		node = node->GetNext();
	}

	// now walk through any views of child-documents
	wxList::compatibility_iterator child = m_ChildViews.GetFirst();
	while ( child )
	{
		wxView* view = (wxView*)child->GetData();
		if ( view != sender )
			view->OnUpdate(sender, hint);
		child = child->GetNext();
	}
}

//virtual
void lkDocument::NotifyClosing()
{
	wxList::compatibility_iterator node = m_documentViews.GetFirst();
	while ( node )
	{
		wxView* view = (wxView*)node->GetData();
		view->OnClosingDocument();
		node = node->GetNext();
	}

	// now walk through any views of child-documents
	wxList::compatibility_iterator child = m_ChildViews.GetFirst();
	while ( child )
	{
		wxView* view = (wxView*)child->GetData();
		view->OnClosingDocument();
		child = child->GetNext();
	}
}

void lkDocument::AddChildView(wxView* view)
{
	if ( !view )
		return;

	if ( !m_ChildViews.Member(view) )
		m_ChildViews.Append(view);
}

//virtual
bool lkDocument::AddView(wxView* view)
{
	if ( !view )
		return false;

	if ( !m_documentViews.Member(view) )
		m_documentViews.Append(view);

	if ( IsChildDocument() )
	{
		// add this view also to the parent list of child-views
		if ( m_documentParent && m_documentParent->IsKindOf(wxCLASSINFO(lkDocument)) )
			((lkDocument*)m_documentParent)->AddChildView(view);
	}

	OnChangedViewList();
	return true;
}

void lkDocument::RemoveChildView(wxView* view)
{
	if ( !view )
		return;

	m_ChildViews.DeleteObject(view);
}

//virtual
bool lkDocument::RemoveView(wxView* view)
{
	if ( !view )
		return false;

	// keep the pointer, while our child-list only keep track of pointers, not what it contains
	wxView* child_view = view;

	if ( !m_documentViews.DeleteObject(view) )
		return false;

	if ( IsChildDocument() )
	{
		// remove this view also from the parent list of child-views
		if ( m_documentParent && m_documentParent->IsKindOf(wxCLASSINFO(lkDocument)) )
			((lkDocument*)m_documentParent)->RemoveChildView(child_view);
	}

	OnChangedViewList();
	return true;
}

wxView* lkDocument::FindViewByClass(const wxClassInfo* classInfo)
{
	if ( !classInfo )
		return NULL;

	wxList::compatibility_iterator node = GetViews().GetFirst();
	while ( node )
	{
		wxView* view = (wxView*)node->GetData();
		if ( view && view->IsKindOf(classInfo) )
			return view;
		node = node->GetNext();
	}

	// still not found? Walk through child-views
	wxList::compatibility_iterator child = m_ChildViews.GetFirst();
	while ( child )
	{
		wxView* view = (wxView*)child->GetData();
		if ( view && view->IsKindOf(classInfo) )
			return view;
		child = child->GetNext();
	}

	return NULL;
}

// true if safe to close
bool lkDocument::OnSaveModified()
{
	// first check if all views agree to be closed
	const wxList::iterator end = m_documentViews.end();
	for ( wxList::iterator i = m_documentViews.begin(); i != end; ++i )
	{
		wxView* view = (wxView*)*i;
		if ( view && view->IsKindOf(wxCLASSINFO(lkView)) )
			if ( !(dynamic_cast<lkView*>(view))->CanClose() || !(dynamic_cast<lkView*>(view))->UpdateData(true) )
				return false;
	}
	return true;
}

bool lkDocument::HasChildren()
{
	return !(m_ChildViews.IsEmpty());
}

