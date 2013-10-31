// ESDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "ESDlg.h"
#include "afxdialogex.h"
#include "EvolutionaryStrategy.h"
#include "AntennaConfiguration.h"



// ESDlg dialog

IMPLEMENT_DYNAMIC(ESDlg, CDialogEx)


#define DEFAULT_ES_GENERATIONS 10
#define DEFAULT_ES_MUTATION 0.001
#define DEFAULT_ES_LAMBDA 7
#define DEFAULT_ES_MU 1
#define DEFAULT_ES_SEED 0
#define DEFAULT_ES_EXPWEIGHT 2.0f
#define DEFAULT_ES_MUTATION_TYPE 0

IntESParam intESParams[] =
{
// *valuePtr,              default-value,                       text-controller,        value, dirty
    {&(ESInfo.generations),			DEFAULT_ES_GENERATIONS,		IDC_EDIT_ES_GENERATIONS},
    {&(ESInfo.lambda),				DEFAULT_ES_LAMBDA,			IDC_EDIT_ES_LAMBDA},
    {&(ESInfo.seed),				DEFAULT_ES_SEED,            IDC_EDIT_ES_SEED},
    {&(ESInfo.mu),					DEFAULT_ES_MU,				IDC_EDIT_ES_MU},
	{&(ESInfo.mutationType),		DEFAULT_ES_MUTATION_TYPE,	0,		IDC_RADIO_ES_SM},
};
#define MAX_INT_ES_PARAMS 5

FloatESParam floatESParams[] =
{
// *valuePtr,              default-value,       min-value, max-value,  text-controller,    slider-controller, value, dirty
    {&(ESInfo.mutation_probability),		DEFAULT_ES_MUTATION,    IDC_EDIT_ES_MUTATION},
    {&(ESInfo.expWeight),					DEFAULT_ES_EXPWEIGHT,		IDC_EDIT_ES_EXPWEIGHT},
};
#define MAX_FLOAT_ES_PARAMS 2

ESDlg::ESDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(ESDlg::IDD, pParent)
    , updating (false)
    , autogen (true)
	, ln_mutation(false)
	, use_default_resolution(true)
{
    this->Create(IDD_ES_DIALOG);

    ResetToDefaults();
    UpdateValues();
}

ESDlg::~ESDlg()
{
}

void 
ESDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ESDlg, CDialogEx)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_EXPWEIGHT, &ESDlg::OnEnKillfocusEditEsExpweight)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_MUTATION, &ESDlg::OnEnKillfocusEditEsMutation)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_LAMBDA, &ESDlg::OnEnKillfocusEditEsLambda)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_MU, &ESDlg::OnEnKillfocusEditEsMu)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_GENERATIONS, &ESDlg::OnEnKillfocusEditEsGenerations)
    ON_BN_CLICKED(IDC_BUTTON_ES_APPLY, &ESDlg::OnBnClickedButtonEsApply)
    ON_BN_CLICKED(IDC_CHECK_ES_AUTOGEN, &ESDlg::OnBnClickedCheckEsAutogen)
    ON_BN_CLICKED(IDC_BUTTON_ES_GENERATE, &ESDlg::OnBnClickedButtonEsGenerate)
    ON_EN_KILLFOCUS(IDC_EDIT_ES_SEED, &ESDlg::OnEnKillfocusEditEsSeed)
	ON_BN_CLICKED(IDC_RADIO_ES_SM, &ESDlg::OnBnClickedRadioEsSm)
	ON_BN_CLICKED(IDC_RADIO_ES_NM, &ESDlg::OnBnClickedRadioEsNm)
	ON_BN_CLICKED(IDC_RADIO_ES_LNM, &ESDlg::OnBnClickedRadioEsLnm)
	ON_BN_CLICKED(IDC_CHECK_ES_LNM, &ESDlg::OnBnClickedCheckEsLnm)
	ON_EN_KILLFOCUS(IDC_EDIT_ES_LNM, &ESDlg::OnEnKillfocusEditEsLnm)
END_MESSAGE_MAP()


// ESDlg message handlers


void 
ESDlg::OnEnKillfocusEditEsExpweight()
{
    this->handleEditChangeFloat(IDC_EDIT_ES_EXPWEIGHT);
}


void 
ESDlg::OnEnKillfocusEditEsMutation()
{
    this->handleEditChangeFloat(IDC_EDIT_ES_MUTATION);
}


void 
ESDlg::OnEnKillfocusEditEsLambda()
{
    this->handleEditChange(IDC_EDIT_ES_LAMBDA);
}


void 
ESDlg::OnEnKillfocusEditEsMu()
{
    this->handleEditChange(IDC_EDIT_ES_MU);
}


void 
ESDlg::OnEnKillfocusEditEsGenerations()
{
    this->handleEditChange(IDC_EDIT_ES_GENERATIONS);
}


void 
ESDlg::OnOK()
{
    ApplyValues();
    CDialogEx::OnOK();
}


BOOL 
ESDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE;
}

void 
ESDlg::ApplyValues()
{
    for (int i = 0; i < MAX_INT_ES_PARAMS; i++)
    {
        *(intESParams[i].valuePtr) = intESParams[i].value;
    }
    for (int i = 0; i < MAX_FLOAT_ES_PARAMS; i++)
    {
        *(floatESParams[i].valuePtr) =  floatESParams[i].value;
    }
    ESInfo.autoSeed = autogen;
}

void 
ESDlg::UpdateValues(void)
{
    CString buffer;

    updating = TRUE;
    for (int i = 0; i < MAX_INT_ES_PARAMS; i++)
    {
        IntESParam *p = intESParams + i;
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

    for (int i = 0; i < MAX_FLOAT_ES_PARAMS; i++)
    {
        FloatESParam *p = floatESParams + i;
        if (p->dirty)
        {
            float value = p->value;
            buffer.Format("%3.3f", value);
            GetDlgItem(p->textControl)->SetWindowText(buffer);
            p->dirty = FALSE;
        }
    }

    // Handle autogen check box.
    ((CButton*) GetDlgItem(IDC_CHECK_ES_AUTOGEN))->SetCheck(autogen ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_EDIT_ES_SEED)->EnableWindow(!autogen);
    GetDlgItem(IDC_BUTTON_ES_GENERATE)->EnableWindow(!autogen);

	HandleMutationType((getIntParam(IDC_RADIO_ES_SM)->value));

	// Handle local neighborhood mutation
	GetDlgItem(IDC_CHECK_ES_LNM)->EnableWindow(ln_mutation);
	((CButton*) GetDlgItem(IDC_CHECK_ES_LNM))->SetCheck(use_default_resolution);
	GetDlgItem(IDC_EDIT_ES_LNM)->EnableWindow(!use_default_resolution & ln_mutation);
	mesh_resolution = use_default_resolution ? default_mesh_resolution : mesh_resolution;
	buffer.Format("%f", mesh_resolution);
	GetDlgItem(IDC_EDIT_ES_LNM)->SetWindowText(buffer);

    updating = FALSE;

}

void 
ESDlg::HandleMutationType(unsigned int type)
{
	if (type == SIMPLE_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_ES_SM))->SetCheck(BST_CHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_ES_NM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_ES_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == GLOBAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_ES_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_ES_NM))->SetCheck(BST_CHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_ES_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == LOCAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_ES_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_ES_NM))->SetCheck(BST_UNCHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_ES_LNM))->SetCheck(BST_CHECKED);
		ln_mutation = true;
	}
}



void 
ESDlg::ResetToDefaults(void)
{
    for (int i = 0; i < MAX_INT_ES_PARAMS; i++)
    {
        intESParams[i].value = intESParams[i].defaultValue;
        intESParams[i].dirty = TRUE;
    }
    autogen = TRUE;

    for (int i = 0; i < MAX_FLOAT_ES_PARAMS; i++)
    {
        floatESParams[i].value = floatESParams[i].defaultValue;
        floatESParams[i].dirty = TRUE;
    }
}

void
ESDlg::ReadValues(void)
{
    for (int i = 0; i < MAX_INT_ES_PARAMS; i++)
    {
        intESParams[i].value = *(intESParams[i].valuePtr);
        intESParams[i].dirty = TRUE;
    }
    for (int i = 0; i < MAX_FLOAT_ES_PARAMS; i++)
    {
        floatESParams[i].value = *(floatESParams[i].valuePtr);
        floatESParams[i].dirty = TRUE;
    }
}


void 
ESDlg::handleEditChange(int editId)
{
    if (updating) return;

    IntESParam* p = this->getIntParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = atoi(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}


IntESParam* 
ESDlg::getIntParam(int editId)
{
    for (int i = 0; i < MAX_INT_ES_PARAMS; i++)
    {
        if (intESParams[i].textControl == editId
			|| intESParams[i].otherControl == editId)
        {
            return intESParams + i;
        }
    }
    return NULL;
}


void
ESDlg::handleEditChangeFloat(int editId)
{
    if (updating) return;

    FloatESParam* p = this->getFloatParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = (float) atof(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}

FloatESParam*
ESDlg::getFloatParam(int editId)
{
    for (int i = 0; i < MAX_FLOAT_ES_PARAMS; i++)
    {
        if (floatESParams[i].textControl == editId )
        {
            return floatESParams + i;
        }
    }
    return NULL;
}


void ESDlg::OnBnClickedButtonEsApply()
{
    // TODO: Add your control notification handler code here
    ApplyValues();
}


void ESDlg::OnBnClickedCheckEsAutogen()
{
    // TODO: Add your control notification handler code here
    CButton* button = (CButton*) GetDlgItem(IDC_CHECK_ES_AUTOGEN);
    autogen = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}


void ESDlg::OnBnClickedButtonEsGenerate()
{
    
    IntESParam* p = getIntParam(IDC_EDIT_ES_SEED);
    p->value = (unsigned int) time(NULL);
    p->dirty = TRUE;
    UpdateValues();
}


void ESDlg::OnEnKillfocusEditEsSeed()
{
	 this->handleEditChange(IDC_EDIT_ES_SEED);
}


void 
ESDlg::OnBnClickedRadioEsSm()
{
	IntESParam* mutationType = getIntParam(IDC_RADIO_ES_SM);
    mutationType->value = SIMPLE_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}


void 
ESDlg::OnBnClickedRadioEsNm()
{
	IntESParam* mutationType = getIntParam(IDC_RADIO_ES_SM);
    mutationType->value = GLOBAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}


void 
ESDlg::OnBnClickedRadioEsLnm()
{
	IntESParam* mutationType = getIntParam(IDC_RADIO_ES_SM);
    mutationType->value = LOCAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}


void 
ESDlg::OnBnClickedCheckEsLnm()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_ES_LNM);
    use_default_resolution = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}


void 
ESDlg::OnEnKillfocusEditEsLnm()
{
	CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_ES_LNM))->GetWindowText(buffer);
    mesh_resolution = (float) atof(buffer);
}
