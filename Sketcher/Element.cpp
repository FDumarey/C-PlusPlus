// Element.cpp : implementation file
//

#include "stdafx.h"
#include "Sketcher.h"
#include "Element.h"

IMPLEMENT_SERIAL(CElement, CObject, VERSION_NUMBER);

// CElement

CElement::CElement()
{
}

CElement::CElement(const CPoint& start, COLORREF color, int penWidth) : m_StartPoint(start), m_PenWidth(penWidth), m_Color(color) {}

CElement::~CElement()
{
}


// CElement member functions

void CElement::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_StartPoint
			<< m_PenWidth
			<< m_Color
			<< m_EnclosingRect;
	}
	else
	{
		ar >> m_StartPoint
			>> m_PenWidth
			>> m_Color
			>> m_EnclosingRect;
	}
}
