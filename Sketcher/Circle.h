#pragma once
#include "element.h"

class CCircle :
	public CElement
{
DECLARE_SERIAL(CCircle);
public:
	virtual ~CCircle(void);
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement=nullptr) override;
	virtual void Move(const CSize& aSize) override;
	virtual void Serialize(CArchive& ar) override;

	CCircle(const CPoint& start, const CPoint& end, COLORREF color, int penWidth);

protected:
	CPoint m_BottomRight;
	CCircle(void);
};

