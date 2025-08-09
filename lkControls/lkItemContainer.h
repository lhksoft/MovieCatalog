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
#ifndef __LK_ITEMCONTAINER_H__
#define __LK_ITEMCONTAINER_H__
#pragma once

#include <wx/arrstr.h>
#include <wx/clntdata.h>

/* *****************************************************************************************
 * Class lkItemContainer
 * ---------------------
 * Based at wxItemContainerImmutable and wxItemContainer but not making pure abstract classes,
 * but as subclasses which one can just add to a wxControl class to have nearly the same
 * functionality.
 * 
 * In these classes the same functionality will be used as with MFC where custom data and the actual
 * strings stays together (not seperate as in wxWidgets)
 * 
 * The differance with wxWidgets is that you can set mixed data as custom data,
 * either derived from wxClientData, or a pure void*. If an item is set as object,
 * it will be freed first before setting a new owbject/void*
 * ----------------------
 * This class is used as a sub-base class of <lkChkImgListbox>
 * ***************************************************************************************** */

class lkItemContainer
{
public:
	lkItemContainer();
	virtual ~lkItemContainer();

public:
    // accessing data
    // --------------

    unsigned int                GetCount(void) const;
    bool                        IsEmpty(void) const;

    wxString                    GetString(unsigned int) const;

    // returns a copy of internal strings
    wxArrayString               GetStrings(void) const;
    void                        SetString(unsigned int, const wxString&);

    // if used in a cheklistbox...

    bool                        IsChecked(unsigned int) const;
    void                        SetCheck(unsigned int, bool);

    // finding string natively is either case sensitive or insensitive but never both so fall back to this base version for not supported search type
    int                         FindString(const wxString&, bool bCase = false) const;

    // adding / inserting items
    // ------------------------

    int                         AppendItem(const wxString&, const void*, wxClientDataType);
    int                         InsertItem(unsigned int, const wxString&, const void*, wxClientDataType);

    // replacing items
    // ---------------

    int                         SetAt(unsigned int pos, const wxString& s, const void* clientData, wxClientDataType type);

protected:
    // various accessors
    // -----------------

    // check that the index is valid
    bool                        IsValid(unsigned int n) const;
    bool                        IsValidInsert(unsigned int n) const;

    // The control may maintain its items in a sorted order in which case
    // items are automatically inserted at the right position when they are
    // inserted or appended. Derived classes have to override this method if
    // they implement sorting, typically by returning HasFlag(wxXX_SORT)
    virtual bool                IsSorted(void) const;

    void                        SetInternalData(unsigned int, const void*, wxClientDataType);

public:
    // deleting items
    // --------------

    void                        RemoveAt(unsigned int pos);
    void                        Clear(void);

    // client data stuff
    // -----------------

    void                        SetClientData(unsigned int, const void*);
    void*                       GetClientData(unsigned int) const;

    // SetClientObject() takes ownership of the pointer, GetClientObject()
    // returns it but keeps the ownership while DetachClientObject() expects
    // the caller to delete the pointer and also resets the internally stored
    // one to NULL for this item
    void                        SetClientObject(unsigned int, wxClientData*);
    wxClientData*               GetClientObject(unsigned int) const;
    wxClientData*               DetachClientObject(unsigned int);

private:
    wxArrayPtrVoid				m_InternalData; // contains objects of lkCtrlData
};



#endif // !__LK_ITEMCONTAINER_H__
