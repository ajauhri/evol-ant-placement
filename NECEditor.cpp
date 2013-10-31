// NECEditor.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "NECEditor.h"
#include "Geometry.h"

extern antenna*			platform;

// CNECEditor dialog

IMPLEMENT_DYNAMIC(CNECEditor, CDialog)

CNECEditor::CNECEditor(CWnd* pParent /*=NULL*/)
    : CDialog(CNECEditor::IDD, pParent)
{
    this->Create(IDD_EDITOR_DIALOG);
}

CNECEditor::~CNECEditor()
{
}

void CNECEditor::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNECEditor, CDialog)
    ON_BN_CLICKED(IDOK, &CNECEditor::OnBnClickedOk)
END_MESSAGE_MAP()


// CNECEditor message handlers

void CNECEditor::UpdateText(void)
{
    CEdit* editField = (CEdit*) GetDlgItem(IDC_EDIT_NEC);

    CString buffer;
    int excitation;
    point offset;

    //	build the nec deck
    clear_nec_deck();
    platform->translate2deck(offset);
    // convert genotype into an antenna offset
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {

        if (ii == 0) excitation = g_nec_segment;
        antennas[ii]->translate2deck(*(antennas[ii]->points->at(0)));
    }
    if (antennas.size() > 0) antennas[0]->append_analysis(excitation);

    for (int ii = 0; ii < g_nec_line; ii++)
    {
        buffer.AppendFormat("%s\r\n", g_nec_deck[ii]);
    }
    editField->SetWindowText(buffer);

    /*	else {
    		for (unsigned int ii = 0; ii < CONTROL_COUNT; ii++) {
    			platform->cparams.card[ii][0] = 0;
    		}
    		editField->GetWindowText(cmd);
    		char* pch = strtok(cmd.GetBuffer(),"\r\n");
    		while (pch != NULL) {
    			for (unsigned int ii = 0; ii < CONTROL_COUNT; ii++) {
    				if (!strncmp(pch, sControlCodes[ii], 2)) {
    					strcpy(platform->cparams.card[ii], pch);
    					break;
    				}
    			}
    		    pch = strtok (NULL, "\r\n");
    		}
    	}
    */
}

void CNECEditor::OnBnClickedOk()
{
    ShowWindow(SW_HIDE);
//	UpdateText(FALSE);
}

BOOL CNECEditor::OnInitDialog()
{
    CDialog::OnInitDialog();

    CEdit* editField = (CEdit*) GetDlgItem(IDC_EDIT_NEC);
    /*	CFont font;
    	font.CreateFont(
    	   9,                         // nHeight
    	   0,                         // nWidth
    	   0,                         // nEscapement
    	   0,                         // nOrientation
    	   FW_LIGHT,                 // nWeight
    	   FALSE,                     // bItalic
    	   FALSE,                     // bUnderline
    	   0,                         // cStrikeOut
    	   ANSI_CHARSET,              // nCharSet
    	   OUT_DEFAULT_PRECIS,        // nOutPrecision
    	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
    	   DEFAULT_QUALITY,           // nQuality
    	   DEFAULT_PITCH | FF_MODERN,  // nPitchAndFamily
    	   NULL);                // lpszFacename

    	editField->SetFont(&font);
    	font.DeleteObject();

    	LOGFONT LogFont;
    	editField->GetFont()->GetLogFont(&LogFont);
    	LogFont.lfFaceName = "Courier";
    	editField->SetFont(&LogFont);
    */
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
