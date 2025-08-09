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
#include "lkItemContainer.h"
#include "lkCtrlData.h" // -- private data class

lkItemContainer::lkItemContainer() : m_InternalData()
{
}
lkItemContainer::~lkItemContainer()
{
    Clear();
}


// accessing data
// --------------

unsigned int lkItemContainer::GetCount() const
{
    return m_InternalData.GetCount();
}
bool lkItemContainer::IsEmpty() const
{
    return m_InternalData.IsEmpty();
}

wxString lkItemContainer::GetString(unsigned int n) const
{
    wxCHECK_MSG(IsValid(n), wxEmptyString, wxT("invalid index in lkItemContainer::GetString"));

    const lkCtrlData* pItem = (const lkCtrlData*)(m_InternalData[n]);
    wxASSERT(pItem != NULL);
    return (pItem) ? pItem->GetData() : wxString();
}

wxArrayString lkItemContainer::GetStrings() const
{
    wxArrayString result;

    const unsigned int count = GetCount();
    result.Alloc(count);
    for ( unsigned int n = 0; n < count; n++ )
        result.Add(GetString(n));

    return result;
}

void lkItemContainer::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET(IsValid(n), wxT("invalid index in lkItemContainer::SetString"));

    lkCtrlData* pItem = (lkCtrlData*)(m_InternalData[n]);
    wxASSERT(pItem != NULL);

    pItem->SetData(s);
}

bool lkItemContainer::IsChecked(unsigned int n) const
{
    wxCHECK_MSG(IsValid(n), false, wxT("invalid index in lkItemContainer::IsChecked"));
    if ( !IsValid(n) )
        return false;

    lkCtrlData* pItem = (lkCtrlData*)(m_InternalData[n]);
    wxASSERT(pItem != NULL);

    return pItem->IsChecked();
}
void lkItemContainer::SetCheck(unsigned int n, bool c)
{
    wxCHECK_RET(IsValid(n), wxT("invalid index in lkItemContainer::SetCheck"));

    lkCtrlData* pItem = (lkCtrlData*)(m_InternalData[n]);
    wxASSERT(pItem != NULL);

    pItem->SetCheck(c);
}

int lkItemContainer::FindString(const wxString& s, bool bCase) const
{
    unsigned int count = GetCount();

    for ( unsigned int i = 0; i < count; i++ )
    {
        if ( GetString(i).IsSameAs(s, bCase) )
            return (int)i;
    }

    return wxNOT_FOUND;
}

// adding / inserting items
// ------------------------

int lkItemContainer::AppendItem(const wxString& s, const void* clientData, wxClientDataType type)
{
    unsigned int _count = GetCount();
    unsigned int pos = _count;

    if (IsSorted())
    {
        lkCtrlData* pItem;
        // Find position
        for ( unsigned int i = 0; i < _count; i++ )
        {
            pItem = (lkCtrlData*)(m_InternalData[i]);
            wxASSERT(pItem != NULL);
            if ( (pItem != NULL) && (pItem->GetData().CmpNoCase(s) > 0) )
            {
                pos = i;
                break;
            }
        }
    }

    return InsertItem(pos, s, clientData, type);
}

int lkItemContainer::InsertItem(unsigned int pos, const wxString& s, const void* clientData, wxClientDataType type)
{
    unsigned int _count = GetCount();
    if ( pos > _count )
        pos = _count;

    lkCtrlData* pData = new lkCtrlData(s);
    if ( clientData )
    {
        if ( type == wxClientDataType::wxClientData_Object )
            pData->SetClientObject((wxClientData*)clientData);
        else
            pData->SetClientData((void*)clientData); // can be NULL
    }

    m_InternalData.Insert(NULL, pos, 1);
    m_InternalData[pos] = pData;

    return pos;
}


// replacing items
// ---------------

int lkItemContainer::SetAt(unsigned int pos, const wxString& s, const void* clientData, wxClientDataType type)
{
    if ( pos > GetCount() )
        return wxNOT_FOUND;

    lkCtrlData* pItem = (lkCtrlData*)(m_InternalData[pos]);
    wxASSERT(pItem != NULL);
    if ( pItem != NULL )
    {
        pItem->SetData(s);

        if ( type == wxClientDataType::wxClientData_Object )
            pItem->SetClientObject((wxClientData*)clientData);
        else
            pItem->SetClientData((void*)clientData);

        return pos;
    }

    return wxNOT_FOUND;
}
 


// various accessors
// -----------------

bool lkItemContainer::IsValid(unsigned int n) const
{
    return (((int)n >= 0) && (n < GetCount()));
}
bool lkItemContainer::IsValidInsert(unsigned int n) const
{
    return (((int)n >= 0) && (n <= GetCount()));
}

//virtual
bool lkItemContainer::IsSorted() const
{
    return false;
}


// deleting items
// --------------

void lkItemContainer::RemoveAt(unsigned int pos)
{
    if ( !IsValid(pos) )
        return;

    lkCtrlData* pItem = (lkCtrlData*)(m_InternalData[pos]);
    m_InternalData.RemoveAt(pos, 1);
    if ( pItem != NULL ) delete pItem;
}

void lkItemContainer::Clear()
{
    if ( IsEmpty() )
        return;

    lkCtrlData* pItem;
    for ( unsigned int i = 0; i < GetCount(); i++ )
    {
        pItem = (lkCtrlData*)(m_InternalData[i]);
        wxASSERT(pItem != NULL);
        if ( pItem != NULL ) delete pItem;
    }
    m_InternalData.Empty();
}


// client data stuff
// -----------------

void lkItemContainer::SetInternalData(unsigned int n, const void* pV, wxClientDataType dataType)
{
    wxCHECK_RET(IsValid(n), wxT("invalid index in lkItemContainer::SetInternalData"));
    if ( !IsValid(n) )
        return;

    lkCtrlData* pData = (lkCtrlData*)(m_InternalData[n]);
    if ( pData != NULL )
    {
        if ( dataType == wxClientDataType::wxClientData_Object )
            pData->SetClientObject((wxClientData*)pV);
        else
            pData->SetClientData((void*)pV);
    }
}

void lkItemContainer::SetClientData(unsigned int n, const void* pV)
{
    SetInternalData(n, pV, wxClientDataType::wxClientData_Void);
}
void* lkItemContainer::GetClientData(unsigned int n) const
{
    wxCHECK_MSG(IsValid(n), (void*)NULL, wxT("invalid index in lkItemContainer::GetClientData"));
    if ( !IsValid(n) )
        return NULL;

    lkCtrlData* pData = (lkCtrlData*)(m_InternalData[n]);
    if ( pData != NULL )
        return pData->GetClientData();

    return NULL;
}

void lkItemContainer::SetClientObject(unsigned int n, wxClientData* pO)
{
    SetInternalData(n, (const void*)pO, wxClientDataType::wxClientData_Object);
}
wxClientData* lkItemContainer::GetClientObject(unsigned int n) const
{
    wxCHECK_MSG(IsValid(n), (wxClientData*)NULL, wxT("invalid index in lkItemContainer::GetClientObject"));
    if ( !IsValid(n) )
        return NULL;

    lkCtrlData* pData = (lkCtrlData*)(m_InternalData[n]);
    if ( pData != NULL )
        return pData->GetClientObject();

    return NULL;
}
wxClientData* lkItemContainer::DetachClientObject(unsigned int n)
{
    wxCHECK_MSG(IsValid(n), (wxClientData*)NULL, wxT("invalid index in lkItemContainer::DetachClientObject"));
    if ( !IsValid(n) )
        return NULL;

    lkCtrlData* pData = (lkCtrlData*)(m_InternalData[n]);
    if ( pData != NULL )
        return pData->DetachClientObject();

    return NULL;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

