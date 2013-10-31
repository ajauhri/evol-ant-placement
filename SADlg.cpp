// SADlg.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "SADlg.h"
#include "afxdialogex.h"
#include "SA.h"
#include "AntennaConfiguration.h"

// SADlg dialog

IMPLEMENT_DYNAMIC(SADlg, CDialogEx)

#define DEFAULT_SA_ITERATIONS 20
#define DEFAULT_SA_SEED 4321
#define DEFAULT_SA_ITER_PER_TEMP 1000
#define DEFAULT_SA_FREQ 5
#define DEFAULT_SA_GEN_COND 10
#define DEFAULT_SA_FITNESS_COND 1
#define DEFAULT_SA_MUTATIONS 0.001
#define DEFAULT_SA_INITIAL_TEMP 25.0
#define DEFAULT_SA_TEMP_FACTOR 0.92
#define DEFAULT_SA_CONVERGENCE 1.0
#define DEFAULT_SA_EXPWEIGHT 2.0f
#define DEFAULT_SA_MUTATION_TYPE 0

IntSAParam intSAParams[] =
{
// *valuePtr,              default-value,                       text-controller,        , other-controller, value, dirty
    {&(SAInfo.iterations),				DEFAULT_SA_ITERATIONS,		IDC_EDIT_SA_ITERATIONS},
    {&(SAInfo.seed),					DEFAULT_SA_SEED,			IDC_EDIT_SA_SEED},
    {&(SAInfo.iterationsPerTempChng),   DEFAULT_SA_ITER_PER_TEMP,   IDC_EDIT_SA_ITER_PER_TEMP},
    {&(SAInfo.outputFreq),				DEFAULT_SA_FREQ,			IDC_EDIT_SA_FREQ},
    {&(SAInfo.genCondition),			DEFAULT_SA_GEN_COND,		IDC_EDIT_SA_GEN_COND},
    {&(SAInfo.fitnessCondition),		DEFAULT_SA_FITNESS_COND,	IDC_EDIT_SA_FITNESS_COND},
	{&(SAInfo.mutationType),			DEFAULT_SA_MUTATION_TYPE,	0, IDC_RADIO_SA_SM},
};
#define MAX_INT_SA_PARAMS 7

FloatSAParam floatSAParams[] =
{
// *valuePtr,              default-value,       min-value, max-value,  text-controller,    slider-controller, value, dirty
    {&(SAInfo.mutationProbability),		DEFAULT_SA_MUTATIONS,       IDC_EDIT_SA_MUTATIONS},
    {&(SAInfo.initialTemp),				DEFAULT_SA_INITIAL_TEMP,	IDC_EDIT_SA_INITIAL_TEMP},
    {&(SAInfo.tempFactor),				DEFAULT_SA_TEMP_FACTOR,     IDC_EDIT_SA_TEMP_FACTOR},
    {&(SAInfo.convergenceFactor),		DEFAULT_SA_CONVERGENCE,     IDC_EDIT_SA_CONVERGENCE},
    {&(SAInfo.expWeight),				DEFAULT_SA_EXPWEIGHT,	    IDC_EDIT_SA_EXPWEIGHT},
};
#define MAX_FLOAT_SA_PARAMS 5


SADlg::SADlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(SADlg::IDD, pParent)
    , updating(false)
	, autogen(true)
	, ln_mutation(false)
	, use_default_resolution(true)
{
    this->Create(IDD_SA_DIALOG);
    ResetToDefaults();
    UpdateValues();
}

SADlg::~SADlg()
{
}

void 
SADlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SADlg, CDialogEx)
//	ON_EN_CHANGE(IDC_EDIT_SA_ITERATIONS, &SADlg::OnEnChangeEditSaIterations)
//	ON_EN_CHANGE(IDC_EDIT_SA_MUTATIONS, &SADlg::OnEnChangeEditSaMutations)
//	ON_EN_CHANGE(IDC_EDIT_SA_SEED, &SADlg::OnEnChangeEditSaSeed)
//	ON_EN_CHANGE(IDC_EDIT_SA_INITIAL_TEMP, &SADlg::OnEnChangeEditSaInitialTemp)
//	ON_EN_CHANGE(IDC_EDIT_SA_TEMP_FACTOR, &SADlg::OnEnChangeEditSaTempFactor)
//	ON_EN_CHANGE(IDC_EDIT_SA_ITER_PER_TEMP, &SADlg::OnEnChangeEditSaIterPerTemp)
//	ON_EN_CHANGE(IDC_EDIT_SA_FREQ, &SADlg::OnEnChangeEditSaFreq)
//	ON_EN_CHANGE(IDC_EDIT_SA_CONVERGENCE, &SADlg::OnEnChangeEditSaConvergence)
//	ON_EN_CHANGE(IDC_EDIT_SA_GEN_COND, &SADlg::OnEnChangeEditSaGenCond)
//	ON_EN_CHANGE(IDC_EDIT_SA_FITNESS_COND, &SADlg::OnEnChangeEditSaFitnessCond)
    ON_BN_CLICKED(IDC_BUTTON_SA_APPLY, &SADlg::OnBnClickedButtonSaApply)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_CONVERGENCE, &SADlg::OnKillfocusEditSaConvergence)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_TEMP_FACTOR, &SADlg::OnKillfocusEditSaTempFactor)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_SEED, &SADlg::OnKillfocusEditSaSeed)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_FITNESS_COND, &SADlg::OnKillfocusEditSaFitnessCond)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_FREQ, &SADlg::OnKillfocusEditSaFreq)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_GEN_COND, &SADlg::OnKillfocusEditSaGenCond)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_INITIAL_TEMP, &SADlg::OnKillfocusEditSaInitialTemp)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_MUTATIONS, &SADlg::OnKillfocusEditSaMutations)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_ITERATIONS, &SADlg::OnKillfocusEditSaIterations)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_ITER_PER_TEMP, &SADlg::OnKillfocusEditSaIterPerTemp)
    ON_EN_KILLFOCUS(IDC_EDIT_SA_EXPWEIGHT, &SADlg::OnEnKillfocusEditSaExpweight)
	ON_BN_CLICKED(IDC_RADIO_SA_SM, &SADlg::OnBnClickedRadioSaSm)
	ON_BN_CLICKED(IDC_RADIO_SA_NM, &SADlg::OnBnClickedRadioSaNm)
	ON_BN_CLICKED(IDC_CHECK_SA_AUTOGEN, &SADlg::OnBnClickedCheckSaAutogen)
	ON_BN_CLICKED(IDC_BUTTON_SA_GENERATE, &SADlg::OnBnClickedButtonSaGenerate)
	ON_EN_KILLFOCUS(IDC_EDIT_SA_LNM, &SADlg::OnEnKillfocusEditSaLnm)
	ON_BN_CLICKED(IDC_RADIO_SA_LNM, &SADlg::OnBnClickedRadioSaLnm)
	ON_BN_CLICKED(IDC_CHECK_SA_LNM, &SADlg::OnBnClickedCheckSaLnm)
END_MESSAGE_MAP()


// SADlg message handlers

BOOL 
SADlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void 
SADlg::ApplyValues()
{
    for (int i = 0; i < MAX_INT_SA_PARAMS; i++)
    {
        *(intSAParams[i].valuePtr) = intSAParams[i].value;
    }
    for (int i = 0; i < MAX_FLOAT_SA_PARAMS; i++)
    {
        *(floatSAParams[i].valuePtr) =  floatSAParams[i].value;
    }
	SAInfo.autoSeed = autogen;
}

void 
SADlg::OnBnClickedButtonSaApply()
{
    ApplyValues();
}


void
SADlg::UpdateValues(void)
{

    CString buffer;
    updating = TRUE;
    for (int i = 0; i < MAX_INT_SA_PARAMS; i++)
    {
        IntSAParam *p = intSAParams + i;
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

    for (int i = 0; i < MAX_FLOAT_SA_PARAMS; i++)
    {
        FloatSAParam *p = floatSAParams + i;
        if (p->dirty)
        {
            float value = p->value;
            buffer.Format("%3.3f", value);
            GetDlgItem(p->textControl)->SetWindowText(buffer);
            p->dirty = FALSE;
        }
    }

	 // Handle autogen check box.
    ((CButton*) GetDlgItem(IDC_CHECK_SA_AUTOGEN))->SetCheck(autogen ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_EDIT_SA_SEED)->EnableWindow(!autogen);
    GetDlgItem(IDC_BUTTON_SA_GENERATE)->EnableWindow(!autogen);

	HandleMutationType((getIntParam(IDC_RADIO_SA_SM)->value));
	
	// Handle local neighborhood mutation
	GetDlgItem(IDC_CHECK_SA_LNM)->EnableWindow(ln_mutation);
	((CButton*) GetDlgItem(IDC_CHECK_SA_LNM))->SetCheck(use_default_resolution);
	GetDlgItem(IDC_EDIT_SA_LNM)->EnableWindow(!use_default_resolution & ln_mutation);
	mesh_resolution = use_default_resolution ? default_mesh_resolution : mesh_resolution;
	buffer.Format("%f", mesh_resolution);
	GetDlgItem(IDC_EDIT_SA_LNM)->SetWindowText(buffer);
	

    updating = FALSE;
}


void 
SADlg::HandleMutationType(unsigned int type)
{
	if (type == SIMPLE_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_SA_SM))->SetCheck(BST_CHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_SA_NM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_SA_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == GLOBAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_SA_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_SA_NM))->SetCheck(BST_CHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_SA_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == LOCAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_SA_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_SA_NM))->SetCheck(BST_UNCHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_SA_LNM))->SetCheck(BST_CHECKED);
		ln_mutation = true;
	}

}


void 
SADlg::ResetToDefaults(void)
{
    for (int i = 0; i < MAX_INT_SA_PARAMS; i++)
    {
        intSAParams[i].value = intSAParams[i].defaultValue;
        intSAParams[i].dirty = TRUE;
    }
	autogen = TRUE;
    for (int i = 0; i < MAX_FLOAT_SA_PARAMS; i++)
    {
        floatSAParams[i].value = floatSAParams[i].defaultValue;
        floatSAParams[i].dirty = TRUE;
    }
}

void 
SADlg::ReadValues(void)
{
    for (int i = 0; i < MAX_INT_SA_PARAMS; i++)
    {
        intSAParams[i].value = *(intSAParams[i].valuePtr);
        intSAParams[i].dirty = TRUE;
    }
    for (int i = 0; i < MAX_FLOAT_SA_PARAMS; i++)
    {
        floatSAParams[i].value = *(floatSAParams[i].valuePtr);
        floatSAParams[i].dirty = TRUE;
    }
}

void 
SADlg::OnOK()
{
    ApplyValues();
    CDialogEx::OnOK();
}


void 
SADlg::handleEditChange(int editId)
{
    if (updating) return;
    IntSAParam* p = this->getIntParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = atoi(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}


IntSAParam* 
SADlg::getIntParam(int editId)
{
    for (int i = 0; i < MAX_INT_SA_PARAMS; i++)
    {
        if (intSAParams[i].textControl == editId
			|| intSAParams[i].otherControl == editId)
        {
            return intSAParams + i;
        }
    }
    return NULL;
}


void 
SADlg::handleEditChangeFloat(int editId)
{
    if (updating) return;

    FloatSAParam* p = this->getFloatParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = (float) atof(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}

FloatSAParam*
SADlg::getFloatParam(int editId)
{
    for (int i = 0; i < MAX_FLOAT_SA_PARAMS; i++)
    {
        if (floatSAParams[i].textControl == editId )
        {
            return floatSAParams + i;
        }
    }
    return NULL;
}


void 
SADlg::OnKillfocusEditSaConvergence()
{
    this->handleEditChangeFloat(IDC_EDIT_SA_CONVERGENCE);
}


void 
SADlg::OnKillfocusEditSaTempFactor()
{
    this->handleEditChangeFloat(IDC_EDIT_SA_TEMP_FACTOR);
}


void
SADlg::OnKillfocusEditSaSeed()
{
    this->handleEditChange(IDC_EDIT_SA_SEED);
}


void 
SADlg::OnKillfocusEditSaFitnessCond()
{
    this->handleEditChange(IDC_EDIT_SA_FITNESS_COND);
}


void 
SADlg::OnKillfocusEditSaFreq()
{
    this->handleEditChange(IDC_EDIT_SA_FREQ);
}


void 
SADlg::OnKillfocusEditSaGenCond()
{
    this->handleEditChange(IDC_EDIT_SA_GEN_COND);
}


void 
SADlg::OnKillfocusEditSaInitialTemp()
{
    this->handleEditChangeFloat(IDC_EDIT_SA_INITIAL_TEMP);
}


void 
SADlg::OnKillfocusEditSaMutations()
{
    this->handleEditChangeFloat(IDC_EDIT_SA_MUTATIONS);
}


void 
SADlg::OnKillfocusEditSaIterations()
{
    this->handleEditChange(IDC_EDIT_SA_ITERATIONS);
}


void 
SADlg::OnKillfocusEditSaIterPerTemp()
{
    this->handleEditChange(IDC_EDIT_SA_ITER_PER_TEMP);
}


void 
SADlg::OnEnKillfocusEditSaExpweight()
{
    this->handleEditChangeFloat(IDC_EDIT_SA_EXPWEIGHT);
}


void 
SADlg::OnBnClickedCheckSaAutogen()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_SA_AUTOGEN);
    autogen = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}


void 
SADlg::OnBnClickedButtonSaGenerate()
{
	IntSAParam* p = getIntParam(IDC_EDIT_SA_SEED);
    p->value = (unsigned int) time(NULL);
    p->dirty = TRUE;
    UpdateValues();
}

void 
SADlg::OnBnClickedRadioSaSm()
{
	IntSAParam* mutationType = getIntParam(IDC_RADIO_SA_SM);
    mutationType->value = SIMPLE_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void 
SADlg::OnBnClickedRadioSaNm()
{
	IntSAParam* mutationType = getIntParam(IDC_RADIO_SA_SM);
    mutationType->value = GLOBAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void 
SADlg::OnBnClickedRadioSaLnm()
{
	IntSAParam* mutationType = getIntParam(IDC_RADIO_SA_SM);
    mutationType->value = LOCAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}


void 
SADlg::OnEnKillfocusEditSaLnm()
{
	CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_SA_LNM))->GetWindowText(buffer);
    mesh_resolution = (float) atof(buffer);
}

void 
SADlg::OnBnClickedCheckSaLnm()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_SA_LNM);
    use_default_resolution = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}
