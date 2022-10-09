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
#ifndef __MAIN_DOCMANAGER_H__
#define	__MAIN_DOCMANAGER_H__
#pragma once

#include <wx/docview.h>

class MainDocManager : public wxDocManager
{
public:
	MainDocManager();
	virtual ~MainDocManager();

	virtual wxFileHistory*						OnCreateFileHistory() wxOVERRIDE;

	// this one is used when menu-command ID_NEW or ID_OPEN is launched, creating a child-document is done somewhere else...
	virtual wxDocument*							CreateDocument(const wxString& path, long flags = 0) wxOVERRIDE;

	// Searches for any existing chil-docs, then activates that, otherwise will create a new child-doc instead
	wxDocument*									CreateOrActivateChildDocument(const wxClassInfo*);

	// this one searches for a matching lkDocTemplate and adds the given wxView to its internal ChildViews-list
//	virtual void								AddChildTemplate(lkDocTemplate*);


	// Find an existing document by classinfo, if the (main) document not yet is created, NULL is returned
	wxDocument*									FindDocumentByClassinfo(const wxClassInfo*);

	// Find template from view class info, may return NULL.
	wxDocTemplate*								FindTemplateByViewClass(const wxClassInfo* viewClassInfo);

	// Searches in the list of available views in document, if found will return that one, otherwise creates a new view
	virtual wxView*								CreateView(wxDocument* doc, long flags = 0) wxOVERRIDE;

	wxDocument*									GetMainDocument(const wxClassInfo* maindocClassInfo = NULL);

	/* ************************************************************************************************************
	 * Found a bug in wxWidgets :
	 * When there is a Document with child-docs in it, when closing it the parent-doc auto removes any
	 * child-docs. But those child-docs is a private member of wxDocument (so not accessible).
	 * In wxDocManager, all documents (parent and children) are stored in 1 list. So when calling CloseDocuments
	 * in wxDocManager, it is possible that it trys to delete a document which already has been removed by
	 * its parent, resulting in strange things (and memory-leaks).
	 * 
	 * I've fixed this by taking over the wxEVT_CLOSE_WINDOW event in a custom wxDocParentFrame so I call
	 * my own 'corrected' versions of CloseDocuments.. 
	 * Problem is, Clear, CloseDocuments and CloseDocument are not virtual in the baseclass
	 * which is very enoying
	 * ************************************************************************************************************
	 */
	// closes all currently open documents
	virtual bool								lkCloseDocuments(bool force = true);

	// closes the specified document
	virtual bool								lkCloseDocument(wxDocument* doc, bool force = false);

	// Clear remaining documents and templates
	virtual bool								lkClear(bool force = true);

// Handlers for common user commands
protected:
	void										OnViewsBase(wxCommandEvent& event);
	void										OnViewsCategory(wxCommandEvent& event);
	void										OnViewsCountry(wxCommandEvent& event);
	void										OnViewsGenres(wxCommandEvent& event);
	void										OnViewsJudgement(wxCommandEvent& event);
	void										OnViewsLocation(wxCommandEvent& event);
	void										OnViewsMedia(wxCommandEvent& event);
	void										OnViewsMovies(wxCommandEvent& event);
	void										OnViewsOrigine(wxCommandEvent& event);
	void										OnViewsQuality(wxCommandEvent& event);
	void										OnViewsRating(wxCommandEvent& event);
	void										OnViewsScreen(wxCommandEvent& event);
	void										OnViewsStorage(wxCommandEvent& event);

	void										OnUpdateCompact(wxUpdateUIEvent& event);

private:
	wxDocTemplate*								DoPromptFile(wxString& sPath, long flags = 0);

protected:
	wxDECLARE_EVENT_TABLE();

private:
	wxDECLARE_DYNAMIC_CLASS(MainDocManager);
	wxDECLARE_NO_COPY_CLASS(MainDocManager);
};

// -------------------------------------------------------------------------------------------------------------------

// use this class to create child-documents
class lkChildDocTemplate : public wxDocTemplate
{
public:
	// Associate document and view types. They're for identifying what view is
	// associated with what template/document type
	lkChildDocTemplate(wxDocManager* manager, const wxString& descr, const wxString& filter, const wxString& dir, const wxString& ext,
					   const wxString& docTypeName, const wxString& viewTypeName,
					   wxClassInfo* docClassInfo = NULL, wxClassInfo* viewClassInfo = NULL, long flags = wxDEFAULT_TEMPLATE_FLAGS);
	lkChildDocTemplate(wxDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);
	virtual ~lkChildDocTemplate();

private:
	wxDECLARE_CLASS(lkChildDocTemplate);
	wxDECLARE_NO_COPY_CLASS(lkChildDocTemplate);
};

#endif // !__MAIN_DOCMANAGER_H__
