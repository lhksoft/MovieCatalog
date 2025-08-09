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
/* *************************************************************************
 * Very enoying wxCoders use 'private' in their classes, so ppl like me
 * then completely must overwrite their classes to get certain things
 * done, even for very small modifications.
 * 
 * *********************************************************************** */
#ifndef __LK_FILECONFIG_H__
#define __LK_FILECONFIG_H__
#pragma once

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/confbase.h>
#include <wx/filename.h>
#include <wx/textfile.h>

class lkFileConfigGroup;
class lkFileConfigEntry;
class lkFileConfigLineList;

class wxInputStream;
class wxOutputStream;

class lkFileConfig : public wxConfigBase
{
public:
	// ctor & dtor
	  // New constructor: one size fits all. Specify wxCONFIG_USE_LOCAL_FILE or
	  // wxCONFIG_USE_GLOBAL_FILE to say which files should be used.
	lkFileConfig(const wxString& appName = wxEmptyString,
				 const wxString& vendorName = wxEmptyString,
				 const wxString& localFilename = wxEmptyString,
				 const wxString& globalFilename = wxEmptyString,
				 long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE,
				 const wxMBConv& conv = wxConvAuto());

	// ctor that takes an input stream.
	lkFileConfig(wxInputStream& inStream, const wxMBConv& conv = wxConvAuto());

	// dtor will save unsaved data
	virtual ~lkFileConfig();

	static wxFileName GetGlobalFile(const wxString& szFile);
	static wxFileName GetLocalFile(const wxString& szFile, int style = 0);

	static wxString GetGlobalFileName(const wxString& szFile);
	static wxString GetLocalFileName(const wxString& szFile, int style = 0);

	// under Unix, set the umask to be used for the file creation, do nothing under other systems
	void								SetUmask(int mode);

	// implement inherited pure virtual functions
	virtual void						SetPath(const wxString& strPath) wxOVERRIDE;
	virtual const wxString&				GetPath() const wxOVERRIDE;

	virtual bool						GetFirstGroup(wxString& str, long& lIndex) const wxOVERRIDE;
	virtual bool						GetNextGroup(wxString& str, long& lIndex) const wxOVERRIDE;
	virtual bool						GetFirstEntry(wxString& str, long& lIndex) const wxOVERRIDE;
	virtual bool						GetNextEntry(wxString& str, long& lIndex) const wxOVERRIDE;

	virtual size_t						GetNumberOfEntries(bool bRecursive = false) const wxOVERRIDE;
	virtual size_t						GetNumberOfGroups(bool bRecursive = false) const wxOVERRIDE;

	virtual bool						HasGroup(const wxString& strName) const wxOVERRIDE;
	virtual bool						HasEntry(const wxString& strName) const wxOVERRIDE;

	virtual bool						Flush(bool bCurrentOnly = false) wxOVERRIDE;

	virtual bool						RenameEntry(const wxString& oldName, const wxString& newName) wxOVERRIDE;
	virtual bool						RenameGroup(const wxString& oldName, const wxString& newName) wxOVERRIDE;

	virtual bool						DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso = true) wxOVERRIDE;
	virtual bool						DeleteGroup(const wxString& szKey) wxOVERRIDE;
	virtual bool						DeleteAll() wxOVERRIDE;

	// additional, wxFileConfig-specific, functionality

	// save the entire config file text to the given stream, note that the text
	// won't be saved again in dtor when Flush() is called if you use this method
	// as it won't be "changed" any more
	virtual bool						Save(wxOutputStream& os, const wxMBConv& conv = wxConvAuto());

	void								EnableAutoSave(void);
	void								DisableAutoSave(void);

public:
	// functions to work with this list
	lkFileConfigLineList*				LineListAppend(const wxString& str);
	lkFileConfigLineList*				LineListInsert(const wxString& str, lkFileConfigLineList* pLine);    // NULL => Prepend()
	void								LineListRemove(lkFileConfigLineList* pLine);
	bool								LineListIsEmpty(void);

protected:
	virtual bool						DoReadString(const wxString& key, wxString* pStr) const wxOVERRIDE;
	virtual bool						DoReadLong(const wxString& key, long* pl) const wxOVERRIDE;
	virtual bool						DoReadBinary(const wxString& key, wxMemoryBuffer* buf) const wxOVERRIDE;

	virtual bool						DoWriteString(const wxString& key, const wxString& szValue) wxOVERRIDE;
	virtual bool						DoWriteLong(const wxString& key, long lValue) wxOVERRIDE;
	virtual bool						DoWriteBinary(const wxString& key, const wxMemoryBuffer& buf) wxOVERRIDE;

// was.. private, should be protected  !!
protected:
	// GetXXXFileName helpers: return ('/' terminated) directory names
	static wxString						GetGlobalDir(void);
	static wxString						GetLocalDir(int style = 0);

	// common part of from dtor and DeleteAll
	void								CleanUp(void);

	// parse the whole file
	void								Parse(const wxTextBuffer& buffer, bool bLocal);

	// the same as SetPath("/")
	void								SetRootPath();

	// real SetPath() implementation, returns true if path could be set or false if path doesn't exist and createMissingComponents == false
	bool								DoSetPath(const wxString& strPath, bool createMissingComponents);

	// set/test the dirty flag
	void								SetDirty(void);
	void								ResetDirty(void);
	bool								IsDirty(void) const;


	// member variables
	// ----------------
	lkFileConfigLineList*				m_linesHead,		// head of the linked list
										*m_linesTail;		// tail

	wxFileName							m_fnLocalFile,		// local  file name passed to ctor
										m_fnGlobalFile;		// global
	wxString							m_strPath;			// current path (not '/' terminated)

	lkFileConfigGroup*					m_pRootGroup,		// the top (unnamed) group
										* m_pCurrentGroup;	// the current group

	wxMBConv*							m_conv;

#ifdef __UNIX__
	int									m_umask;			// the umask to use for file creation
#endif // __UNIX__

	bool								m_isDirty;			// if true, we have unsaved changes
	bool								m_autosave;			// if true, save changes on destruction


// common part of all ctors (assumes that m_str{Local|Global}File are already initialized
private:
	void Init();

	wxDECLARE_ABSTRACT_CLASS(lkFileConfig);
	wxDECLARE_NO_COPY_CLASS(lkFileConfig);
};

#endif // !__LK_FILECONFIG_H__
