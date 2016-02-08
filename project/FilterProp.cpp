// FilterProp.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "FilterProp.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterProp dialog


CFilterProp::CFilterProp(stConfig *poptions, CMpegMovie *pmovie, CWnd* pParent/*=NULL*/)
	: CDialog(CFilterProp::IDD, pParent),
		pOptions(poptions),
		pMovie(pmovie),
		pGB(NULL)
{
	//{{AFX_DATA_INIT(CFilterProp)
	//}}AFX_DATA_INIT
	if ( pMovie ) pGB = pMovie->GetGraphBuilder();
	pOptionsTemp = new stConfig;
	memcpy( pOptionsTemp, pOptions, sizeof(stConfig) );
}

CFilterProp::~CFilterProp()
{
	delete pOptionsTemp;
}

void CFilterProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterProp)
	DDX_Control(pDX, IDC_BUTTON_DEF, m_BDefault);
	DDX_Control(pDX, IDCANCEL, m_BCancel);
	DDX_Control(pDX, IDOK, m_BOK);
	DDX_Control(pDX, IDC_TEXTT_WINXPONLY, m_TTWinXPOnly);
	DDX_Control(pDX, IDC_TEXTT_TITLESFILTERS, m_TTTitleFilters);
	DDX_Control(pDX, IDC_TEXTT_TITLEPICTURE, m_TTTitlePicture);
	DDX_Control(pDX, IDC_TEXTT_SATURATION, m_TTSaturation);
	DDX_Control(pDX, IDC_TEXTT_CONTRAST, m_TTContrast);
	DDX_Control(pDX, IDC_TEXTT_BRIGHTNESS, m_TTBrightness);
	DDX_Control(pDX, IDAPPLY, m_Apply);
	DDX_Control(pDX, IDC_STATIC_SATURATION, m_stcSaturation);
	DDX_Control(pDX, IDC_STATIC_CONTRAST, m_stcContrast);
	DDX_Control(pDX, IDC_STATIC_BRIGHTNESS, m_stcBrightness);
	DDX_Control(pDX, IDC_SLIDER_SATURATION, m_Saturation);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_Contrast);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_Brightness);
	DDX_Control(pDX, IDC_BUTTON_PROP, m_ButtonProp);
	DDX_Control(pDX, IDC_LIST_FILTERS, m_ListFilters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterProp, CDialog)
	//{{AFX_MSG_MAP(CFilterProp)
	ON_BN_CLICKED(IDC_BUTTON_PROP, OnButtonProp)
	ON_LBN_DBLCLK(IDC_LIST_FILTERS, OnDblclkListFilters)
	ON_LBN_SELCHANGE(IDC_LIST_FILTERS, OnSelchangeListFilters)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_DEF, OnButtonDef)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterProp message handlers

HRESULT CFilterProp::EnumFilters()
{
	if ( !pGB ) return E_FAIL;

    HRESULT hr;
    IEnumFilters *pEnum = NULL;
    IBaseFilter *pFilter = NULL;
    ULONG cFetched;

    // Clear filters list box
    m_ListFilters.ResetContent();
    
    // Get filter enumerator
    hr = pGB->EnumFilters(&pEnum);
    if (FAILED(hr))
    {
        m_ListFilters.AddString(TEXT("<ERROR>"));
        return hr;
    }

    // Enumerate all filters in the graph
    while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
    {
        FILTER_INFO FilterInfo;
        TCHAR szName[256];
        
        hr = pFilter->QueryFilterInfo(&FilterInfo);
        if (FAILED(hr))
        {
            m_ListFilters.AddString(TEXT("<ERROR>"));
        }
        else
        {
				ISpecifyPropertyPages *pSpecify;
				hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
				if (SUCCEEDED(hr)) 
				{
	            pSpecify->Release();

					// Add the filter name to the filters listbox
					USES_CONVERSION;

					lstrcpy(szName, W2T(FilterInfo.achName));
					m_ListFilters.AddString(szName);

				}
				FilterInfo.pGraph->Release();
        }       
        pFilter->Release();
    }
    pEnum->Release();

    return hr;
}
IBaseFilter *CFilterProp::FindFilterFromName(LPTSTR szNameToFind)
{
	if ( !pGB ) return NULL;

    USES_CONVERSION;

    HRESULT hr;
    IEnumFilters *pEnum = NULL;
    IBaseFilter *pFilter = NULL;
    ULONG cFetched;
    BOOL bFound = FALSE;

    // Get filter enumerator
    hr = pGB->EnumFilters(&pEnum);
    if (FAILED(hr))
        return NULL;

    // Enumerate all filters in the graph
    while((pEnum->Next(1, &pFilter, &cFetched) == S_OK) && (!bFound))
    {
        FILTER_INFO FilterInfo;
        TCHAR szName[256];
        
        hr = pFilter->QueryFilterInfo(&FilterInfo);
        if (FAILED(hr))
        {
            pFilter->Release();
            pEnum->Release();
            return NULL;
        }

        // Compare this filter's name with the one we want
        lstrcpy(szName, W2T(FilterInfo.achName));
        if (! lstrcmp(szName, szNameToFind))
        {
            bFound = TRUE;
        }

        FilterInfo.pGraph->Release();

        // If we found the right filter, don't release its interface.
        // The caller will use it and release it later.
        if (!bFound)
            pFilter->Release();
        else
            break;
    }
    pEnum->Release();

    return (bFound ? pFilter : NULL);
}
void CFilterProp::ShowProperties()
{
	if ( !pGB ) return;

    HRESULT hr;
    IBaseFilter *pFilter = NULL;
    TCHAR szNameToFind[128];
    ISpecifyPropertyPages *pSpecify;

    // Read the current filter name from the list box
    int nCurSel = m_ListFilters.GetCurSel();
    m_ListFilters.GetText(nCurSel, szNameToFind);

    // Read the current list box name and find it in the graph
    pFilter = FindFilterFromName(szNameToFind);
    if (!pFilter)
        return;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        do 
        {
            FILTER_INFO FilterInfo;
            hr = pFilter->QueryFilterInfo(&FilterInfo);
            if (FAILED(hr))
                break;

            CAUUID caGUID;
            hr = pSpecify->GetPages(&caGUID);
            if (FAILED(hr))
                break;

            pSpecify->Release();
        
            // Display the filter's property page
            OleCreatePropertyFrame(
                m_hWnd,                 // Parent window
                0,                      // x (Reserved)
                0,                      // y (Reserved)
                FilterInfo.achName,     // Caption for the dialog box
                1,                      // Number of filters
                (IUnknown **)&pFilter,  // Pointer to the filter 
                caGUID.cElems,          // Number of property pages
                caGUID.pElems,          // Pointer to property page CLSIDs
                0,                      // Locale identifier
                0,                      // Reserved
                NULL                    // Reserved
            );
            CoTaskMemFree(caGUID.pElems);
            FilterInfo.pGraph->Release(); 

        } while(0);
    }

    pFilter->Release();

}


void CFilterProp::OnButtonProp() 
{
	ShowProperties();	
}


void CFilterProp::OnDblclkListFilters() 
{
	ShowProperties();	
}

void CFilterProp::OnSelchangeListFilters() 
{
	m_ButtonProp.EnableWindow(TRUE);	
}


BOOL CFilterProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitLanguage(); 

	m_ListFilters.ResetContent();

	// Enumarate filters with property page.
	EnumFilters();

	if ( m_ListFilters.GetCurSel()==LB_ERR )
		m_ButtonProp.EnableWindow(FALSE);
	else 
		m_ButtonProp.EnableWindow(TRUE);
	
	m_Brightness.SetTicFreq(10); 
	m_Brightness.SetPageSize(10);
	m_Brightness.SetPos(pOptions->valBrightness);

	m_Contrast.SetTicFreq(10); 
	m_Contrast.SetPageSize(10);
	m_Contrast.SetPos(pOptions->valContrast);

	m_Saturation.SetTicFreq(10); 
	m_Saturation.SetPageSize(10);
	m_Saturation.SetPos(pOptions->valSaturation);


	char sz[32];

	sprintf(sz," %i\n[IRE]", m_Brightness.GetPos() );
	m_stcBrightness.SetWindowText(sz);

	sprintf(sz," %i\n[IRE]", m_Contrast.GetPos() );
	m_stcContrast.SetWindowText(sz);

	sprintf(sz," %i\n[IRE]", m_Saturation.GetPos() );
	m_stcSaturation.SetWindowText(sz);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterProp::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	char sz[32];

	sprintf(sz," %i\n[IRE]", m_Brightness.GetPos() );
	m_stcBrightness.SetWindowText(sz);

	sprintf(sz," %i\n[IRE]", m_Contrast.GetPos() );
	m_stcContrast.SetWindowText(sz);

	sprintf(sz," %i\n[IRE]", m_Saturation.GetPos() );
	m_stcSaturation.SetWindowText(sz);

	SetColorControl();

	m_Apply.EnableWindow(TRUE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFilterProp::OnButtonDef() 
{
	m_Brightness.SetPos(0);
	m_stcBrightness.SetWindowText(" 0\n[IRE]");

	m_Contrast.SetPos(50);
	m_stcContrast.SetWindowText(" 50\n[IRE]");

	m_Saturation.SetPos(50);
	m_stcSaturation.SetWindowText(" 50\n[IRE]");	

	SetColorControl();

	m_Apply.EnableWindow(TRUE);
}

void CFilterProp::SetColorControl()
{
	pOptionsTemp->valBrightness = m_Brightness.GetPos();
	pOptionsTemp->valContrast = m_Contrast.GetPos();
	pOptionsTemp->valSaturation = m_Saturation.GetPos();

	if ( pMovie )
	{
		pMovie->SetColorControl( pOptionsTemp->valBrightness, 
			pOptionsTemp->valContrast, pOptionsTemp->valSaturation );
	}
}

void CFilterProp::OnOK() 
{
	OnApply();
	
	CDialog::OnOK();
}

void CFilterProp::OnCancel() 
{
	if ( pMovie )
	{
		pMovie->SetColorControl( pOptions->valBrightness, 
			pOptions->valContrast, pOptions->valSaturation );
	}
	
	CDialog::OnCancel();
}

void CFilterProp::OnApply() 
{
	m_Apply.EnableWindow(FALSE);
	memcpy( pOptions, pOptionsTemp, sizeof(stConfig) );
}

void CFilterProp::InitLanguage()
{

	int sect = TRANSL_FILTERPROP;
	SetWindowText( transl(IDD_DIALOG_FILTERSPROP, sect) );
	TRANSLW( m_TTTitlePicture, IDC_TEXTT_TITLEPICTURE, sect );
	TRANSLW( m_TTWinXPOnly, IDC_TEXTT_WINXPONLY, sect );
	TRANSLW( m_TTBrightness, IDC_TEXTT_BRIGHTNESS, sect );
	TRANSLW( m_TTContrast, IDC_TEXTT_CONTRAST, sect );
	TRANSLW( m_TTSaturation, IDC_TEXTT_SATURATION, sect );
	TRANSLW( m_TTTitleFilters, IDC_TEXTT_TITLESFILTERS, sect );
	TRANSLW( m_ButtonProp, IDC_BUTTON_PROP, sect );
	TRANSLW( m_BOK, IDOK, sect );
	TRANSLW( m_BCancel, IDCANCEL, sect );


	TRANSLW( m_Apply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );

}

BOOL CFilterProp::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		case IDM_CALLBACK_LANGUAGECHANGED:
			InitLanguage();
			return TRUE;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}
