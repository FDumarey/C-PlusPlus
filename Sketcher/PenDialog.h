#pragma once
#include "Sketcher.h"

// CPenDialog dialog

class CPenDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPenDialog)

public:
	CPenDialog(CWnd* pParent = NULL); // standard constructor
	virtual ~CPenDialog();

// Dialog Data
	enum { IDD = IDD_PENWIDTH };

	int m_PenWidth;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPenwidth0();
	afx_msg void OnPenwidth1();
	afx_msg void OnPenwidth2();
	afx_msg void OnPenwidth3();
	afx_msg void OnPenwidth4();
	afx_msg void OnPenwidth5();
};
