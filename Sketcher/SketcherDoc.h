
// SketcherDoc.h : interface of the CSketcherDoc class
//


#pragma once

#include "ElementType.h"
#include "ElementColor.h"
#include <list>
#include <memory>
#include "Element.h"
#include "PenDialog.h"

typedef std::list<std::shared_ptr<CElement>>::const_iterator SketchIterator;

class CSketcherDoc : public CDocument
{
protected: // create from serialization only
	CSketcherDoc();
	DECLARE_DYNCREATE(CSketcherDoc)

// Attributes
public:

// Operations
public:
	SketchIterator begin() const { return std::begin(m_Sketch); }
	SketchIterator end() const { return std::end(m_Sketch); }
	int GetPenWidth() const {return m_PenWidth; }

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CSketcherDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnColorBlack();
	afx_msg void OnColorRed();
	afx_msg void OnColorGreen();
	afx_msg void OnColorBlue();
	afx_msg void OnElementLine();
	afx_msg void OnElementRectangle();
	afx_msg void OnElementCircle();
	afx_msg void OnElementCurve();
	ElementType GetElementType() const {return m_Element; }
	ElementColor GetElementColor() const {return m_Color; }
	CSize GetDocSize() const {return m_DocSize; }
	CRect GetDocExtent() const;

	void SendToBack(std::shared_ptr<CElement>& pElement);
	
	void AddElement(std::shared_ptr<CElement>& pElement) 
	{ 
		m_Sketch.push_back(pElement);
		UpdateAllViews(nullptr, 0, pElement.get());
		SetModifiedFlag();
	}
	
	void DeleteElement(std::shared_ptr<CElement>& pElement) 
	{ 
		m_Sketch.remove(pElement);
		UpdateAllViews(nullptr, 0, pElement.get());
		SetModifiedFlag();
	}

	std::shared_ptr<CElement> FindElement(const CPoint& point)const
	{
		for(auto& pElement : m_Sketch)
		{
			if(pElement->GetEnclosingRect().PtInRect(point))
				return pElement;
		}
		return nullptr;
	}



protected:
	ElementType m_Element; // Current element type
	ElementColor m_Color; // Current element color
	std::list<std::shared_ptr<CElement>> m_Sketch; // A list containing the sketch
	int m_PenWidth; //Current pen width
	CSize m_DocSize; //Document size

public:
	afx_msg void OnUpdateColorBlack(CCmdUI *pCmdUI);
	afx_msg void OnUpdateColorRed(CCmdUI *pCmdUI);
	afx_msg void OnUpdateColorGreen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateColorBlue(CCmdUI *pCmdUI);
	afx_msg void OnUpdateElementLine(CCmdUI *pCmdUI);
	afx_msg void OnUpdateElementRectangle(CCmdUI *pCmdUI);
	afx_msg void OnUpdateElementCircle(CCmdUI *pCmdUI);
	afx_msg void OnUpdateElementCurve(CCmdUI *pCmdUI);
	afx_msg void OnPenWidth();
	afx_msg void OnElementText();
	afx_msg void OnUpdateElementText(CCmdUI *pCmdUI);
};
