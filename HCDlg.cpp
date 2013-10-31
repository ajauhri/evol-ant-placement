// HCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "HCDlg.h"
#include "afxdialogex.h"
#include "HC.h"
#include "AntennaConfiguration.h"

// HCDlg dialog

IMPLEMENT_DYNAMIC(HCDlg, CDialogEx)


#define DEFAULT_HC_ITERATIONS 20
#define DEFAULT_HC_SEED 4321
#define DEFAULT_HC_FREQ 5
#define DEFAULT_HC_GEN_COND 10
#define DEFAULT_HC_FITNESS_COND 1
#define DEFAULT_HC_MUTATIONS 0.001
#define DEFAULT_HC_CONVERGENCE 1.0
#define DEFAULT_HC_EXPWEIGHT 2.0f
#define DEFAULT_HC_MUTATION_TYPE 0

IntHCParam intHCParams[] =
{
// *valuePtr,              default-value,                       text-controller,        value, dirty
    {&(HCInfo.iterations),				DEFAULT_HC_ITERATIONS,		IDC_EDIT_HC_ITERATIONS},
    {&(HCInfo.seed),					DEFAULT_HC_SEED,			IDC_EDIT_HC_SEED},
    {&(HCInfo.outputFreq),				DEFAULT_HC_FREQ,			IDC_EDIT_HC_FREQ},
    {&(HCInfo.genCondition),			DEFAULT_HC_GEN_COND,		IDC_EDIT_HC_GEN_COND},
    {&(HCInfo.fitnessCondition),		DEFAULT_HC_FITNESS_COND,	IDC_EDIT_HC_FITNESS_COND},
	{&(HCInfo.mutationType),			DEFAULT_HC_MUTATION_TYPE, 0,	IDC_RADIO_HC_SM},
};
#define MAX_INT_HC_PARAMS 6

FloatHCParam floatHCParams[] =
{
// *valuePtr,              default-value,       min-value, max-value,  text-controller,    slider-controller, value, dirty
    {&(HCInfo.mutationProbability),		DEFAULT_HC_MUTATIONS,       IDC_EDIT_HC_MUTATIONS},
    {&(HCInfo.convergenceFactor),		DEFAULT_HC_CONVERGENCE,     IDC_EDIT_HC_CONVERGENCE},
    {&(HCInfo.expWeight),				DEFAULT_HC_EXPWEIGHT,		IDC_EDIT_HC_EXPWEIGHT},
};
#define MAX_FLOAT_HC_PARAMS 3

HCDlg::HCDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(HCDlg::IDD, pParent)
    , updating (false)
	, autogen(true)
	, ln_mutation(false)
	, use_default_resolution(true)
{
    this->Create(IDD_HC_DIALOG);

    ResetToDefaults();
    UpdateValues();
}

HCDlg::~HCDlg()
{
}

void HCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HCDlg, CDialogEx)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_ITERATIONS, &HCDlg::OnEnKillfocusEditHcIterations)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_MUTATIONS, &HCDlg::OnEnKillfocusEditHcMutations)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_SEED, &HCDlg::OnEnKillfocusEditHcSeed)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_FREQ, &HCDlg::OnEnKillfocusEditHcFreq)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_CONVERGENCE, &HCDlg::OnEnKillfocusEditHcConvergence)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_GEN_COND, &HCDlg::OnEnKillfocusEditHcGenCond)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_FITNESS_COND, &HCDlg::OnEnKillfocusEditHcFitnessCond)
    ON_BN_CLICKED(IDC_BUTTON_HC_APPLY, &HCDlg::OnBnClickedButtonHcApply)
    ON_EN_KILLFOCUS(IDC_EDIT_HC_EXPWEIGHT, &HCDlg::OnEnKillfocusEdiHcExpweight)
	ON_BN_CLICKED(IDC_RADIO_HC_SM, &HCDlg::OnBnClickedRadioHcSm)
	ON_BN_CLICKED(IDC_RADIO_HC_NM, &HCDlg::OnBnClickedRadioHcNm)
	ON_EN_KILLFOCUS(IDC_EDIT_HC_LNM, &HCDlg::OnEnKillfocusEditHcLnm)
	ON_BN_CLICKED(IDC_RADIO_HC_LNM, &HCDlg::OnBnClickedRadioHcLnm)
	ON_BN_CLICKED(IDC_CHECK_HC_LNM, &HCDlg::OnBnClickedCheckHcLnm)
	ON_BN_CLICKED(IDC_CHECK_HC_AUTOGEN, &HCDlg::OnBnClickedCheckHcAutogen)
	ON_BN_CLICKED(IDC_BUTTON_HC_GENERATE, &HCDlg::OnBnClickedButtonHcGenerate)
END_MESSAGE_MAP()


// HCDlg message handlers


void 
HCDlg::OnEnKillfocusEditHcIterations()
{
    this->handleEditChange(IDC_EDIT_HC_ITERATIONS);
}


void 
HCDlg::OnEnKillfocusEditHcMutations()
{
    this->handleEditChangeFloat(IDC_EDIT_HC_MUTATIONS);
}


void 
HCDlg::OnEnKillfocusEditHcSeed()
{
    this->handleEditChange(IDC_EDIT_HC_SEED);
}


void 
HCDlg::OnEnKillfocusEditHcFreq()
{
    this->handleEditChange(IDC_EDIT_HC_FREQ);
}


void 
HCDlg::OnEnKillfocusEditHcConvergence()
{
    this->handleEditChangeFloat(IDC_EDIT_HC_CONVERGENCE);
}


void 
HCDlg::OnEnKillfocusEditHcGenCond()
{
    this->handleEditChange(IDC_EDIT_HC_GEN_COND);
}


void 
HCDlg::OnEnKillfocusEditHcFitnessCond()
{
    this->handleEditChange(IDC_EDIT_HC_FITNESS_COND);
}


void 
HCDlg::OnEnKillfocusEdiHcExpweight()
{
    this->handleEditChangeFloat(IDC_EDIT_HC_EXPWEIGHT);
}



BOOL 
HCDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void 
HCDlg::OnOK()
{
    ApplyValues();
    CDialogEx::OnOK();
}




void 
HCDlg::ApplyValues()
{
    for (int i = 0; i < MAX_INT_HC_PARAMS; i++)
    {
        *(intHCParams[i].valuePtr) = intHCParams[i].value;
    }
    for (int i = 0; i < MAX_FLOAT_HC_PARAMS; i++)
    {
        *(floatHCParams[i].valuePtr) =  floatHCParams[i].value;
    }
	HCInfo.autoSeed = autogen;
}

void 
HCDlg::UpdateValues(void)
{

    CString buffer;
    updating = TRUE;
    for (int i = 0; i < MAX_INT_HC_PARAMS; i++)
    {
        IntHCParam *p = intHCParams + i;
        if (p->dirty)
        {
            unsigned int value = p->value;
            CEdit* edit = (CEdit *) GetDlgItem(p->textControl);
            if (edit)
            {
                buffer.Format("%d", value);
                edit->SetWindowText(buffer);
            }
            p->dirty = FALSE;
        }
    }

    for (int i = 0; i < MAX_FLOAT_HC_PARAMS; i++)
    {
        FloatHCParam *p = floatHCParams + i;
        if (p->dirty)
        {
            float value = p->value;
            buffer.Format("%3.3f", value);
            GetDlgItem(p->textControl)->SetWindowText(buffer);
            p->dirty = FALSE;
        }
    }
	 // Handle autogen check box.
    ((CButton*) GetDlgItem(IDC_CHECK_HC_AUTOGEN))->SetCheck(autogen ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_EDIT_HC_SEED)->EnableWindow(!autogen);
    GetDlgItem(IDC_BUTTON_HC_GENERATE)->EnableWindow(!autogen);

	HandleMutationType((getIntParam(IDC_RADIO_HC_SM)->value));
	
	// Handle local neighborhood mutation
	GetDlgItem(IDC_CHECK_HC_LNM)->EnableWindow(ln_mutation);
	((CButton*) GetDlgItem(IDC_CHECK_HC_LNM))->SetCheck(use_default_resolution);
	GetDlgItem(IDC_EDIT_HC_LNM)->EnableWindow(!use_default_resolution & ln_mutation);
	mesh_resolution = use_default_resolution ? default_mesh_resolution : mesh_resolution;
	buffer.Format("%f", mesh_resolution);
	GetDlgItem(IDC_EDIT_HC_LNM)->SetWindowText(buffer);

    updating = FALSE;
}


void 
HCDlg::HandleMutationType(unsigned int type)
{
	if (type == SIMPLE_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_HC_SM))->SetCheck(BST_CHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_HC_NM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_HC_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == GLOBAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_HC_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_HC_NM))->SetCheck(BST_CHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_HC_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == LOCAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_HC_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_HC_NM))->SetCheck(BST_UNCHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_HC_LNM))->SetCheck(BST_CHECKED);
		ln_mutation = true;
	}	
}


void 
HCDlg::ResetToDefaults(void)
{
    for (int i = 0; i < MAX_INT_HC_PARAMS; i++)
    {
        intHCParams[i].value = intHCParams[i].defaultValue;
        intHCParams[i].dirty = TRUE;
    }

    for (int i = 0; i < MAX_FLOAT_HC_PARAMS; i++)
    {
        floatHCParams[i].value = floatHCParams[i].defaultValue;
        floatHCParams[i].dirty = TRUE;
    }
}

void
HCDlg::ReadValues(void)
{
    for (int i = 0; i < MAX_INT_HC_PARAMS; i++)
    {
        intHCParams[i].value = *(intHCParams[i].valuePtr);
        intHCParams[i].dirty = TRUE;
    }
    for (int i = 0; i < MAX_FLOAT_HC_PARAMS; i++)
    {
        floatHCParams[i].value = *(floatHCParams[i].valuePtr);
        floatHCParams[i].dirty = TRUE;
    }
}


void 
HCDlg::handleEditChange(int editId)
{
    if (updating) return;

    IntHCParam* p = this->getIntParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = atoi(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}


IntHCParam*
HCDlg::getIntParam(int editId)
{
    for (int i = 0; i < MAX_INT_HC_PARAMS; i++)
    {
        if (intHCParams[i].textControl == editId
			|| intHCParams[i].otherControl == editId)
        {
            return intHCParams + i;
        }
    }
    return NULL;
}


void 
HCDlg::handleEditChangeFloat(int editId)
{
    if (updating) return;

    FloatHCParam* p = this->getFloatParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = (float) atof(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}

FloatHCParam* HCDlg::getFloatParam(int editId)
{
    for (int i = 0; i < MAX_FLOAT_HC_PARAMS; i++)
    {
        if (floatHCParams[i].textControl == editId )
        {
            return floatHCParams + i;
        }
    }
    return NULL;
}


void
HCDlg::OnBnClickedButtonHcApply()
{
    ApplyValues();
}

void 
HCDlg::OnBnClickedRadioHcSm()
{
	IntHCParam* mutationType = getIntParam(IDC_RADIO_HC_SM);
    mutationType->value = SIMPLE_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}


void 
HCDlg::OnBnClickedRadioHcNm()
{
	IntHCParam* mutationType = getIntParam(IDC_RADIO_HC_SM);
    mutationType->value = GLOBAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void 
HCDlg::OnBnClickedRadioHcLnm()
{
	IntHCParam* mutationType = getIntParam(IDC_RADIO_HC_SM);
    mutationType->value = LOCAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void 
HCDlg::OnEnKillfocusEditHcLnm()
{
	CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_HC_LNM))->GetWindowText(buffer);
    mesh_resolution = (float) atof(buffer);
}



void 
HCDlg::OnBnClickedCheckHcLnm()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_HC_LNM);
    use_default_resolution = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}


void HCDlg::OnBnClickedCheckHcAutogen()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_HC_AUTOGEN);
    autogen = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}


void HCDlg::OnBnClickedButtonHcGenerate()
{
	IntHCParam* p = getIntParam(IDC_EDIT_HC_SEED);
    p->value = (unsigned int) time(NULL);
    p->dirty = TRUE;
    UpdateValues();
}
