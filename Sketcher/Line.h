#pragma once
#include "element.h"

class CLine :
	public CElement
{
DECLARE_SERIAL(CLine);
public:
	virtual ~CLine(void);
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement=nullptr) override;
	virtual void Move(const CSize& aSize) override;
	virtual void Serialize(CArchive& ar) override;

	CLine(const CPoint& start, const CPoint& end, COLORREF aColor, int penWidth);

protected:
	CPoint m_EndPoint;

protected:
	CLine(void);
};

