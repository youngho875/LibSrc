#include "pch.h"
#include "ObjectText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



namespace OVERLAY
{
	const char* OVERLAY::strFontStyle[] =
	{
		"REGULAR",
		"BOLD",				// 1
		"ITALIC",			// 2
		"BOLD ITALIC",
		"UNDERLINE",		// 4
		"STRIKEDOUT",		// 8
	};

	const WCHAR* OVERLAY::strFamilyName[] =
	{
		L"±Ã¼­",
		L"±¼¸²",
		L"±¼¸²Ã¼",
		L"µ¸¿ò",
		L"¹ÙÅÁ",
		L"¸¼Àº °íµñ",
		L"Alial",
		L"HY°ß°íµñ",
		L"HY°ß¸íÁ¶",
		L"¹°¾ð",
	};

	ObjectText::ObjectText()
		:OBJECT(TEXT, POINTS_OF_OBJECT_TEXT)
	{
		//m_FontColor 	= 0xffffffff;//
		m_FontColor = 0xff32CD32;
		m_dFontSize = 0.0;
		m_eFamilyName = FAMILY_NAME_GODICK;
		m_eFontStyle = Gdiplus::FontStyleRegular;
		m_eUnit = Gdiplus::UnitPoint;
		memset(m_Text, 0, sizeof(m_Text));

		WCHAR temp[] = L"ÆíÁýÃ¢À¸·Î ¹®ÀÚ¸¦ ÀÔ·ÂÇÏ¼¼¿ä.";

		memcpy(m_Text, temp, sizeof(temp));

		Property.bIsLine = FALSE;
		Property.bIsFill = TRUE;
	}

	ObjectText::~ObjectText()
	{
	}

	void ObjectText::SetText(WCHAR* pText, UINT nLength)
	{
		memset(m_Text, 0, sizeof(m_Text));
		if (sizeof(m_Text) < nLength)
			return;
		memcpy(m_Text, pText, nLength);
	}

	void ObjectText::SetColor(COLORREF color)
	{
		m_FontColor = color;
	}

	void ObjectText::SetSize(double dSize)
	{
		m_dFontSize = dSize;

		pt[1].SetXY(pt[1].X(), pt[0].Y() + (INT)(m_dFontSize / FONT_RESOLUTION));
	}

	void ObjectText::SetFontFamilyName(E_FAMILY_NAME_ eFontType)
	{
		m_eFamilyName = eFontType;
	}

	void ObjectText::SetFontStyle(Gdiplus::FontStyle eFontStyle)
	{
		m_eFontStyle = eFontStyle;
	}

	void ObjectText::SetFontUnit(Gdiplus::Unit eUnit)
	{
		m_eUnit = eUnit;
	}
	double				ObjectText::GetFontSize() { return m_dFontSize; }
	COLORREF			ObjectText::GetFontColor() { return m_FontColor; }
	E_FAMILY_NAME_		ObjectText::GetFamilyName() { return m_eFamilyName; }
	Gdiplus::FontStyle	ObjectText::GetFontStyle() { return m_eFontStyle; }
	Gdiplus::Unit		ObjectText::GetUnit() { return m_eUnit; }
	WCHAR* ObjectText::GetText() { return m_Text; }
	int					ObjectText::GetSize() { return sizeof(m_Text); }

}