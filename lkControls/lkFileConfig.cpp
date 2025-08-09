///////////////////////////////////////////////////////////////////////////////
// Name:        lkFileConfig.cpp
// Purpose:     implementation of wxFileConfig derivation of wxConfig
// Author:      Laurens Koehoorn (original wxFileConfig by Vadim Zeitlin
// Modified by:
// Created:     19.06.22 (adapted from wx/fileconf.cpp)
// Original:    07.04.98 (adapted from appconf.cpp)
// Copyright:   (c) 1997 Karsten Ballueder  &  Vadim Zeitlin
//                       Ballueder@usa.net     <zeitlin@dptmaths.ens-cachan.fr>
//              (c) 2022 Laurens Koehoorn
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
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
#include "lkFileConfig.h"
#include "lkFileConfig_private.h"

#include <wx/string.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/app.h>
#include <wx/utils.h>    // for wxGetHomeDir
#include <wx/stream.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/memtext.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/filefn.h>
#include <wx/base64.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>

#if defined(__WINDOWS__)
#include <wx/msw/private.h>
#endif  //windows.h

#include <stdlib.h>
#include <ctype.h>


wxIMPLEMENT_ABSTRACT_CLASS(lkFileConfig, wxConfigBase);

// constructor supports creation of wxFileConfig objects of any type
lkFileConfig::lkFileConfig(const wxString& appName, const wxString& vendorName, const wxString& strLocal, const wxString& strGlobal,
						   long style, const wxMBConv& conv) : wxConfigBase((!appName && wxTheApp) ? wxTheApp->GetAppName() : appName,
																			vendorName, strLocal, strGlobal, style),
	m_fnLocalFile(strLocal), m_fnGlobalFile(strGlobal), m_conv(conv.Clone())
{
	// Make up names for files if empty
	if ( !m_fnLocalFile.IsOk() && (style & wxCONFIG_USE_LOCAL_FILE) )
		m_fnLocalFile = GetLocalFile(GetAppName(), style);

	if ( !m_fnGlobalFile.IsOk() && (style & wxCONFIG_USE_GLOBAL_FILE) )
		m_fnGlobalFile = GetGlobalFile(GetAppName());

	// Check if styles are not supplied, but filenames are, in which case add the correct styles.
	if ( m_fnLocalFile.IsOk() )
		SetStyle(GetStyle() | wxCONFIG_USE_LOCAL_FILE);

	if ( m_fnGlobalFile.IsOk() )
		SetStyle(GetStyle() | wxCONFIG_USE_GLOBAL_FILE);

	// if the path is not absolute, prepend the standard directory to it unless explicitly asked not to
	if ( !(style & wxCONFIG_USE_RELATIVE_PATH) )
	{
		if ( m_fnLocalFile.IsOk() )
			m_fnLocalFile.MakeAbsolute(GetLocalDir(style));

		if ( m_fnGlobalFile.IsOk() )
			m_fnGlobalFile.MakeAbsolute(GetGlobalDir());
	}

	SetUmask(-1);

	Init();
}

lkFileConfig::lkFileConfig(wxInputStream& inStream, const wxMBConv& conv) : m_conv(conv.Clone())
{
	m_isDirty = false;
	m_autosave = true;

	// always local_file when this constructor is called (?)
	SetStyle(GetStyle() | wxCONFIG_USE_LOCAL_FILE);

	m_pCurrentGroup = m_pRootGroup = new lkFileConfigGroup(NULL, wxEmptyString, this);

	m_linesHead = m_linesTail = NULL;

	// read the entire stream contents in memory
	wxWxCharBuffer cbuf;
	static const size_t chunkLen = 1024;

	wxMemoryBuffer buf(chunkLen);
	do
	{
		inStream.Read(buf.GetAppendBuf(chunkLen), chunkLen);
		buf.UngetAppendBuf(inStream.LastRead());

		const wxStreamError err = inStream.GetLastError();

		if ( err != wxSTREAM_NO_ERROR && err != wxSTREAM_EOF )
		{
			wxLogError(wxT("Error reading config options."));
			break;
		}
	}
	while ( !inStream.Eof() );

#if wxUSE_UNICODE
	size_t len;
	cbuf = conv.cMB2WC((char*)buf.GetData(), buf.GetDataLen() + 1, &len);
	if ( !len && buf.GetDataLen() )
	{
		wxLogError(wxT("Failed to read config options."));
	}
#else // !wxUSE_UNICODE
	// no need for conversion
	cbuf = wxCharBuffer::CreateNonOwned((char*)buf.GetData(), buf.GetDataLen());
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

	// parse the input contents if there is anything to parse
	if ( cbuf )
	{
		// now break it into lines
		wxMemoryText memText;
		for ( const wxChar* s = cbuf; ; ++s )
		{
			const wxChar* e = s;
			while ( *e != '\0' && *e != '\n' && *e != '\r' )
				++e;

			// notice that we throw away the original EOL kind here, maybe we
			// should preserve it?
			if ( e != s )
				memText.AddLine(wxString(s, e));

			if ( *e == '\0' )
				break;

			// skip the second EOL byte if it's a DOS one
			if ( *e == '\r' && e[1] == '\n' )
				++e;

			s = e;
		}

		// Finally we can parse it all.
		Parse(memText, true /* local */);
	}

	SetRootPath();
	ResetDirty();
}

lkFileConfig::~lkFileConfig()
{
	if ( m_autosave )
		Flush();

	CleanUp();

	delete m_conv;
}

void lkFileConfig::CleanUp()
{
	delete m_pRootGroup;

	lkFileConfigLineList* pCur = m_linesHead;
	while ( pCur != NULL )
	{
		lkFileConfigLineList* pNext = pCur->Next();
		delete pCur;
		pCur = pNext;
	}
}

void lkFileConfig::Init()
{
	m_pCurrentGroup = m_pRootGroup = new lkFileConfigGroup(NULL, wxEmptyString, this);

	m_linesHead = m_linesTail = NULL;

	// It's not an error if (one of the) file(s) doesn't exist.

	// parse the global file
	if ( m_fnGlobalFile.IsOk() && m_fnGlobalFile.FileExists() )
	{
		wxTextFile fileGlobal(m_fnGlobalFile.GetFullPath());

		if ( fileGlobal.Open(*m_conv/*ignored in ANSI build*/) )
		{
			Parse(fileGlobal, false /* global */);
			SetRootPath();
		}
		else
		{
			wxLogWarning(wxT("can't open global configuration file '%s'."), m_fnGlobalFile.GetFullPath().c_str());
		}
	}

	// parse the local file
	if ( m_fnLocalFile.IsOk() && m_fnLocalFile.FileExists() )
	{
		wxTextFile fileLocal(m_fnLocalFile.GetFullPath());
		if ( fileLocal.Open(*m_conv/*ignored in ANSI build*/) )
		{
			Parse(fileLocal, true /* local */);
			SetRootPath();
		}
		else
		{
			const wxString path = m_fnLocalFile.GetFullPath();
			wxLogWarning(wxT("can't open user configuration file '%s'."), path.c_str());

			if ( m_fnLocalFile.FileExists() )
			{
				wxLogWarning(wxT("Changes won't be saved to avoid overwriting the existing file \"%s\""), path.c_str());
				m_fnLocalFile.Clear();
			}
		}
	}

	m_isDirty = false;
	m_autosave = true;
}

//static
wxString lkFileConfig::GetGlobalDir()
{
	return wxStandardPaths::Get().GetConfigDir();
}

//static
wxString lkFileConfig::GetLocalDir(int style)
{
	wxUnusedVar(style);

	wxStandardPathsBase& stdp = wxStandardPaths::Get();

	// it so happens that user data directory is a subdirectory of user config
	// directory on all supported platforms, which explains why we use it here
	return (style & wxCONFIG_USE_SUBDIR) ? stdp.GetUserDataDir() : stdp.GetUserConfigDir();
}

//static
wxFileName lkFileConfig::GetGlobalFile(const wxString& szFile)
{
	wxStandardPathsBase& stdp = wxStandardPaths::Get();

	return wxFileName(GetGlobalDir(), stdp.MakeConfigFileName(szFile));
}

//static
wxFileName lkFileConfig::GetLocalFile(const wxString& szFile, int style)
{
	wxStandardPathsBase& stdp = wxStandardPaths::Get();

	// If the config file is located in a subdirectory, we always use an
	// extension for it, but we use just the leading dot if it is located
	// directly in the home directory. Note that if wxStandardPaths is
	// configured to follow XDG specification, all config files go to a
	// subdirectory of XDG_CONFIG_HOME anyhow, so in this case we'll still end
	// up using the extension even if wxCONFIG_USE_SUBDIR is not set, but this
	// is the correct and expected (if a little confusing) behaviour.
	const wxStandardPaths::ConfigFileConv conv = (style & wxCONFIG_USE_SUBDIR) ? wxStandardPaths::ConfigFileConv_Ext : wxStandardPaths::ConfigFileConv_Dot;

	// *** added by LhK-Soft at [19/06/2022]
	wxString localdir = GetLocalDir(style);
	if ( !wxDirExists(localdir) && !wxFileExists(localdir) )
		wxFileName::Mkdir(localdir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	// *** end-add

	return wxFileName(localdir, stdp.MakeConfigFileName(szFile, conv));
}

//static
wxString lkFileConfig::GetGlobalFileName(const wxString& szFile)
{
	return GetGlobalFile(szFile).GetFullPath();
}

//static
wxString lkFileConfig::GetLocalFileName(const wxString& szFile, int style)
{
	return GetLocalFile(szFile, style).GetFullPath();
}

void lkFileConfig::SetUmask(int mode)
{
#ifdef __UNIX__
	m_umask = mode;
#endif // __UNIX__/!__UNIX__
}

void lkFileConfig::EnableAutoSave()
{
	m_autosave = true;
}
void lkFileConfig::DisableAutoSave()
{
	m_autosave = false;
}

void lkFileConfig::SetDirty()
{
	m_isDirty = true;
}
void lkFileConfig::ResetDirty()
{
	m_isDirty = false;
}
bool lkFileConfig::IsDirty() const
{
	return m_isDirty;
}

void lkFileConfig::Parse(const wxTextBuffer& buffer, bool bLocal)
{

	size_t nLineCount = buffer.GetLineCount();

	for ( size_t n = 0; n < nLineCount; n++ )
	{
		wxString strLine = buffer[n];
		// FIXME-UTF8: rewrite using iterators, without this buffer
		wxWxCharBuffer buf(strLine.c_str());
		const wxChar* pStart;
		const wxChar* pEnd;

		// skip leading spaces
		for ( pStart = buf; wxIsspace(*pStart); pStart++ )
			;

		// *** modified by LhK-Soft at [19.06.22]
		// skip blank lines
		if ( *pStart == wxT('\0') || *pStart == wxT('\r') || *pStart == wxT('\n') )
			continue;
		// *** end modification
		
		// *** Position MOVED by LhK-Soft at [19.06.2022] -- blank lines before new [groups] were also copied which shouldn't
		// add the line to linked list
		if ( bLocal )
			LineListAppend(strLine);
		// *** end move

		// *** added by LhK-Soft at [19.06.22]
		// skip comment lines
		if ( *pStart == wxT(';') || *pStart == wxT('#') )
			continue;
		// *** end addition

		if ( *pStart == wxT('[') )
		{          // a new group
			pEnd = pStart;

			while ( *++pEnd != wxT(']') )
			{
				if ( *pEnd == wxT('\\') )
				{
					// the next char is escaped, so skip it even if it is ']'
					pEnd++;
				}

				if ( *pEnd == wxT('\n') || *pEnd == wxT('\0') )
				{
					// we reached the end of line, break out of the loop
					break;
				}
			}

			if ( *pEnd != wxT(']') )
			{
				wxLogError(wxT("file '%s': unexpected character %c at line %zu."), buffer.GetName(), *pEnd, n + 1);
				continue; // skip this line
			}

			// group name here is always considered as abs path
			wxString strGroup;
			pStart++;
			strGroup << wxCONFIG_PATH_SEPARATOR << FilterInEntryName(wxString(pStart, pEnd - pStart));

			// will create it if doesn't yet exist
			SetPath(strGroup);

			if ( bLocal )
			{
				if ( m_pCurrentGroup->Parent() )
					m_pCurrentGroup->Parent()->SetLastGroup(m_pCurrentGroup);
				m_pCurrentGroup->SetLine(m_linesTail);
			}

			// check that there is nothing except comments left on this line
			bool bCont = true;
			while ( *++pEnd != wxT('\0') && bCont )
			{
				switch ( *pEnd )
				{
					case wxT('#'):
					case wxT(';'):
						bCont = false;
						break;

					case wxT(' '):
					case wxT('\t'):
						// ignore whitespace ('\n' impossible here)
						break;

					default:
						wxLogWarning(wxT("file '%s', line %zu: '%s' ignored after group header."), buffer.GetName(), n + 1, pEnd);
						bCont = false;
				}
			}
		}
		else
		{                        // a key
			pEnd = pStart;
			while ( *pEnd && *pEnd != wxT('=') /* && !wxIsspace(*pEnd)*/ )
			{
				if ( *pEnd == wxT('\\') )
				{
					// next character may be space or not - still take it because it's
					// quoted (unless there is nothing)
					pEnd++;
					if ( !*pEnd )
					{
						// the error message will be given below anyhow
						break;
					}
				}

				pEnd++;
			}

			wxString strKey(FilterInEntryName(wxString(pStart, pEnd).Trim()));

			// skip whitespace
			while ( wxIsspace(*pEnd) )
				pEnd++;

			if ( *pEnd++ != wxT('=') )
			{
				wxLogError(wxT("file '%s', line %zu: '=' expected."), buffer.GetName(), n + 1);
			}
			else
			{
				lkFileConfigEntry* pEntry = m_pCurrentGroup->FindEntry(strKey);

				if ( pEntry == NULL )
				{
					// new entry
					pEntry = m_pCurrentGroup->AddEntry(strKey, n);
				}
				else
				{
					if ( bLocal && pEntry->IsImmutable() )
					{
						// immutable keys can't be changed by user
						wxLogWarning(wxT("file '%s', line %zu: value for immutable key '%s' ignored."), buffer.GetName(), n + 1, strKey.c_str());
						continue;
					}
					// the condition below catches the cases (a) and (b) but not (c):
					//  (a) global key found second time in global file
					//  (b) key found second (or more) time in local file
					//  (c) key from global file now found in local one
					// which is exactly what we want.
					else if ( !bLocal || pEntry->IsLocal() )
					{
						wxLogWarning(wxT("file '%s', line %zu: key '%s' was first found at line %d."), buffer.GetName(), n + 1, strKey.c_str(), pEntry->Line());

					}
				}

				if ( bLocal )
					pEntry->SetLine(m_linesTail);

				// skip whitespace
				while ( wxIsspace(*pEnd) )
					pEnd++;

				wxString value = pEnd;
				if ( !(GetStyle() & wxCONFIG_USE_NO_ESCAPE_CHARACTERS) )
					value = FilterInValue(value);

				pEntry->SetValue(value, false);
			}
		}
	}
}

void lkFileConfig::SetRootPath()
{
	m_strPath.Empty();
	m_pCurrentGroup = m_pRootGroup;
}

bool lkFileConfig::DoSetPath(const wxString& strPath, bool createMissingComponents)
{
	wxArrayString aParts;

	if ( strPath.empty() )
	{
		SetRootPath();
		return true;
	}

	if ( strPath[0] == wxCONFIG_PATH_SEPARATOR )
	{
		// absolute path
		wxSplitPath(aParts, strPath);
	}
	else
	{
		// relative path, combine with current one
		wxString strFullPath = m_strPath;
		strFullPath << wxCONFIG_PATH_SEPARATOR << strPath;
		wxSplitPath(aParts, strFullPath);
	}

	// change current group
	size_t n;
	m_pCurrentGroup = m_pRootGroup;
	for ( n = 0; n < aParts.GetCount(); n++ )
	{
		lkFileConfigGroup* pNextGroup = m_pCurrentGroup->FindSubgroup(aParts[n]);
		if ( pNextGroup == NULL )
		{
			if ( !createMissingComponents )
				return false;

			pNextGroup = m_pCurrentGroup->AddSubgroup(aParts[n]);
		}

		m_pCurrentGroup = pNextGroup;
	}

	// recombine path parts in one variable
	m_strPath.Empty();
	for ( n = 0; n < aParts.GetCount(); n++ )
	{
		m_strPath << wxCONFIG_PATH_SEPARATOR << aParts[n];
	}

	return true;
}

void lkFileConfig::SetPath(const wxString& strPath)
{
	DoSetPath(strPath, true /* create missing path components */);
}

const wxString& lkFileConfig::GetPath() const
{
	return m_strPath;
}

bool lkFileConfig::GetFirstGroup(wxString& str, long& lIndex) const
{
	lIndex = 0;
	return GetNextGroup(str, lIndex);
}

bool lkFileConfig::GetNextGroup(wxString& str, long& lIndex) const
{
	if ( size_t(lIndex) < m_pCurrentGroup->Groups().GetCount() )
	{
		str = m_pCurrentGroup->Groups()[(size_t)lIndex++]->Name();
		return true;
	}
	else
		return false;
}

bool lkFileConfig::GetFirstEntry(wxString& str, long& lIndex) const
{
	lIndex = 0;
	return GetNextEntry(str, lIndex);
}

bool lkFileConfig::GetNextEntry(wxString& str, long& lIndex) const
{
	if ( size_t(lIndex) < m_pCurrentGroup->Entries().GetCount() )
	{
		str = m_pCurrentGroup->Entries()[(size_t)lIndex++]->Name();
		return true;
	}
	else
		return false;
}

size_t lkFileConfig::GetNumberOfEntries(bool bRecursive) const
{
	size_t n = m_pCurrentGroup->Entries().GetCount();
	if ( bRecursive )
	{
		lkFileConfig* const self = const_cast<lkFileConfig*>(this);

		lkFileConfigGroup* pOldCurrentGroup = m_pCurrentGroup;
		size_t nSubgroups = m_pCurrentGroup->Groups().GetCount();
		for ( size_t nGroup = 0; nGroup < nSubgroups; nGroup++ )
		{
			self->m_pCurrentGroup = m_pCurrentGroup->Groups()[nGroup];
			n += GetNumberOfEntries(true);
			self->m_pCurrentGroup = pOldCurrentGroup;
		}
	}

	return n;
}

size_t lkFileConfig::GetNumberOfGroups(bool bRecursive) const
{
	size_t n = m_pCurrentGroup->Groups().GetCount();
	if ( bRecursive )
	{
		lkFileConfig* const self = const_cast<lkFileConfig*>(this);

		lkFileConfigGroup* pOldCurrentGroup = m_pCurrentGroup;
		size_t nSubgroups = m_pCurrentGroup->Groups().GetCount();
		for ( size_t nGroup = 0; nGroup < nSubgroups; nGroup++ )
		{
			self->m_pCurrentGroup = m_pCurrentGroup->Groups()[nGroup];
			n += GetNumberOfGroups(true);
			self->m_pCurrentGroup = pOldCurrentGroup;
		}
	}

	return n;
}

bool lkFileConfig::HasGroup(const wxString& strName) const
{
	// special case: DoSetPath("") does work as it's equivalent to DoSetPath("/")
	// but there is no group with empty name so treat this separately
	if ( strName.empty() )
		return false;

	const wxString pathOld = GetPath();

	lkFileConfig* self = const_cast<lkFileConfig*>(this);
	const bool
		rc = self->DoSetPath(strName, false /* don't create missing components */);

	self->SetPath(pathOld);

	return rc;
}

bool lkFileConfig::HasEntry(const wxString& entry) const
{
	// path is the part before the last "/"
	wxString path = entry.BeforeLast(wxCONFIG_PATH_SEPARATOR);

	// except in the special case of "/keyname" when there is nothing before "/"
	if ( path.empty() && *entry.c_str() == wxCONFIG_PATH_SEPARATOR )
	{
		path = wxCONFIG_PATH_SEPARATOR;
	}

	// change to the path of the entry if necessary and remember the old path
	// to restore it later
	wxString pathOld;
	lkFileConfig* const self = const_cast<lkFileConfig*>(this);
	if ( !path.empty() )
	{
		pathOld = GetPath();
		if ( pathOld.empty() )
			pathOld = wxCONFIG_PATH_SEPARATOR;

		if ( !self->DoSetPath(path, false /* don't create if doesn't exist */) )
		{
			return false;
		}
	}

	// check if the entry exists in this group
	const bool exists = m_pCurrentGroup->FindEntry(
		entry.AfterLast(wxCONFIG_PATH_SEPARATOR)) != NULL;

	// restore the old path if we changed it above
	if ( !pathOld.empty() )
	{
		self->SetPath(pathOld);
	}

	return exists;
}

bool lkFileConfig::DoReadString(const wxString& key, wxString* pStr) const
{
	wxConfigPathChanger path(this, key);

	lkFileConfigEntry* pEntry = m_pCurrentGroup->FindEntry(path.Name());
	if ( pEntry == NULL )
	{
		return false;
	}

	*pStr = pEntry->Value();

	return true;
}

bool lkFileConfig::DoReadLong(const wxString& key, long* pl) const
{
	wxString str;
	if ( !Read(key, &str) )
		return false;

	// extra spaces shouldn't prevent us from reading numeric values
	str.Trim();

	return str.ToLong(pl);
}

bool lkFileConfig::DoReadBinary(const wxString& key, wxMemoryBuffer* buf) const
{
	wxCHECK_MSG(buf, false, wxT("NULL buffer"));

	wxString str;
	if ( !Read(key, &str) )
		return false;

	*buf = wxBase64Decode(str);
	return true;
}

bool lkFileConfig::DoWriteString(const wxString& key, const wxString& szValue)
{
	wxConfigPathChanger     path(this, key);
	wxString                strName = path.Name();

	if ( strName.empty() )
	{
		// setting the value of a group is an error

		wxASSERT_MSG(szValue.empty(), wxT("can't set value of a group!"));

		// ... except if it's empty in which case it's a way to force it's creation

		SetDirty();

		// this will add a line for this group if it didn't have it before (or
		// do nothing for the root but it's ok as it always exists anyhow)
		(void)m_pCurrentGroup->GetGroupLine();
	}
	else
	{
		// writing an entry check that the name is reasonable
		if ( strName[0u] == wxCONFIG_IMMUTABLE_PREFIX )
		{
			wxLogError(wxT("Config entry name cannot start with '%c'."), wxCONFIG_IMMUTABLE_PREFIX);
			return false;
		}

		lkFileConfigEntry* pEntry = m_pCurrentGroup->FindEntry(strName);

		if ( pEntry == 0 )
		{
			pEntry = m_pCurrentGroup->AddEntry(strName);
		}

		pEntry->SetValue(szValue);

		SetDirty();
	}

	return true;
}

bool lkFileConfig::DoWriteLong(const wxString& key, long lValue)
{
	return Write(key, wxString::Format(wxT("%ld"), lValue));
}

bool lkFileConfig::DoWriteBinary(const wxString& key, const wxMemoryBuffer& buf)
{
	return Write(key, wxBase64Encode(buf));
}

bool lkFileConfig::Flush(bool /* bCurrentOnly */)
{
	if ( !IsDirty() || !m_fnLocalFile.GetFullPath() )
		return true;

	// set the umask if needed
	wxCHANGE_UMASK(m_umask);

	wxTempFile file(m_fnLocalFile.GetFullPath());

	if ( !file.IsOpened() )
	{
		wxLogError(wxT("can't open user configuration file."));
		return false;
	}

	// write all strings to file
	wxString filetext;
	filetext.reserve(4096);
	bool bFirst = true; // *** added by LhK-Soft at [19/06/2022]
	for ( lkFileConfigLineList* p = m_linesHead; p != NULL; p = p->Next() )
	{
		// *** added by LhK-Soft at [19/06/2022]
		const wxString& l = p->Text();
		if ( !l.IsEmpty() && (l[0] == '[') )
			if ( !bFirst )
				filetext << wxTextFile::GetEOL();
		bFirst = false;
		// *** end-add

		filetext << l << wxTextFile::GetEOL();
	}

	if ( !file.Write(filetext, *m_conv) )
	{
		wxLogError(wxT("can't write user configuration file."));
		return false;
	}

	if ( !file.Commit() )
	{
		wxLogError(wxT("Failed to update user configuration file."));
		return false;
	}

	ResetDirty();

	return true;
}

bool lkFileConfig::Save(wxOutputStream& os, const wxMBConv& conv)
{
	// save unconditionally, even if not dirty
	bool bFirst = true; // *** added by LhK-Soft at [19/06/2022]
	for ( lkFileConfigLineList* p = m_linesHead; p != NULL; p = p->Next() )
	{
		// *** added by LhK-Soft at [19/06/2022]
		const wxString& l = p->Text();
		wxString line;
		line = wxT("");
		if ( !l.IsEmpty() && (l[0] == '[') )
			if ( !bFirst )
				line = wxTextFile::GetEOL();
		bFirst = false;
		line += l;
		// *** end-add

		line += wxTextFile::GetEOL();

		wxCharBuffer buf(line.mb_str(conv));
		if ( !os.Write(buf, strlen(buf)) )
		{
			wxLogError(wxT("Error saving user configuration data."));

			return false;
		}
	}

	ResetDirty();

	return true;
}

bool lkFileConfig::RenameEntry(const wxString& oldName, const wxString& newName)
{
	wxASSERT_MSG(oldName.find(wxCONFIG_PATH_SEPARATOR) == wxString::npos, wxT("RenameEntry(): paths are not supported"));

	// check that the entry exists
	lkFileConfigEntry* oldEntry = m_pCurrentGroup->FindEntry(oldName);
	if ( !oldEntry )
		return false;

	// check that the new entry doesn't already exist
	if ( m_pCurrentGroup->FindEntry(newName) )
		return false;

	// delete the old entry, create the new one
	wxString value = oldEntry->Value();
	if ( !m_pCurrentGroup->DeleteEntry(oldName) )
		return false;

	SetDirty();

	lkFileConfigEntry* newEntry = m_pCurrentGroup->AddEntry(newName);
	newEntry->SetValue(value);

	return true;
}

bool lkFileConfig::RenameGroup(const wxString& oldName, const wxString& newName)
{
	// check that the group exists
	lkFileConfigGroup* group = m_pCurrentGroup->FindSubgroup(oldName);
	if ( !group )
		return false;

	// check that the new group doesn't already exist
	if ( m_pCurrentGroup->FindSubgroup(newName) )
		return false;

	group->Rename(newName);

	SetDirty();

	return true;
}

bool lkFileConfig::DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso)
{
	wxConfigPathChanger path(this, key);

	if ( !m_pCurrentGroup->DeleteEntry(path.Name()) )
		return false;

	SetDirty();

	if ( bGroupIfEmptyAlso && m_pCurrentGroup->IsEmpty() )
	{
		if ( m_pCurrentGroup != m_pRootGroup )
		{
			lkFileConfigGroup* pGroup = m_pCurrentGroup;
			SetPath(wxT(".."));  // changes m_pCurrentGroup!
			m_pCurrentGroup->DeleteSubgroupByName(pGroup->Name());
		}
		//else: never delete the root group
	}

	return true;
}

bool lkFileConfig::DeleteGroup(const wxString& key)
{
	wxConfigPathChanger path(this, RemoveTrailingSeparator(key));

	if ( !m_pCurrentGroup->DeleteSubgroupByName(path.Name()) )
		return false;

	path.UpdateIfDeleted();

	SetDirty();

	return true;
}

bool lkFileConfig::DeleteAll()
{
	CleanUp();

	if ( m_fnLocalFile.IsOk() )
	{
		if ( m_fnLocalFile.FileExists() &&
			!wxRemoveFile(m_fnLocalFile.GetFullPath()) )
		{
			wxLogSysError(wxT("can't delete user configuration file '%s'"), m_fnLocalFile.GetFullPath().c_str());
			return false;
		}
	}

	Init();

	return true;
}

// append a new line to the end of the list
lkFileConfigLineList* lkFileConfig::LineListAppend(const wxString& str)
{
	lkFileConfigLineList* pLine = new lkFileConfigLineList(str);

	if ( m_linesTail == NULL )
	{
		// list is empty
		m_linesHead = pLine;
	}
	else
	{
		// adjust pointers
		m_linesTail->SetNext(pLine);
		pLine->SetPrev(m_linesTail);
	}

	m_linesTail = pLine;

	return m_linesTail;
}

// insert a new line after the given one or in the very beginning if !pLine
lkFileConfigLineList* lkFileConfig::LineListInsert(const wxString& str, lkFileConfigLineList* pLine)
{
	if ( pLine == m_linesTail )
		return LineListAppend(str);

	lkFileConfigLineList* pNewLine = new lkFileConfigLineList(str);
	if ( pLine == NULL )
	{
		// prepend to the list
		pNewLine->SetNext(m_linesHead);
		m_linesHead->SetPrev(pNewLine);
		m_linesHead = pNewLine;
	}
	else
	{
		// insert before pLine
		lkFileConfigLineList* pNext = pLine->Next();
		pNewLine->SetNext(pNext);
		pNewLine->SetPrev(pLine);
		pNext->SetPrev(pNewLine);
		pLine->SetNext(pNewLine);
	}

	return pNewLine;
}

void lkFileConfig::LineListRemove(lkFileConfigLineList* pLine)
{
	lkFileConfigLineList* pPrev = pLine->Prev(), * pNext = pLine->Next();

	// first entry?

	if ( pPrev == NULL )
		m_linesHead = pNext;
	else
		pPrev->SetNext(pNext);

	// last entry?

	if ( pNext == NULL )
		m_linesTail = pPrev;
	else
		pNext->SetPrev(pPrev);

	delete pLine;
}

bool lkFileConfig::LineListIsEmpty()
{
	return m_linesHead == NULL;
}

