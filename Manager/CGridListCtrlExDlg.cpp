// CGridListCtrlExDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGridListCtrlExApp.h"
#include "CGridListCtrlExDlg.h"


#include "..\CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include "..\CGridListCtrlEx\CGridColumnTraitEdit.h"
#include "..\CGridListCtrlEx\CGridColumnTraitCombo.h"
#include "..\CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include "..\CGridListCtrlEx\CGridRowTraitXP.h"
#include "..\CGridListCtrlEx\ViewConfigSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

private:
	CAboutDlg(const CAboutDlg&);
	CAboutDlg& operator=(const CAboutDlg&);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGridListCtrlExDlg dialog



CGridListCtrlExDlg::CGridListCtrlExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridListCtrlExDlg::IDD, pParent)
{
    m_TimeoutSeconds = 30;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGridListCtrlExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CGridListCtrlExDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


VOID CGridListCtrlExDlg::ClearListView()
{
    while (m_ListCtrl.GetItemCount())
        m_ListCtrl.DeleteItem(0);
}

VOID CGridListCtrlExDlg::UpdateListView()
{
	// Insert data into list-control by copying from datamodel
	int nItem = 0;
	for(size_t rowId = 0; rowId < m_DataModel.GetRowIds() ; ++rowId)
	{
		nItem = m_ListCtrl.InsertItem(++nItem, m_DataModel.GetCellText(rowId,0));
		m_ListCtrl.SetItemData(nItem, rowId);
		for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
		{
			int nCellCol = col+1;	// +1 because of hidden column
			const CString& strCellText = m_DataModel.GetCellText(rowId, col);
			m_ListCtrl.SetItemText(nItem, nCellCol, strCellText);
			if (nCellCol==3)
			{
                CGridColumnTraitCombo* pComboTrait = (CGridColumnTraitCombo*)
                                m_ListCtrl.GetCellColumnTrait(rowId, nCellCol);
				if (strCellText==_T("")) {
					m_ListCtrl.SetCellImage(nItem, nCellCol, m_nStateImageIdx);	    // unchecked
				} else {
					m_ListCtrl.SetCellImage(nItem, nCellCol, m_nStateImageIdx + 1);	// checked
                }
			}
		}
	}
}

// CGridListCtrlExDlg message handlers

BOOL CGridListCtrlExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		VERIFY(strAboutMenu.LoadString(IDS_ABOUTBOX));
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Create and attach image list
	m_ImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 1, 0);
	m_nStateImageIdx =CGridColumnTraitCombo::AppendStateImages(m_ListCtrl, m_ImageList);	// Add checkboxes

	// Give better margin to editors
	m_ListCtrl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
	m_ListCtrl.SetDefaultRowTrait(pRowTrait);
	m_ListCtrl.EnableVisualStyles(true);

	// Create Columns
	m_ListCtrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
	{
		const CString& title = m_DataModel.GetColTitle(col);
		CGridColumnTrait* pTrait = NULL;
		if (col==0)	// Country
		{
			pTrait = new CGridColumnTraitText;
		}
		if (col==1)	// City
		{
			pTrait = new CGridColumnTraitText;
		}
		if (col==2)	// state
		{
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;

		    pComboTrait->AddItem((DWORD_PTR)0, _T("Persistent"));
		    pComboTrait->AddItem((DWORD_PTR)1, _T("Temporary"));

			pComboTrait->AddImageIndex(m_nStateImageIdx, _T(""), false);		// Unchecked (and not editable)
			pComboTrait->AddImageIndex(m_nStateImageIdx+1,_T("Persistent") , true);	// Checked (and editable)
            pComboTrait->SetToggleSelection(true);
			pTrait = pComboTrait;
		}
		if (col==3)	// Year won
		{
			pTrait = new CGridColumnTraitText;
		}

		m_ListCtrl.InsertColumnTrait(col+1, title, LVCFMT_LEFT, 100, col, pTrait);
	}

    UpdateListView();

	CViewConfigSectionWinApp* pColumnProfile = new CViewConfigSectionWinApp(_T("Sample List"));
	pColumnProfile->AddProfile(_T("Default"));
	pColumnProfile->AddProfile(_T("Special"));
	m_ListCtrl.SetupColumnConfig(pColumnProfile);

    SetTimer('STAT', 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGridListCtrlExDlg::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == 'STAT' && ++m_TickCount > m_TimeoutSeconds) {
        m_DataModel.InitDataModel();
        ClearListView();
        UpdateListView();
        m_TickCount = 0;
    }

	CDialog::OnTimer(nIDEvent);
}


void CGridListCtrlExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGridListCtrlExDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(static_cast<CWnd*>(this)); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGridListCtrlExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CMoureListCtrlGroups::OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI)
{
	bool rc;

	rc = CGridListCtrlGroups::OnEditComplete(nRow, nCol, pEditor, pLVDI);

    if (nCol != 3)
        goto errorout;

	if ((pLVDI->item.mask & LVIF_TEXT) && (pLVDI->item.pszText != NULL)) {

        CGridListCtrlExDlg *dlg = (CGridListCtrlExDlg *)GetParent();
        dlg->m_DataModel.UpdateMouseState(nRow, pLVDI->item.pszText);
        dlg->m_TickCount += dlg->m_TimeoutSeconds;
	}

errorout:
	return rc;
}