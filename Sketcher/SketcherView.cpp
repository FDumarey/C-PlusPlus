
// SketcherView.cpp : implementation of the CSketcherView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Sketcher.h"
#endif

#include "SketcherDoc.h"
#include "SketcherView.h"
#include "Curve.h"
#include "Circle.h"
#include "Line.h"
#include "Rectangle.h"
#include "ScaleDialog.h"
#include "TextDialog.h"
#include "Text.h"
#include "PrintData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSketcherView

IMPLEMENT_DYNCREATE(CSketcherView, CScrollView)

BEGIN_MESSAGE_MAP(CSketcherView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ELEMENT_MOVE, &CSketcherView::OnElementMove)
	ON_COMMAND(ID_ELEMENT_DELETE, &CSketcherView::OnElementDelete)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_ELEMENT_SENDTOBA, &CSketcherView::OnElementSendtoba)
	ON_COMMAND(ID_VIEW_SCALE, &CSketcherView::OnViewScale)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCALE, &CSketcherView::OnUpdateScale)
END_MESSAGE_MAP()

// CSketcherView construction/destruction

CSketcherView::CSketcherView()
	: m_FirstPoint(CPoint(0,0))
	, m_SecondPoint(CPoint(0,0))
	, m_pSelected(nullptr)
	, m_CursorPos(CPoint(0,0)), m_FirstPos(CPoint(0,0))
	, m_MoveMode(false)
	, m_Scale("1")
{
	// TODO: add construction code here
	SetScrollSizes(MM_TEXT, CSize(0,0));

}

CSketcherView::~CSketcherView()
{
}

BOOL CSketcherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSketcherView drawing

void CSketcherView::OnDraw(CDC* pDC)
{
	CSketcherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	for (auto iter = pDoc->begin() ; iter != pDoc->end() ; iter++)
	{
		std::shared_ptr<CElement> pElement(*iter);
		if(pDC->RectVisible(pElement->GetEnclosingRect()))
			pElement->Draw(pDC, m_pSelected);
	}
}


// CSketcherView printing

BOOL CSketcherView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	CPrintData* printData(new CPrintData);
	CSketcherDoc* pDoc = GetDocument();
	CRect docExtent = pDoc->GetDocExtent();

	printData->m_DocRefPoint = docExtent.TopLeft();
	printData->m_DocTitle = pDoc->GetTitle();

	printData->m_nWidths = static_cast<UINT>(ceil(
		static_cast<double>(docExtent.Width())/printData->printWidth));

	printData->m_nLenghts = static_cast<UINT>(ceil(
		static_cast<double>(docExtent.Height())/printData->printLength));

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(printData->m_nWidths*printData->m_nLenghts);
	pInfo->m_lpUserData = printData;

	return DoPreparePrinting(pInfo);
}

void CSketcherView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSketcherView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
	delete static_cast<CPrintData*>(pInfo->m_lpUserData);
}


// CSketcherView diagnostics

#ifdef _DEBUG
void CSketcherView::AssertValid() const
{
	CView::AssertValid();
}

void CSketcherView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSketcherDoc* CSketcherView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSketcherDoc)));
	return (CSketcherDoc*)m_pDocument;
}
#endif //_DEBUG


// CSketcherView message handlers


void CSketcherView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(this == GetCapture())
		ReleaseCapture();

	if(m_pTempElement)
	{
		CRect aRect = m_pTempElement->GetEnclosingRect();
		GetDocument()->AddElement(m_pTempElement);
		CClientDC aDC(this);
		OnPrepareDC(&aDC);
		aDC.LPtoDP(aRect);
		InvalidateRect(aRect);
		m_pTempElement.reset();
	}


	CView::OnLButtonUp(nFlags, point);
}


void CSketcherView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	aDC.DPtoLP(&point);
	CSketcherDoc* pDoc = GetDocument();

	if(m_MoveMode)
	{
		m_MoveMode = false;
		auto pElement(m_pSelected);
		m_pSelected.reset();
		pDoc->UpdateAllViews(nullptr, 0, pElement.get());
		pDoc->SetModifiedFlag();
	}
	else if(pDoc->GetElementType() == ElementType::TEXT)
	{
		CTextDialog aDlg;
		if(aDlg.DoModal() == IDOK)
		{
			CSize textExtent = aDC.GetOutputTextExtent(aDlg.m_TextString);
			textExtent.cx *= _wtoi(m_Scale);
			textExtent.cy *= _wtoi(m_Scale);
			std::shared_ptr<CElement> pTextElement = std::make_shared<CText>(point, point + textExtent, aDlg.m_TextString, static_cast<COLORREF>(pDoc->GetElementColor()));
			pDoc->AddElement(pTextElement);
			pDoc->UpdateAllViews(nullptr, 0, pTextElement.get());
		}
	}
	else
	{

	m_FirstPoint = point; //record the cursor position
	SetCapture();
	}

	CView::OnLButtonDown(nFlags, point);
}


void CSketcherView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	aDC.DPtoLP(&point);

	if(m_MoveMode)
	{
		MoveElement(aDC, point);
	}
	else if((nFlags & MK_LBUTTON) && (this == GetCapture()))
	{
		m_SecondPoint = point;

		// test for a previous element
		if(m_pTempElement)
		{

			// delete old element
			if(ElementType::CURVE == GetDocument()->GetElementType())
			{
				std::static_pointer_cast<CCurve>(m_pTempElement)->AddSegment(m_SecondPoint);
				m_pTempElement->Draw(&aDC);
				return;
			}
			else
			{
				aDC.SetROP2(R2_NOTXORPEN);
				m_pTempElement->Draw(&aDC);
			}
		}
		m_pTempElement.reset(CreateElement());
		m_pTempElement->Draw(&aDC);
	}
	else
	{
		// we are not creating, so tracking
		auto pOldSelected(m_pSelected);
		m_pSelected = GetDocument()->FindElement(point);
		if(m_pSelected != pOldSelected)
		{
			if(m_pSelected)
				GetDocument()->UpdateAllViews(nullptr, 0, m_pSelected.get());
			if(pOldSelected)
				GetDocument()->UpdateAllViews(nullptr, 0, pOldSelected.get());
		}
	}

	// add code to create a new element

	CView::OnMouseMove(nFlags, point);
}

CElement* CSketcherView::CreateElement(void) const
{
	CSketcherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	COLORREF color = static_cast<COLORREF>(pDoc->GetElementColor());

	int penWidth(pDoc->GetPenWidth());

	switch (pDoc->GetElementType())
	{
	case ElementType::RECTANGLE:
		return new CRectangle(m_FirstPoint, m_SecondPoint, color, penWidth);

	case ElementType::CIRCLE:
		return new CCircle(m_FirstPoint, m_SecondPoint, color, penWidth);

	case ElementType::CURVE:
		return new CCurve(m_FirstPoint, m_SecondPoint, color, penWidth);

	case ElementType::LINE:
		return new CLine(m_FirstPoint, m_SecondPoint, color, penWidth);

	default:
		AfxMessageBox(_T("Bad Element code"), MB_OK);
		AfxAbort();
		return nullptr;
	}
}



void CSketcherView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pHint)
	{
		CClientDC aDC(this);
		OnPrepareDC(&aDC);
		CRect aRect = static_cast<CElement*>(pHint)->GetEnclosingRect();
		aDC.LPtoDP(aRect);
		InvalidateRect(aRect);
	}
	else
	{
		InvalidateRect(nullptr);
	}
}


void CSketcherView::OnInitialUpdate()
{
	ResetScrollSizes();
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	
}


void CSketcherView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenuW(IDR_CONTEXT_MENU); // load the context menu

	CMenu* pContext(nullptr); // if nothing selected, then return menu 2

	if(m_pSelected)
	{
		pContext = menu.GetSubMenu(0);
	}
	else
	{
		pContext = menu.GetSubMenu(1);

		ElementColor color = GetDocument()->GetElementColor();
		menu.CheckMenuItem(ID_COLOR_BLACK,(ElementColor::BLACK == color ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_RED,(ElementColor::RED == color ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_GREEN,(ElementColor::GREEN == color ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_COLOR_BLUE,(ElementColor::BLUE == color ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

		ElementType type = GetDocument()->GetElementType();
		menu.CheckMenuItem(ID_ELEMENT_LINE,(ElementType::LINE == type ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_RECTANGLE,(ElementType::RECTANGLE == type ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_CIRCLE,(ElementType::CIRCLE == type ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_CURVE,(ElementType::CURVE == type ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		menu.CheckMenuItem(ID_ELEMENT_TEXT,(ElementType::TEXT == type ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
	}
	
	ASSERT(pContext != nullptr); // assure there is a pointer in return

	pContext->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); // display popupmenu
	
}


void CSketcherView::OnElementMove()
{
	// TODO: Add your command handler code here
	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	GetCursorPos(&m_CursorPos);
	ScreenToClient(&m_CursorPos);
	aDC.DPtoLP(&m_CursorPos);
	m_FirstPos = m_CursorPos;
	m_MoveMode = true;
}


void CSketcherView::OnElementDelete()
{
	// TODO: Add your command handler code here
	if(m_pSelected)
	{
		GetDocument()->DeleteElement(m_pSelected);
		m_pSelected.reset();
	}

}

void CSketcherView::MoveElement(CClientDC& aDC, const CPoint& point)
{
	CSize distance = point - m_CursorPos;
	m_CursorPos = point;

	if(m_pSelected)
	{
		CSketcherDoc* pDoc = GetDocument();
		pDoc->UpdateAllViews(this, 0L, m_pSelected.get());

		if (typeid(*(m_pSelected.get())) == typeid(CText))
		{
			CRect oldRect = m_pSelected->GetEnclosingRect();
			aDC.LPtoDP(oldRect);
			m_pSelected->Move(distance);
			InvalidateRect(&oldRect);
			UpdateWindow();
			m_pSelected->Draw(&aDC, m_pSelected);
		}
		else
		{
			aDC.SetROP2(R2_NOTXORPEN);
			m_pSelected->Draw(&aDC, m_pSelected);
			m_pSelected->Move(distance);
			m_pSelected->Draw(&aDC, m_pSelected);
		}
		
		pDoc->UpdateAllViews(this, 0, m_pSelected.get());
	}
}


void CSketcherView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_MoveMode)
	{
		CClientDC aDC(this);
		OnPrepareDC(&aDC);
		MoveElement(aDC, m_FirstPos);
		m_pSelected.reset();
		GetDocument()->UpdateAllViews(nullptr);
	}

	CScrollView::OnRButtonDown(nFlags, point);
}


void CSketcherView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_MoveMode)
	{
		m_MoveMode = false;
	}
	else
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}
	
	CScrollView::OnRButtonUp(nFlags, point);
}


void CSketcherView::OnElementSendtoba()
{
	// TODO: Add your command handler code here
	GetDocument()->SendToBack(m_pSelected);

}


void CSketcherView::OnViewScale()
{
	// TODO: Add your command handler code here
	CScaleDialog aDlg;
	aDlg.m_Scale = m_Scale;
	if(aDlg.DoModal()==IDOK)
	{
		m_Scale = aDlg.m_Scale;
		ResetScrollSizes();
		InvalidateRect(nullptr);
	}
}


void CSketcherView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CScrollView::OnPrepareDC(pDC, pInfo);
	CSketcherDoc* pDoc = GetDocument();
	pDC->SetMapMode(MM_ANISOTROPIC);
	CSize DocSize = pDoc->GetDocSize();

	pDC->SetWindowExt(DocSize);

	int xLogPixels = pDC->GetDeviceCaps(LOGPIXELSX);
	int yLogPixels = pDC->GetDeviceCaps(LOGPIXELSY);
	int xyScale = _wtoi(m_Scale);

	int scale(pDC->IsPrinting()? 1 : xyScale);
	int xExtent = (DocSize.cx*scale*xLogPixels)/100;
	int yExtent = (DocSize.cy*scale*yLogPixels)/100;

	pDC->SetViewportExt(xExtent,yExtent);

}


void CSketcherView::ResetScrollSizes(void)
{
	CClientDC aDC(this);
	OnPrepareDC(&aDC);
	CSize DocSize = GetDocument()->GetDocSize();
	aDC.LPtoDP(&DocSize);
	SetScrollSizes(MM_TEXT, DocSize);

}


void CSketcherView::OnUpdateScale(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	CString scaleStr;
	scaleStr.Format(_T(" View Scale : %s"), m_Scale);
	pCmdUI->SetText(scaleStr);
}


void CSketcherView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CPrintData* p(static_cast<CPrintData*>(pInfo->m_lpUserData));

	pDC->SetTextAlign(TA_CENTER);
	pDC->TextOutW(pInfo->m_rectDraw.right/2, 20, p->m_DocTitle);
	CString str; str.Format(_T("Page %u"), pInfo->m_nCurPage);
	pDC->TextOutW(pInfo->m_rectDraw.right/2, pInfo->m_rectDraw.bottom-20, str);
	pDC->SetTextAlign(TA_LEFT);

	int xOrg = p->m_DocRefPoint.x + p->printWidth*((pInfo->m_nCurPage-1)%(p->m_nWidths));
	int yOrg = p->m_DocRefPoint.y + p->printLength*((pInfo->m_nCurPage-1)/(p->m_nWidths));

	int xOffset = (pInfo->m_rectDraw.right - p->printWidth)/2;
	int yOffset = (pInfo->m_rectDraw.bottom - p->printLength)/2;

	CPoint OldOrg = pDC->SetWindowOrg(xOrg - xOffset, yOrg - yOffset);

	pDC->IntersectClipRect(xOrg, yOrg, xOrg + p->printWidth, yOrg + p->printLength);

	OnDraw(pDC);

	pDC->SelectClipRgn(nullptr);
	pDC->SetWindowOrg(OldOrg);

}
