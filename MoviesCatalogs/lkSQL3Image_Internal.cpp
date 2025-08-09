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
#include "lkSQL3Image_Internal.h"

#include <wx/mstream.h>
#include <wx/utils.h>

static const char* m_szVer1String = "LhK-Soft\0";
static const char* m_szVer3String = "LhKSoft\0";

#pragma pack(push, 1)

typedef struct DAO_IMAGE1
{ // version 1
	char		reserved[9];	// always will contain 'LhK-Soft\0' [ 9bytes]
	wxUint32	size;			// size of this structure			[ 4bytes] -- = 21bytes for version#1
	wxUint32	szRaw;			// size of raw-image data			[ 4bytes]
	wxUint32	lkType;			// one of the ClkImage::<formats>	[ 4bytes]
								//	char[]		filename;		// filename, must include terminating '\0' [variable length-can be '\0' if no name]
								//	BYTE[]		raw;			// raw data of the actual image [variable length]
} DAO_IMAGE1_t, * DAO_IMAGE1_p; // should be 21bytes
typedef struct DAO_IMAGE2
{ // version 2
	char		reserved[8];	// always will contain 'LhKSoft\0'  [ 8bytes]
	wxUint8		version;		// '2' ...							[ 1byte ]
	wxUint32	size;			// size of this structure			[ 4bytes] -- = 21bytes for version#2
	wxUint32	szRaw;			// size of raw-image data			[ 4bytes]
	wxUint32	lkType;			// one of the ClkImage::<formats>	[ 4bytes]
								//	char[]		filename;		// filename, must include terminating '\0' [variable length-can be '\0' if no name]
								//	BYTE[]		raw;			// raw data of the actual image [variable length]
} DAO_IMAGE2_t, * DAO_IMAGE2_p; // should be 21bytes
typedef struct DAO_IMAGE3
{ // version 3
	char		reserved[8];	// always will contain 'LhKSoft\0'  [ 8bytes]
	wxUint8		version;		// '3' ...							[ 1byte ]
	wxUint32	size;			// size of this structure			[ 4bytes] -- = 21bytes for version#3
	wxUint32	szRaw;			// size of raw-image data			[ 4bytes]
	wxUint32	wxType;			// one of the wxBitmap_Types		[ 4bytes]
								//	UTF8char[]	filename;		// filename, must include terminating '\0' [variable length-can be '\0' if no name]
								//	BYTE[]		raw;			// raw data of the actual image [variable length]
} DAO_IMAGE3_t, * DAO_IMAGE3_p; // should be 21bytes

#pragma pack(pop)

// --------------------------------------------------------------------------------------------------------------- //

lkSQL3Image_In::lkSQL3Image_In()
{
	m_pBuf = NULL;
	m_nSize = 0;
}
lkSQL3Image_In::~lkSQL3Image_In()
{
	if ( m_pBuf )
		delete[] m_pBuf;
}

const void* lkSQL3Image_In::GetBlob() const
{
	return static_cast<const void*>(m_pBuf);
}
size_t lkSQL3Image_In::GetSize() const
{
	return m_nSize;
}

bool lkSQL3Image_In::SetFromImage(const wxImage& _img, const wxString& _name)
{
	if ( !_img.IsOk() )
		return false;

	wxUint8* ibuf = NULL;
	size_t size = 0;
	if ( !lkSQL3Image_In::CopyImageToBuffer(_img, (void**)(&ibuf), size) || (ibuf == NULL) )
		return false;

	// now all okay
	wxUint32 imgLen = (wxUint32)(size & 0xFFFFFFFF);
	wxBitmapType imgType = _img.GetType();

	size_t imgName_len = 0;
	wxCharBuffer imgName;
	if ( !_name.IsEmpty() )
	{
		imgName = _name.ToUTF8();
		imgName_len = wxStrlen(imgName);
	}

	// ---


	size_t nReq = sizeof(DAO_IMAGE3);
	nReq += imgName_len;
	nReq++; // terminating '\0'
	nReq += imgLen;

	bool bRet = false;
	if ( m_pBuf != NULL )
	{
		delete m_pBuf; m_pBuf = NULL;
		m_nSize = 0;
	}
	m_pBuf = new wxUint8[nReq];
	if ( m_pBuf != NULL )
	{
		memset(m_pBuf, 0, sizeof(wxUint8) * nReq);
		m_nSize = nReq;

		size_t nPos = 0;
		DAO_IMAGE3_p pStr = (DAO_IMAGE3_p)m_pBuf;
		wxStrcpy(pStr->reserved, m_szVer3String);
		pStr->version = 3;
		pStr->size = (wxUint32)sizeof(DAO_IMAGE3);
		pStr->szRaw = imgLen; // an image wouldn't be larger than 4GB
		pStr->wxType = (wxUint32)imgType;
		nPos += sizeof(DAO_IMAGE3);

		if ( imgName_len > 0 )
		{
			wxStrcpy(&((char*)m_pBuf)[nPos], imgName);
			nPos += imgName_len;
		}
		nPos++;

		wxASSERT(imgLen + nPos == nReq);
		memcpy(&((wxUint8*)m_pBuf)[nPos], ibuf, imgLen);

//		SetValue2((const void*)p, nReq);
		bRet = true;
	}
	delete[] ibuf;
	return bRet;
}

//static
bool lkSQL3Image_In::CopyImageToBuffer(const wxImage& _img, void** pBuf, size_t& size)
{
	if ( !_img.IsOk() || (pBuf == NULL) )
		return false;

	wxBitmapType nType = _img.GetType();
	wxMemoryOutputStream buf;
	if ( !_img.SaveFile(buf, nType) )
		return false;

	size = buf.GetSize();
	if ( size == 0 )
		return false;

	*pBuf = new wxUint8[size];
	if ( *pBuf == NULL )
		return false;

	size_t copied = buf.CopyTo(*pBuf, size);
	if ( copied != size )
	{
		delete[] (wxUint8*)(*pBuf);
		*pBuf = NULL;
		size = 0;
		return false;
	}
	return true;
}


// --------------------------------------------------------------------------------------------------------------- //

lkSQL3Image_Out::lkSQL3Image_Out() : m_Image(), m_Name()
{
	m_Type = wxBITMAP_TYPE_INVALID;
}
lkSQL3Image_Out::~lkSQL3Image_Out()
{
}

wxImage lkSQL3Image_Out::GetImage() const
{
	return m_Image;
}
wxString lkSQL3Image_Out::GetName() const
{
	return m_Name;
}
wxBitmapType lkSQL3Image_Out::GetType() const
{
	return m_Type;
}

bool lkSQL3Image_Out::SetFromBlob(const void* p, size_t t)
{
	wxUint8 nVer = 0;
	wxUint8* pBuf = NULL;
	if ( (p != NULL) && (t > 0) )
	{
		// Analize the Blob
		size_t smallest = wxMin(sizeof(DAO_IMAGE1) + 1, sizeof(DAO_IMAGE2) + 1);
		smallest = wxMin(smallest, sizeof(DAO_IMAGE3) + 1);
		if ( t < smallest )
			return false;

		pBuf = new wxUint8[t];
		if ( pBuf )
			memcpy(pBuf, p, t);
		else
			return false;

		if ( memcmp(pBuf, m_szVer1String, 8) == 0 )
			nVer = 1;
		else if ( memcmp(pBuf, m_szVer3String, 7) == 0 )
			nVer = 2; // might also be 3

		if ( nVer == 2 )
		{
			// bug, appearantly in version 2 the version-int wasn't set :o
			DAO_IMAGE2_p pStr = (DAO_IMAGE2_p)pBuf;
			nVer = (pStr->version == 0) ? 2 : pStr->version;
		}
	}
	if ( nVer == 0 )
	{
		if (pBuf) delete[] pBuf;
		return false;
	}

	wxUint32 nSize = 0;
	wxBitmapType nType = wxBITMAP_TYPE_INVALID;
	size_t nPos = 0;

	switch ( nVer )
	{
		case 1:
		{
			DAO_IMAGE1_p pStr1 = (DAO_IMAGE1_p)pBuf;

			nType = ConvertLkImageType(pStr1->lkType);
			nSize = pStr1->szRaw;
			nPos = sizeof(DAO_IMAGE1);
			break;
		}
		case 2:
		{
			DAO_IMAGE2_p pStr2 = (DAO_IMAGE2_p)pBuf;

			nType = ConvertLkImageType(pStr2->lkType);
			nSize = pStr2->szRaw;
			nPos = sizeof(DAO_IMAGE2);
			break;
		}
		case 3:
		{
			DAO_IMAGE3_p pStr3 = (DAO_IMAGE3_p)pBuf;

			nType = (wxBitmapType)pStr3->wxType;
			nSize = pStr3->szRaw;
			nPos = sizeof(DAO_IMAGE3);
			break;
		}
		default:
			nVer = 0;
			break;
	}

	if ( nVer == 0 )
	{
		delete[] pBuf;
		return false;
	}

	const char* sFile = &((const char*)pBuf)[nPos];

	nPos += wxStrlen(sFile);
	nPos++; // must count '\0' too
	if ( t < nPos + (size_t)nSize )
	{
		delete[] pBuf;
		return false;
	}
	wxASSERT(t == nPos + (size_t)nSize);

	// then it defenitely is ours -- fill local variables

	// First create a memory-stream suitable for wxImage
	wxMemoryInputStream buffer(&((wxUint8*)pBuf)[nPos], nSize);

	// Now load into wxImage
	m_Image.LoadFile(buffer, nType);

	if ( !m_Image.IsOk() )
	{
		delete[] pBuf;
		return false;
	}
	m_Name = wxString::FromUTF8(sFile);
	m_Type = nType;
	delete[] pBuf;

	return true;
}

const wxBitmapType lkSQL3Image_Out::ConvertLkImageType(wxUint32 lkType) const
{
	enum class lkImage_Supported_Formats
	{
		LKIMAGE_FORMAT_UNKNOWN = 0x00000,
		LKIMAGE_FORMAT_BMP = 0x00001,
		LKIMAGE_FORMAT_GIF = 0x00002,
		LKIMAGE_FORMAT_JPG = 0x00004,
		LKIMAGE_FORMAT_PNG = 0x00008,
		LKIMAGE_FORMAT_MNG = 0x00010,
		LKIMAGE_FORMAT_ICO = 0x00020,
		LKIMAGE_FORMAT_TIF = 0x00040,
		LKIMAGE_FORMAT_TGA = 0x00080,
		LKIMAGE_FORMAT_PCX = 0x00100,
		LKIMAGE_FORMAT_WBMP = 0x00200,
		LKIMAGE_FORMAT_WMF = 0x00400,
		LKIMAGE_FORMAT_JP2 = 0x00800,
		LKIMAGE_FORMAT_JPC = 0x01000,
		LKIMAGE_FORMAT_PGX = 0x02000,
		LKIMAGE_FORMAT_PNM = 0x04000, // changed: Portable Network Map (PPM & PNM, def.ext.: PPM)
		LKIMAGE_FORMAT_RAS = 0x08000,
		LKIMAGE_FORMAT_PGM = 0x10000, // Portable Network Map (only GRAYscale)
		LKIMAGE_FORMAT_RAW = 0X20000,
		//		LKIMAGE_FORMAT_SKA		= 0X40000,
		LKIMAGE_FORMAT_ALL = 0x80000  // equals '*.*' = All Files
	};

	wxBitmapType wxType = wxBITMAP_TYPE_INVALID;
	if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_BMP )
		wxType = wxBITMAP_TYPE_BMP;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_GIF )
		wxType = wxBITMAP_TYPE_GIF;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_ICO )
		wxType = wxBITMAP_TYPE_ICO;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_JPG )
		wxType = wxBITMAP_TYPE_JPEG;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_PCX )
		wxType = wxBITMAP_TYPE_PCX;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_PNG )
		wxType = wxBITMAP_TYPE_PNG;
	else if ( (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_PNM) || (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_PGM) )
		wxType = wxBITMAP_TYPE_PNM;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_TGA )
		wxType = wxBITMAP_TYPE_TGA;
	else if ( lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_TIF )
		wxType = wxBITMAP_TYPE_TIF;
	else if ( (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_JP2) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_JPC) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_MNG) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_PGX) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_RAS) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_RAW) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_UNKNOWN) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_WBMP) ||
			 (lkType & (wxUint32)lkImage_Supported_Formats::LKIMAGE_FORMAT_WMF) )
		wxType = wxBITMAP_TYPE_ANY; // maybe wxWidgets knows how to deal with these types

	return wxType;
}

