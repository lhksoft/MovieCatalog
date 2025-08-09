/* lkFileCtrl - custom FileCtrl with custom FileListCtrl
 * based at wxGenericFileCtrl
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
#include "lkFileCtrl.h"

#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/msgdlg.h>
// #include <wx/log.h>
#include <wx/filedlg.h>

// #include "wx/clntdata.h"
#include "wx/file.h"        // for wxS_IXXX constants only
#include "wx/dir.h"
// #include "wx/tokenzr.h"
// #include "wx/imaglist.h"

#if defined(__WINDOWS__)
#define IsTopMostDir(dir)   (dir.empty())
#else
#define IsTopMostDir(dir)   (dir == wxT("/"))
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void lkGenerateFilterChangedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd )
{
    wxFileCtrlEvent event( wxEVT_FILECTRL_FILTERCHANGED, wnd, wnd->GetId() );

    event.SetFilterIndex( fileCtrl->GetFilterIndex() );

    wnd->GetEventHandler()->ProcessEvent( event );
}

void lkGenerateFolderChangedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd )
{
    wxFileCtrlEvent event( wxEVT_FILECTRL_FOLDERCHANGED, wnd, wnd->GetId() );

    event.SetDirectory( fileCtrl->GetDirectory() );

    wnd->GetEventHandler()->ProcessEvent( event );
}

void lkGenerateSelectionChangedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd)
{
    wxFileCtrlEvent event( wxEVT_FILECTRL_SELECTIONCHANGED, wnd, wnd->GetId() );
    event.SetDirectory( fileCtrl->GetDirectory() );

    wxArrayString filenames;
    fileCtrl->GetFilenames( filenames );
    event.SetFiles( filenames );

    wnd->GetEventHandler()->ProcessEvent( event );
}

void lkGenerateFileActivatedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd, const wxString& filename = wxEmptyString );
void lkGenerateFileActivatedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd, const wxString& filename )
{
    wxFileCtrlEvent event( wxEVT_FILECTRL_FILEACTIVATED, wnd, wnd->GetId() );
    event.SetDirectory( fileCtrl->GetDirectory() );

    wxArrayString filenames;

    if ( filename.empty() )
    {
        fileCtrl->GetFilenames( filenames );
    }
    else
    {
        filenames.Add( filename );
    }

    event.SetFiles( filenames );

    wnd->GetEventHandler()->ProcessEvent( event );
}


static
int wxCALLBACK lkFileDataNameCompare( wxIntPtr data1, wxIntPtr data2, wxIntPtr sortOrder)
{
     wxFileData *fd1 = (wxFileData *)wxUIntToPtr(data1);
     wxFileData *fd2 = (wxFileData *)wxUIntToPtr(data2);

     if (fd1->GetFileName() == wxT(".."))
         return -sortOrder;
     if (fd2->GetFileName() == wxT(".."))
         return sortOrder;
     if (fd1->IsDir() && !fd2->IsDir())
         return -sortOrder;
     if (fd2->IsDir() && !fd1->IsDir())
         return sortOrder;

     return sortOrder*wxCmpNatural( fd1->GetFileName(), fd2->GetFileName() );
}

static
int wxCALLBACK lkFileDataSizeCompare(wxIntPtr data1, wxIntPtr data2, wxIntPtr sortOrder)
{ // unchanged from original
     wxFileData *fd1 = (wxFileData *)wxUIntToPtr(data1);
     wxFileData *fd2 = (wxFileData *)wxUIntToPtr(data2);

     if (fd1->GetFileName() == wxT(".."))
         return -sortOrder;
     if (fd2->GetFileName() == wxT(".."))
         return sortOrder;
     if (fd1->IsDir() && !fd2->IsDir())
         return -sortOrder;
     if (fd2->IsDir() && !fd1->IsDir())
         return sortOrder;
     if (fd1->IsLink() && !fd2->IsLink())
         return -sortOrder;
     if (fd2->IsLink() && !fd1->IsLink())
         return sortOrder;

     return fd1->GetSize() > fd2->GetSize() ? sortOrder : -sortOrder;
}

static
int wxCALLBACK lkFileDataTypeCompare(wxIntPtr data1, wxIntPtr data2, wxIntPtr sortOrder)
{
     wxFileData *fd1 = (wxFileData *)wxUIntToPtr(data1);
     wxFileData *fd2 = (wxFileData *)wxUIntToPtr(data2);

     if (fd1->GetFileName() == wxT(".."))
         return -sortOrder;
     if (fd2->GetFileName() == wxT(".."))
         return sortOrder;
     if (fd1->IsDir() && !fd2->IsDir())
         return -sortOrder;
     if (fd2->IsDir() && !fd1->IsDir())
         return sortOrder;
     if (fd1->IsLink() && !fd2->IsLink())
         return -sortOrder;
     if (fd2->IsLink() && !fd1->IsLink())
         return sortOrder;

     return sortOrder*wxStricmp( fd1->GetFileType(), fd2->GetFileType() );
}

static
int wxCALLBACK lkFileDataTimeCompare(wxIntPtr data1, wxIntPtr data2, wxIntPtr sortOrder)
{ // unchanged
     wxFileData *fd1 = (wxFileData *)wxUIntToPtr(data1);
     wxFileData *fd2 = (wxFileData *)wxUIntToPtr(data2);

     if (fd1->GetFileName() == wxT(".."))
         return -sortOrder;
     if (fd2->GetFileName() == wxT(".."))
         return sortOrder;
     if (fd1->IsDir() && !fd2->IsDir())
         return -sortOrder;
     if (fd2->IsDir() && !fd1->IsDir())
         return sortOrder;

     return fd1->GetDateTime().IsLaterThan(fd2->GetDateTime()) ? sortOrder : -sortOrder;
}

//-----------------------------------------------------------------------------
//  lkFileListCtrl
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(lkFileListCtrl,wxFileListCtrl);

lkFileListCtrl::lkFileListCtrl() : wxFileListCtrl()
{
}

lkFileListCtrl::lkFileListCtrl( wxWindow *win,
                                wxWindowID id,
                                const wxString& wild,
                                bool showHidden,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxValidator &validator,
                                const wxString &name )
            : wxFileListCtrl(win, id, wild, showHidden, pos, size, style, validator, name)
{
}

lkFileListCtrl::~lkFileListCtrl()
{
}

//virtual
void lkFileListCtrl::SortItems(wxFileData::fileListFieldType field, bool forward)
{
    m_sort_field = field;
    m_sort_forward = forward;
    const long sort_dir = forward ? 1 : -1;

    switch (m_sort_field)
    {
        case wxFileData::FileList_Size :
            wxListCtrl::SortItems(lkFileDataSizeCompare, sort_dir);
            break;

        case wxFileData::FileList_Type :
            wxListCtrl::SortItems(lkFileDataTypeCompare, sort_dir);
            break;

        case wxFileData::FileList_Time :
            wxListCtrl::SortItems(lkFileDataTimeCompare, sort_dir);
            break;

        case wxFileData::FileList_Name :
        default :
            wxListCtrl::SortItems(lkFileDataNameCompare, sort_dir);
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////
// lkFileCtrl implementation
///////////////////////////////////////////////////////////////////////////////

#define  ID_CHOICE        (wxID_FILECTRL + 1)
#define  ID_TEXT          (wxID_FILECTRL + 2)
#define  ID_FILELIST_CTRL (wxID_FILECTRL + 3)
#define  ID_CHECK         (wxID_FILECTRL + 4)

wxIMPLEMENT_DYNAMIC_CLASS( lkFileCtrl, wxNavigationEnabled<wxControl> );

wxBEGIN_EVENT_TABLE( lkFileCtrl, wxNavigationEnabled<wxControl> )
    EVT_LIST_ITEM_SELECTED(  ID_FILELIST_CTRL, lkFileCtrl::OnSelected )
    EVT_LIST_ITEM_ACTIVATED( ID_FILELIST_CTRL, lkFileCtrl::OnActivated )
    EVT_CHOICE(              ID_CHOICE,        lkFileCtrl::OnChoiceFilter )
    EVT_TEXT_ENTER(          ID_TEXT,          lkFileCtrl::OnTextEnter )
    EVT_TEXT(                ID_TEXT,          lkFileCtrl::OnTextChange )
    EVT_CHECKBOX(            ID_CHECK,         lkFileCtrl::OnCheck )
wxEND_EVENT_TABLE()

bool lkFileCtrl::Create( wxWindow *parent,
                         wxWindowID id,
                         const wxString& defaultDirectory,
                         const wxString& defaultFileName,
                         const wxString& wildCard,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name )
{
    this->m_style = style;
    m_inSelected = false;
    m_noSelChgEvent = false;
    m_check = NULL;

    // check that the styles are not contradictory
    wxASSERT_MSG( !( ( m_style & wxFC_SAVE ) && ( m_style & wxFC_OPEN ) ),
                  wxT( "can't specify both wxFC_SAVE and wxFC_OPEN at once" ) );

    wxASSERT_MSG( !( ( m_style & wxFC_SAVE ) && ( m_style & wxFC_MULTIPLE ) ),
                  wxT( "wxFC_MULTIPLE can't be used with wxFC_SAVE" ) );

    wxNavigationEnabled<wxControl>::Create( parent, id,
                                            pos, size,
                                            wxTAB_TRAVERSAL,
                                            wxDefaultValidator,
                                            name );

    m_dir = defaultDirectory;

    m_ignoreChanges = true;

    if ( ( m_dir.empty() ) || ( m_dir == wxT( "." ) ) )
    {
        m_dir = wxGetCwd();
        if ( m_dir.empty() )
            m_dir = wxFILE_SEP_PATH;
    }

    const size_t len = m_dir.length();
    if ( ( len > 1 ) && ( wxEndsWithPathSeparator( m_dir ) ) )
        m_dir.Remove( len - 1, 1 );

    m_filterExtension.clear();

    // layout

    const bool is_pda = ( wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA );

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *staticsizer = new wxBoxSizer( wxHORIZONTAL );
    if ( is_pda )
        staticsizer->Add( new wxStaticText( this, wxID_ANY, _( "Current directory:" ) ),
                          wxSizerFlags().DoubleBorder(wxRIGHT) );
    m_static = new wxStaticText( this, wxID_ANY, m_dir );
    staticsizer->Add( m_static, 1 );
    mainsizer->Add( staticsizer, wxSizerFlags().Expand().Border());

    long style2 = wxLC_LIST;
    if ( !( m_style & wxFC_MULTIPLE ) )
        style2 |= wxLC_SINGLE_SEL;

    style2 |= wxSUNKEN_BORDER;

    m_list = new lkFileListCtrl( this, ID_FILELIST_CTRL,
                                 wxEmptyString, false,
                                 wxDefaultPosition, wxSize( 400, 140 ),
                                 style2 );

    m_text = new wxTextCtrl( this, ID_TEXT, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_PROCESS_ENTER );
    m_choice = new wxChoice( this, ID_CHOICE );

    if ( is_pda )
    {
        // PDAs have a different screen layout
        mainsizer->Add( m_list, wxSizerFlags( 1 ).Expand().HorzBorder() );

        wxBoxSizer *textsizer = new wxBoxSizer( wxHORIZONTAL );
        textsizer->Add( m_text, wxSizerFlags( 1 ).Centre().Border() );
        textsizer->Add( m_choice, wxSizerFlags( 1 ).Centre().Border() );
        mainsizer->Add( textsizer, wxSizerFlags().Expand() );

    }
    else // !is_pda
    {
        mainsizer->Add( m_list, wxSizerFlags( 1 ).Expand().Border() );
        mainsizer->Add( m_text, wxSizerFlags().Expand().Border() );

        wxBoxSizer *choicesizer = new wxBoxSizer( wxHORIZONTAL );
        choicesizer->Add( m_choice, wxSizerFlags( 1 ).Centre() );

        if ( !( m_style & wxFC_NOSHOWHIDDEN ) )
        {
            m_check = new wxCheckBox( this, ID_CHECK, _( "Show &hidden files" ) );
            choicesizer->Add( m_check, wxSizerFlags().Centre().DoubleBorder(wxLEFT) );
        }

        mainsizer->Add( choicesizer, wxSizerFlags().Expand().Border() );
    }

    SetWildcard( wildCard );

    SetSizer( mainsizer );

    if ( !is_pda )
    {
        mainsizer->Fit( this );
    }

    m_list->GoToDir( m_dir );
    UpdateControls();
	m_text->SetValue(defaultFileName);

    m_ignoreChanges = false;

    // must be after m_ignoreChanges = false
    SetFilename( defaultFileName );

    return true;
}

wxString lkFileCtrl::GetPath() const
{
    wxASSERT_MSG ( !(m_style & wxFC_MULTIPLE), "use GetPaths() instead" );

    return DoGetFileName().GetFullPath();
}

wxString lkFileCtrl::GetFilename() const
{
    wxASSERT_MSG ( !(m_style & wxFC_MULTIPLE), "use GetFilenames() instead" );

    return DoGetFileName().GetFullName();
}

wxString lkFileCtrl::GetDirectory() const
{
    // don't check for wxFC_MULTIPLE here, this one is probably safe to call in
    // any case as it can be always taken to mean "current directory"
    return DoGetFileName().GetPath();
}

wxFileName lkFileCtrl::DoGetFileName() const
{
    wxFileName fn;

    wxString value = m_text->GetValue();
    if ( value.empty() )
    {
        // nothing in the text control, get the selected file from the list
        wxListItem item;
        item.m_itemId = m_list->GetNextItem(-1, wxLIST_NEXT_ALL,
                                            wxLIST_STATE_SELECTED);

        // ... if anything is selected in the list
        if ( item.m_itemId != wxNOT_FOUND )
        {
            m_list->GetItem(item);

            fn.Assign(m_list->GetDir(), item.m_text);
        }
    }
    else // user entered the value
    {
        // the path can be either absolute or relative
        fn.Assign(value);
        if ( fn.IsRelative() )
            fn.MakeAbsolute(m_list->GetDir());
    }

    return fn;
}

void lkFileCtrl::DoGetFilenames(wxArrayString& filenames, bool fullPath) const
{
    filenames.clear();

    const wxString dir = m_list->GetDir();

    const wxString value = m_text->GetValue();
    if ( !value.empty() )
    {
        wxFileName fn(value);
        if ( fn.IsRelative() )
            fn.MakeAbsolute(dir);

        filenames.push_back(fullPath ? fn.GetFullPath() : fn.GetFullName());
        return;
    }

    const int numSel = m_list->GetSelectedItemCount();
    if ( !numSel )
        return;

    filenames.reserve(numSel);

    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT;
    item.m_itemId = -1;
    for ( ;; )
    {
        item.m_itemId = m_list->GetNextItem(item.m_itemId, wxLIST_NEXT_ALL,
                                            wxLIST_STATE_SELECTED);

        if ( item.m_itemId == -1 )
            break;

        m_list->GetItem(item);

        const wxFileName fn(dir, item.m_text);
        filenames.push_back(fullPath ? fn.GetFullPath() : fn.GetFullName());
    }
}

bool lkFileCtrl::SetDirectory( const wxString& dir )
{
    m_ignoreChanges = true;
    m_list->GoToDir( dir );
    UpdateControls();
    m_ignoreChanges = false;

    return wxFileName( dir ).SameAs( m_list->GetDir() );
}

bool lkFileCtrl::SetFilename( const wxString& name )
{
    wxString dir, fn, ext;
    wxFileName::SplitPath(name, &dir, &fn, &ext);
    wxCHECK_MSG( dir.empty(), false,
                 wxS( "can't specify directory component to SetFilename" ) );

	m_noSelChgEvent = false;
	//    m_noSelChgEvent = true;

    m_text->ChangeValue( name );

    // Deselect previously selected items
    {
        const int numSelectedItems = m_list->GetSelectedItemCount();

        if ( numSelectedItems > 0 )
        {
            long itemIndex = -1;

            for ( ;; )
            {
                itemIndex = m_list->GetNextItem( itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
                if ( itemIndex == wxNOT_FOUND )
                    break;

                m_list->SetItemState( itemIndex, 0, wxLIST_STATE_SELECTED );
            }
        }
    }

    // Select new filename if it's in the list
    long item = m_list->FindItem(wxNOT_FOUND, name);

    if ( item != wxNOT_FOUND )
    {
        m_list->SetItemState( item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
        m_list->EnsureVisible( item );
    }

//    m_noSelChgEvent = false;

    return true;
}

void lkFileCtrl::DoSetFilterIndex( int filterindex )
{
    wxClientData *pcd = m_choice->GetClientObject( filterindex );
    if ( !pcd )
        return;

    const wxString& str = ((static_cast<wxStringClientData *>(pcd))->GetData());
    m_list->SetWild( str );
    m_filterIndex = filterindex;
    if ( str.Left( 2 ) == wxT( "*." ) )
    {
        m_filterExtension = str.Mid( 1 );
        if ( m_filterExtension == wxT( ".*" ) )
            m_filterExtension.clear();
    }
    else
    {
        m_filterExtension.clear();
    }

    lkGenerateFilterChangedEvent( this, this );
}

void lkFileCtrl::SetWildcard( const wxString& wildCard )
{
    if ( wildCard.empty() || wildCard == wxFileSelectorDefaultWildcardStr )
    {
        m_wildCard = wxString::Format( _( "All files (%s)|%s" ),
                                       wxFileSelectorDefaultWildcardStr,
                                       wxFileSelectorDefaultWildcardStr );
    }
    else
        m_wildCard = wildCard;

    wxArrayString wildDescriptions, wildFilters;
    const size_t count = wxParseCommonDialogsFilter( m_wildCard,
                         wildDescriptions,
                         wildFilters );
    wxCHECK_RET( count, wxT( "wxFileDialog: bad wildcard string" ) );

    m_choice->Clear();

    for ( size_t n = 0; n < count; n++ )
    {
        m_choice->Append(wildDescriptions[n], new wxStringClientData(wildFilters[n]));
    }

    SetFilterIndex( 0 );
}

void lkFileCtrl::SetFilterIndex( int filterindex )
{
    m_choice->SetSelection( filterindex );

    DoSetFilterIndex( filterindex );
}

void lkFileCtrl::OnChoiceFilter( wxCommandEvent &event )
{
    DoSetFilterIndex( ( int )event.GetInt() );
}

void lkFileCtrl::OnCheck( wxCommandEvent &event )
{
    m_list->ShowHidden( event.GetInt() != 0 );
}

void lkFileCtrl::OnActivated( wxListEvent &event )
{
    HandleAction( event.m_item.m_text );
}

void lkFileCtrl::OnTextEnter( wxCommandEvent &WXUNUSED( event ) )
{
    HandleAction( m_text->GetValue() );
}

void lkFileCtrl::OnTextChange( wxCommandEvent &WXUNUSED( event ) )
{
    if ( !m_ignoreChanges )
    {
        // Clear selections.  Otherwise when the user types in a value they may
        // not get the file whose name they typed.
        if ( m_list->GetSelectedItemCount() > 0 )
        {
            long item = m_list->GetNextItem( -1, wxLIST_NEXT_ALL,
                                             wxLIST_STATE_SELECTED );
            while ( item != -1 )
            {
                m_list->SetItemState( item, 0, wxLIST_STATE_SELECTED );
                item = m_list->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
            }
        }
    }
}

void lkFileCtrl::OnSelected( wxListEvent &event )
{
    if ( m_ignoreChanges )
        return;

    if ( m_inSelected )
        return;

    m_inSelected = true;
    const wxString filename( event.m_item.m_text );

    if ( filename == wxT( ".." ) )
    {
        m_inSelected = false;
        return;
    }

    wxString dir = m_list->GetDir();
    if ( !IsTopMostDir( dir ) )
        dir += wxFILE_SEP_PATH;
    dir += filename;
    if ( wxDirExists( dir ) )
    {
        m_inSelected = false;

        return;
    }


    m_ignoreChanges = true;
    m_text->SetValue( filename );

    if ( m_list->GetSelectedItemCount() > 1 )
    {
        m_text->Clear();
    }

    if ( !m_noSelChgEvent )
        lkGenerateSelectionChangedEvent( this, this );

    m_ignoreChanges = false;
    m_inSelected = false;
}

void lkFileCtrl::HandleAction( const wxString &fn )
{
    if ( m_ignoreChanges )
        return;

    wxString filename( fn );
    if ( filename.empty() )
    {
        return;
    }
    if ( filename == wxT( "." ) ) return;

    wxString dir = m_list->GetDir();

    // "some/place/" means they want to chdir not try to load "place"
    const bool want_dir = filename.Last() == wxFILE_SEP_PATH;
    if ( want_dir )
        filename = filename.RemoveLast();

    if ( filename == wxT( ".." ) )
    {
        m_ignoreChanges = true;
        m_list->GoToParentDir();

        lkGenerateFolderChangedEvent( this, this );

        UpdateControls();
        m_ignoreChanges = false;
        return;
    }

#ifdef __UNIX__
    if ( filename == wxT( "~" ) )
    {
        m_ignoreChanges = true;
        m_list->GoToHomeDir();

        lkGenerateFolderChangedEvent( this, this );

        UpdateControls();
        m_ignoreChanges = false;
        return;
    }

    if ( filename.BeforeFirst( wxT( '/' ) ) == wxT( "~" ) )
    {
        filename = wxString( wxGetUserHome() ) + filename.Remove( 0, 1 );
    }
#endif // __UNIX__

    if ( !( m_style & wxFC_SAVE ) )
    {
        if ( ( filename.Find( wxT( '*' ) ) != wxNOT_FOUND ) ||
                ( filename.Find( wxT( '?' ) ) != wxNOT_FOUND ) )
        {
            if ( filename.Find( wxFILE_SEP_PATH ) != wxNOT_FOUND )
            {
                wxMessageBox( _( "Illegal file specification." ),
                              _( "Error" ), wxOK | wxICON_ERROR, this );
                return;
            }
            m_list->SetWild( filename );
            return;
        }
    }

    if ( !IsTopMostDir( dir ) )
        dir += wxFILE_SEP_PATH;
    if ( !wxIsAbsolutePath( filename ) )
    {
        dir += filename;
        filename = dir;
    }

    if ( wxDirExists( filename ) )
    {
        m_ignoreChanges = true;
        m_list->GoToDir( filename );
        UpdateControls();

        lkGenerateFolderChangedEvent( this, this );

        m_ignoreChanges = false;
        return;
    }

    // they really wanted a dir, but it doesn't exist
    if ( want_dir )
    {
        wxMessageBox( _( "Directory doesn't exist." ), _( "Error" ),
                      wxOK | wxICON_ERROR, this );
        return;
    }

    // append the default extension to the filename if it doesn't have any
    //
    // VZ: the logic of testing for !wxFileExists() only for the open file
    //     dialog is not entirely clear to me, why don't we allow saving to a
    //     file without extension as well?
    if ( !( m_style & wxFC_OPEN ) || !wxFileExists( filename ) )
    {
        filename = wxFileDialogBase::AppendExtension( filename, m_filterExtension );
        lkGenerateFileActivatedEvent( this, this, wxFileName( filename ).GetFullName() );
        return;
    }

    lkGenerateFileActivatedEvent( this, this );
}

bool lkFileCtrl::SetPath( const wxString& path )
{
    wxString dir, fn, ext;
    wxFileName::SplitPath(path, &dir, &fn, &ext);

    if ( !dir.empty() && !wxFileName::DirExists(dir) )
        return false;

    m_dir = dir;
    m_fileName = fn;
    if ( !ext.empty() || path.Last() == '.' )
    {
        m_fileName += wxT( "." );
        m_fileName += ext;
    }

    SetDirectory( m_dir );
    SetFilename( m_fileName );

    return true;
}

void lkFileCtrl::GetPaths( wxArrayString& paths ) const
{
    DoGetFilenames( paths, true );
}

void lkFileCtrl::GetFilenames( wxArrayString& files ) const
{
    DoGetFilenames( files, false );
}

void lkFileCtrl::UpdateControls()
{
    const wxString dir = m_list->GetDir();
    m_static->SetLabel( dir );
}

void lkFileCtrl::GoToParentDir()
{
    m_list->GoToParentDir();
    UpdateControls();
}

void lkFileCtrl::GoToHomeDir()
{
    m_list->GoToHomeDir();
    UpdateControls();
}

