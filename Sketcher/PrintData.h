#pragma once
#include "stdafx.h"

class CPrintData
{
public:
	UINT printWidth;
	UINT printLength;
	UINT m_nWidths;
	UINT m_nLenghts;
	CPoint m_DocRefPoint;
	CString m_DocTitle;

	CPrintData():
		printWidth(750),
		printLength(1000)
	{}
};
