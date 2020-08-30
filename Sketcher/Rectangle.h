#pragma once
#include "element.h"

class CRectangle :
	public CElement
{
DECLARE_SERIAL(CRectangle);
public:
	virtual ~CRectangle(void);
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement=nullptr) override;
	virtual void Move(const CSize& aSize) override;
	virtual void Serialize(CArchive& ar) override;

	CRectangle(const CPoint& start, const CPoint& end, COLORREF color, int penWidth);

protected:
	CPoint m_BottomRight;
	CRectangle(void);
};

