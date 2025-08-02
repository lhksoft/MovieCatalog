/* lkFileCtrl
 * wxWidgets 3.2.4
 * Sorting mismatch, 
 * nearly all must be rewritten to have an optimized sort routine
 * see https://groups.google.com/g/wx-dev/c/ThrK06O9Fqc?pli=1
 * -- dummer --
 * 
 * Copyright (C) 2025 Laurens Koehoorn (lhksoft)
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
#ifndef __LK_FILECTRL_H__
#define __LK_FILECTRL_H__
#pragma once

#include <wx/filectrl.h>
#include <wx/generic/filectrlg.h>

//-----------------------------------------------------------------------------
//  lkFileListCtrl
//-----------------------------------------------------------------------------

class lkFileListCtrl : public wxFileListCtrl
{
public:
    lkFileListCtrl();
    lkFileListCtrl( wxWindow *win,
                wxWindowID id,
                const wxString &wild,
                bool showHidden,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxLC_LIST,
                const wxValidator &validator = wxDefaultValidator,
                const wxString &name = wxT("filelist") );
    virtual ~lkFileListCtrl();

    virtual void SortItems(wxFileData::fileListFieldType field, bool forward) wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(lkFileListCtrl);
};

//-----------------------------------------------------------------------------
//  lkFileCtrl
//-----------------------------------------------------------------------------

class lkFileCtrl : public wxNavigationEnabled<wxControl>, public wxFileCtrlBase
{
public:
    lkFileCtrl()
    {
        m_ignoreChanges = false;
    }

    lkFileCtrl( wxWindow *parent,
                wxWindowID id,
                const wxString& defaultDirectory = wxEmptyString,
                const wxString& defaultFilename = wxEmptyString,
                const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                long style = wxFC_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxFileCtrlNameStr) )
    {
        m_ignoreChanges = false;
        Create(parent, id, defaultDirectory, defaultFilename, wildCard,
               style, pos, size, name );
    }

    virtual ~lkFileCtrl() {}

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& defaultDirectory = wxEmptyString,
                 const wxString& defaultFileName = wxEmptyString,
                 const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 long style = wxFC_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 const wxString& name = wxASCII_STR(wxFileCtrlNameStr) );

    virtual void SetWildcard( const wxString& wildCard ) wxOVERRIDE;
    virtual void SetFilterIndex( int filterindex ) wxOVERRIDE;
    virtual bool SetDirectory( const wxString& dir ) wxOVERRIDE;

    // Selects a certain file.
    // In case the filename specified isn't found/couldn't be shown with
    // currently selected filter, false is returned and nothing happens
    virtual bool SetFilename( const wxString& name ) wxOVERRIDE;

    // Changes to a certain directory and selects a certain file.
    // In case the filename specified isn't found/couldn't be shown with
    // currently selected filter, false is returned and if directory exists
    // it's chdir'ed to
    virtual bool SetPath( const wxString& path ) wxOVERRIDE;

    virtual wxString GetFilename() const wxOVERRIDE;
    virtual wxString GetDirectory() const wxOVERRIDE;
    virtual wxString GetWildcard() const wxOVERRIDE { return this->m_wildCard; }
    virtual wxString GetPath() const wxOVERRIDE;
    virtual void GetPaths( wxArrayString& paths ) const wxOVERRIDE;
    virtual void GetFilenames( wxArrayString& files ) const wxOVERRIDE;
    virtual int GetFilterIndex() const wxOVERRIDE { return m_filterIndex; }

    virtual bool HasMultipleFileSelection() const wxOVERRIDE
        { return HasFlag(wxFC_MULTIPLE); }
    virtual void ShowHidden(bool show) wxOVERRIDE { m_list->ShowHidden( show ); }

    void GoToParentDir();
    void GoToHomeDir();

    // get the directory currently shown in the control: this can be different
    // from GetDirectory() if the user entered a full path (with a path other
    // than the one currently shown in the control) in the text control
    // manually
    wxString GetShownDirectory() const { return m_list->GetDir(); }

    wxFileListCtrl *GetFileList() { return m_list; }

    void ChangeToReportMode() { m_list->ChangeToReportMode(); }
    void ChangeToListMode() { m_list->ChangeToListMode(); }

protected:
    void OnChoiceFilter( wxCommandEvent &event );
    void OnCheck( wxCommandEvent &event );
    void OnActivated( wxListEvent &event );
    void OnTextEnter( wxCommandEvent &WXUNUSED( event ) );
    void OnTextChange( wxCommandEvent &WXUNUSED( event ) );
    void OnSelected( wxListEvent &event );
    void HandleAction( const wxString &fn );

    void DoSetFilterIndex( int filterindex );
    void UpdateControls();

    // the first of these methods can only be used for the controls with single
    // selection (i.e. without wxFC_MULTIPLE style), the second one in any case
    wxFileName DoGetFileName() const;
    void DoGetFilenames( wxArrayString& filenames, bool fullPath ) const;

    int m_style;

    wxString         m_filterExtension;
    wxChoice        *m_choice;
    wxTextCtrl      *m_text;
    lkFileListCtrl  *m_list;
    wxCheckBox      *m_check;
    wxStaticText    *m_static;

    wxString        m_dir;
    wxString        m_fileName;
    wxString        m_wildCard; // wild card in one string as we got it

    int             m_filterIndex;
    bool            m_inSelected;
    bool            m_ignoreChanges;
    bool            m_noSelChgEvent; // suppress selection changed events.

private:
    wxDECLARE_DYNAMIC_CLASS(lkFileCtrl);
    wxDECLARE_EVENT_TABLE();
};

#endif // __LK_FILECTRL_H__