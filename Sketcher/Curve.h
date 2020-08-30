#pragma once
#include "element.h"
#include <vector>

class CCurve :
	public CElement
{
DECLARE_SERIAL(CCurve);
public:
	virtual ~CCurve(void);

	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement=nullptr) override;
	virtual void Move(const CSize& aSize) override;
	virtual void Serialize(CArchive& ar) override;

	CCurve(const CPoint& first, const CPoint& second, COLORREF color, int penWidth);

	void AddSegment(const CPoint& point);

protected:
	std::vector<CPoint> m_Points;
	CCurve(void);
};

