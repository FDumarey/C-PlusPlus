#include "stdafx.h"
#include "Text.h"

IMPLEMENT_SERIAL(CText, CObject, VERSION_NUMBER);

CText::CText(const CPoint& start, const CPoint& end, const CString& aString,COLORREF color): CElement(start, color)
{
	m_String = aString;
	m_EnclosingRect = CRect(start, end);
	m_EnclosingRect.NormalizeRect();
	m_EnclosingRect.InflateRect(m_PenWidth, m_PenWidth);
}

void CText::Draw(CDC* pDC, std::shared_ptr<CElement> pElement)
{
	pDC->SetTextColor(this == pElement.get() ? SELECT_COLOR : m_Color);
	pDC->SetBkMode(TRANSPARENT);
	pDC->ExtTextOutW(m_StartPoint.x, m_StartPoint.y, 0, nullptr, m_String, nullptr);
}

void CText::Move(const CSize& size)
{
	m_EnclosingRect += size;
	m_StartPoint += size;
}

void CText::Serialize(CArchive& ar)
{
	CElement::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_String;
	}
	else
	{
		ar >> m_String;
	}
}


