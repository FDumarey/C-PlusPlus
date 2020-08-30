// Dialog2.cpp : implementation file
//

#include "stdafx.h"
#include "WinHook.h"
#include "Dialog2.h"
#include "afxdialogex.h"


// CDialog2 dialog

IMPLEMENT_DYNAMIC(CDialog2, CDialogEx)

CDialog2::CDialog2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialog2::IDD, pParent)
{

}

CDialog2::~CDialog2()
{
}

void CDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialog2, CDialogEx)
END_MESSAGE_MAP()


// CDialog2 message handlers
