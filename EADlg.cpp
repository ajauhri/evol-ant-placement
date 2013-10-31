// EADlg.cpp
//
// How To Add A New Parameter (Integer or Float)
// =============================================
//
// Using this how to, one can easily add a new integer or float parameter to the UI.
// It is written for an integer, but works in an analogous fashion for the float paramters.
//
// 1. Add the new parameter to the UI and give the text field a name.
// 2. Add a new entry to intParam describing
// 3. Increment MAX_INT_PARAMS
// 4. Add a handler for the text field's event KILL FOCUS, calling handleEditChange(text-field-id)

#include "stdafx.h"
#include "APOT6.h"
#include "EADlg.h"
#include "AntennaConfiguration.h"
#include "math.h"
// EADlg dialog

#define DEFAULT_GENERATIONS 10
#define DEFAULT_ELITISM      3
#define DEFAULT_POPSIZE     10
#define DEFAULT_TOURSIZE    10
#define DEFAULT_LAYERS       5
#define DEFAULT_LAYERSIZE   10
#define DEFAULT_MUTATION 0.01f
#define DEFAULT_RECOMB   0.20f
#define DEFAULT_SEED         0
#define DEFAULT_AGEGAP      10
#define DEFAULT_EXPWEIGHT 2.0f
#define DEFAULT_REPR         0
#define DEFAULT_ALGORTYPE    0
#define DEFAULT_MUTATION_TYPE 0

#define MAX_GENERATIONS    100
#define MAX_POPSIZE        100
#define MAX_LAYERS          10
#define MAX_LAYER_SIZE      25
#define MAX_MUTATION     1.00f
#define MAX_RECOMB       1.00f
#define MAX_SEED      UINT_MAX
#define MAX_AGEGAP         100
#define MAX_EXPWEIGHT     5.0f
#define MAX_REPR             1
#define MAX_ALGORTYPE        2
#define MAX_MUTATION_TYPE	2

IntParam intParams[] =
{
// *valuePtr,              default-value,       min-value, max-value,  text-controller,    slider-controller, value, dirty
    {&(EAInfo.generations),    DEFAULT_GENERATIONS, 1, MAX_GENERATIONS,    IDC_EDIT_GEN,       0 },
    {&(EAInfo.elitism),        DEFAULT_ELITISM,     0, DEFAULT_POPSIZE - 1,IDC_EDIT_ELITISM,   0 },
    {&(EAInfo.popSize),        DEFAULT_POPSIZE,     1, MAX_POPSIZE,        IDC_EDIT_POPSIZE,   0 },
    {&(EAInfo.tournamentSize), DEFAULT_TOURSIZE,    2, DEFAULT_POPSIZE,    IDC_EDIT_TOURSIZE,  0 },
    {&(EAInfo.layers),         DEFAULT_LAYERS,      2, MAX_LAYERS,         IDC_EDIT_LAYERS,    0 },
    {&(EAInfo.layerSize),      DEFAULT_LAYERSIZE,   2, MAX_LAYER_SIZE,     IDC_EDIT_LAYERSIZE, 0 },
    {&(EAInfo.seed),           DEFAULT_SEED,        0, MAX_SEED,           IDC_EDIT_SEED,      0 },
    {&(EAInfo.ageGap),         DEFAULT_AGEGAP,      1, MAX_AGEGAP,         IDC_EDIT_AGEGAP,    0},
    {&(EAInfo.representation), DEFAULT_REPR,        0, MAX_REPR,           0,                  IDC_COMBO_REPR},
    {&(EAInfo.algorType),      DEFAULT_ALGORTYPE,   0, MAX_ALGORTYPE,      0,                  IDC_RADIO_STDGA},
	{&(EAInfo.mutationType),   DEFAULT_MUTATION_TYPE,   0, MAX_MUTATION_TYPE,      0,          IDC_RADIO_GA_SM},
};
#define MAX_INT_PARAMS 11

FloatParam floatParams[] =
{
// *valuePtr,              default-value,       min-value, max-value,  text-controller,    slider-controller, value, dirty
    {&(EAInfo.mutation),       DEFAULT_MUTATION,    0.0f, MAX_MUTATION,    IDC_EDIT_MUTATION,  0 },
    {&(EAInfo.recombination),  DEFAULT_RECOMB,      0.0f, MAX_RECOMB,      IDC_EDIT_RECOMB,    0 },
    {&(EAInfo.expWeight),      DEFAULT_EXPWEIGHT,   1.0f, MAX_EXPWEIGHT,   IDC_EDIT_EXPWEIGHT, 0 },
};
#define MAX_FLOAT_PARAMS 3

IMPLEMENT_DYNAMIC(EADlg, CDialog)

EADlg::EADlg(CWnd* pParent /*=NULL*/)
    : CDialog(EADlg::IDD, pParent)
    , updating(false)
    , autogen(true)
	, ln_mutation(false)
	, use_default_resolution(true)
{
    this->Create(IDD_EA_DIALOG);
	EnableSTDGA(getIntParam(IDC_RADIO_STDGA)->value);
    ((CButton*) GetDlgItem(IDC_RADIO_STDGA))->SetCheck(BST_CHECKED);
	
	
    CString str;
    CComboBox* box = (CComboBox*) GetDlgItem(IDC_COMBO_REPR);
    str.Format("A. List of Points");
    box->AddString(str);
    str.Format("B. Cartesian Location");
    box->AddString(str);
    box->SetCurSel(0);

    ResetToDefaults();
    UpdateValues();
}

EADlg::~EADlg()
{
}

void 
EADlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EADlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON2, &EADlg::OnBnClickedButton2)

    ON_EN_KILLFOCUS(IDC_EDIT_GEN, &EADlg::OnEnChangeEditGen)
    ON_EN_KILLFOCUS(IDC_EDIT_TOURSIZE, &EADlg::OnEnChangeEditToursize)
    ON_EN_KILLFOCUS(IDC_EDIT_ELITISM, &EADlg::OnEnChangeEditElitism)
    ON_EN_KILLFOCUS(IDC_EDIT_POPSIZE, &EADlg::OnEnChangeEditPopsize)
    ON_EN_KILLFOCUS(IDC_EDIT_LAYERS, &EADlg::OnEnChangeEditLayers)
    ON_EN_KILLFOCUS(IDC_EDIT_LAYERSIZE, &EADlg::OnEnChangeEditLayersize)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &EADlg::OnBnClickedButtonApply)
    ON_BN_CLICKED(IDC_RADIO_STDGA, &EADlg::OnBnClickedRadioSTDGA)
    ON_BN_CLICKED(IDC_RADIO_ALPSEA, &EADlg::OnBnClickedRadioALPSEA)
    ON_EN_KILLFOCUS(IDC_EDIT_MUTATION, &EADlg::OnEnKillfocusEditMutation)
    ON_EN_KILLFOCUS(IDC_EDIT_RECOMB, &EADlg::OnEnKillfocusEditRecomb)
    ON_BN_CLICKED(IDC_CHECK_AUTOGEN, &EADlg::OnBnClickedCheckAutogen)
    ON_BN_CLICKED(IDC_BUTTON_GENERATE, &EADlg::OnBnClickedButtonGenerate)
    ON_CBN_KILLFOCUS(IDC_COMBO_REPR, &EADlg::OnCbnKillfocusComboRepr)
    ON_EN_KILLFOCUS(IDC_EDIT_AGEGAP, &EADlg::OnEnKillfocusEditAgegap)
    ON_EN_KILLFOCUS(IDC_EDIT_EXPWEIGHT, &EADlg::OnEnKillfocusEditExpweight)
    ON_BN_CLICKED(IDC_RADIO_EXHAUSTIVE, &EADlg::OnBnClickedRadioExhaustive)
	ON_BN_CLICKED(IDC_RADIO_GA_SM, &EADlg::OnBnClickedRadioGaSm)
	ON_BN_CLICKED(IDC_RADIO_GA_NM, &EADlg::OnBnClickedRadioGaNm)
	ON_EN_KILLFOCUS(IDC_EDIT_GA_LNM, &EADlg::OnEnKillfocusEditGaLnm)
	ON_BN_CLICKED(IDC_RADIO_GA_LNM, &EADlg::OnBnClickedRadioGaLnm)
	ON_BN_CLICKED(IDC_CHECK_GA_LNM, &EADlg::OnBnClickedCheckGaLnm)
END_MESSAGE_MAP()


// EADlg message handlers

BOOL
EADlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    return TRUE;
}


void 
EADlg::ResetToDefaults(void)
{
    for (int i = 0; i < MAX_INT_PARAMS; i++)
    {
        intParams[i].value = intParams[i].defaultValue;
        intParams[i].dirty = TRUE;
    }
    updatePopDepValues();
    autogen = TRUE;
    for (int i = 0; i < MAX_FLOAT_PARAMS; i++)
    {
        floatParams[i].value = floatParams[i].defaultValue;
        floatParams[i].dirty = TRUE;
    }
}

void 
EADlg::UpdateValues()
{
    CString buffer;
    updatePopDepValues();
    updating = TRUE;

    // Handle useIndex combo box.  Want to do this before the general loop.
    IntParam *repr = getIntParam(IDC_COMBO_REPR);
    if (repr->dirty)
    {
        CComboBox* box = (CComboBox*) GetDlgItem(IDC_COMBO_REPR);
        box->SetCurSel(repr->value);
        repr->dirty = FALSE;
    }

    for (int i = 0; i < MAX_INT_PARAMS; i++)
    {
        IntParam *p = intParams + i;
        if (p->dirty)
        {
            unsigned int value = min(p->value, p->maxValue);
            CEdit* edit = (CEdit *) GetDlgItem(p->textControl);
            if (edit)
            {
                buffer.Format("%d", value);
                edit->SetWindowText(buffer);
            }
            p->dirty = FALSE;
        }
    }

    for (int i = 0; i < MAX_FLOAT_PARAMS; i++)
    {
        FloatParam *p = floatParams + i;
        if (p->dirty)
        {
            float value = min(p->value, p->maxValue);
            buffer.Format("%3.2f", value);
            GetDlgItem(p->textControl)->SetWindowText(buffer);
            p->dirty = FALSE;
        }
    }
    IntParam* algorType = getIntParam(IDC_RADIO_STDGA);
    // Handle GA type radio button.

	HandleMutationType((getIntParam(IDC_RADIO_GA_SM)->value));
		
    EnableSTDGA(algorType->value);

    // Handle autogen check box.
    ((CButton*) GetDlgItem(IDC_CHECK_AUTOGEN))->SetCheck(autogen ? BST_CHECKED : BST_UNCHECKED);
    GetDlgItem(IDC_EDIT_SEED)->EnableWindow(!autogen);
    GetDlgItem(IDC_BUTTON_GENERATE)->EnableWindow(!autogen);

	// Handle local neighborhood mutation
	GetDlgItem(IDC_CHECK_GA_LNM)->EnableWindow(ln_mutation);
	((CButton*) GetDlgItem(IDC_CHECK_GA_LNM))->SetCheck(use_default_resolution);
	GetDlgItem(IDC_EDIT_GA_LNM)->EnableWindow(!use_default_resolution & ln_mutation);
	mesh_resolution = use_default_resolution ? default_mesh_resolution : mesh_resolution;
	buffer.Format("%f", mesh_resolution);
	GetDlgItem(IDC_EDIT_GA_LNM)->SetWindowText(buffer);
	
    updating = FALSE;
}

void 
EADlg::HandleMutationType(unsigned int type)
{
	if (type == SIMPLE_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_GA_SM))->SetCheck(BST_CHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_GA_NM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_GA_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == GLOBAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_GA_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_GA_NM))->SetCheck(BST_CHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_GA_LNM))->SetCheck(BST_UNCHECKED);
		ln_mutation = false;
	}
	else if (type == LOCAL_NEIGHBOURHOOD_MUTATION)
	{
		((CButton*) GetDlgItem(IDC_RADIO_GA_SM))->SetCheck(BST_UNCHECKED);
		((CButton*) GetDlgItem(IDC_RADIO_GA_NM))->SetCheck(BST_UNCHECKED);	
		((CButton*) GetDlgItem(IDC_RADIO_GA_LNM))->SetCheck(BST_CHECKED);
		ln_mutation = true;
	}
}

void 
EADlg::OnBnClickedButton2()
{
    this->UpdateValues();
}

void 
EADlg::OnEnChangeEditGen()
{
    this->handleEditChange(IDC_EDIT_GEN);
    //OnUpdateRunButton(0, 0);
}

void
EADlg::handleEditChange(int editId)
{
    if (updating) return;

    IntParam* p = this->getIntParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = atoi(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}

IntParam* 
EADlg::getIntParam(int editId)
{
    for (int i = 0; i < MAX_INT_PARAMS; i++)
    {
        if (intParams[i].textControl == editId
                || intParams[i].otherControl == editId)
        {
            return intParams + i;
        }
    }
    return NULL;
}

void 
EADlg::OnEnChangeEditToursize()
{
    this->handleEditChange(IDC_EDIT_TOURSIZE);
}

void 
EADlg::OnEnChangeEditElitism()
{
    this->handleEditChange(IDC_EDIT_ELITISM);
}

void 
EADlg::OnEnChangeEditPopsize()
{
    this->handleEditChange(IDC_EDIT_POPSIZE);
}

void 
EADlg::OnEnChangeEditLayers()
{
    this->handleEditChange(IDC_EDIT_LAYERS);
}

void 
EADlg::OnEnChangeEditLayersize()
{
    handleEditChange(IDC_EDIT_LAYERSIZE);
}


void 
EADlg::ApplyValues(void)
{
    for (int i = 0; i < MAX_INT_PARAMS; i++)
    {
        *(intParams[i].valuePtr) = min(intParams[i].maxValue, intParams[i].value);
    }
    for (int i = 0; i < MAX_FLOAT_PARAMS; i++)
    {
        *(floatParams[i].valuePtr) = min(floatParams[i].maxValue, floatParams[i].value);
    }
    EAInfo.autoSeed = autogen;
}

void 
EADlg::OnBnClickedButtonApply()
{
    ApplyValues();
}

// Update the other ranges that are dependent on the current population value.
void 
EADlg::updatePopDepValues(void)
{
    int population;

    IntParam* algorType = getIntParam(IDC_RADIO_STDGA);

    if (algorType->value == STDGA)
    {
        // Also have to update some ranges.
        IntParam* pop = getIntParam(IDC_EDIT_POPSIZE);
        population = pop->value;
    }
    else if (algorType->value == ALPS)
    {
        IntParam* layers = getIntParam(IDC_EDIT_LAYERS);
        IntParam* layersize = getIntParam(IDC_EDIT_LAYERSIZE);
        population = layersize->value;
    }
    else
    {
        population = 1;
    }
    IntParam* p = getIntParam(IDC_EDIT_TOURSIZE);
    if (p->maxValue != population)
    {
        p->maxValue = population;
        p->dirty = TRUE;
    }
    p = getIntParam(IDC_EDIT_ELITISM);
    if (p->maxValue != (population - 1))
    {
        p->maxValue = population - 1;
        p->dirty = TRUE;
    }
}

void
EADlg::OnOK(void)
{
    ApplyValues();
    CDialog::OnOK();
}

void
EADlg::ReadValues(void)
{
    for (int i = 0; i < MAX_INT_PARAMS; i++)
    {
        intParams[i].value = *(intParams[i].valuePtr);
        intParams[i].dirty = TRUE;
    }
    for (int i = 0; i < MAX_FLOAT_PARAMS; i++)
    {
        floatParams[i].value = *(floatParams[i].valuePtr);
        floatParams[i].dirty = TRUE;
    }
}

void 
EADlg::OnBnClickedRadioSTDGA()
{
    IntParam* algorType = getIntParam(IDC_RADIO_STDGA);
    algorType->value = STDGA;
    algorType->dirty = TRUE;
    UpdateValues();
}

void 
EADlg::EnableSTDGA(unsigned type)
{
    bool stdEnable  = (type == STDGA);
    bool alpsEnable = (type == ALPS);
    bool exhaustEnable = (type == EXHAUST);


    // std ga
    GetDlgItem(IDC_EDIT_POPSIZE)->EnableWindow(stdEnable);

    // alps
    GetDlgItem(IDC_EDIT_LAYERS)->EnableWindow(alpsEnable);

    GetDlgItem(IDC_EDIT_LAYERSIZE)->EnableWindow(alpsEnable);

    GetDlgItem(IDC_EDIT_AGEGAP)->EnableWindow(alpsEnable);

    // exhaust
    GetDlgItem(IDC_EDIT_GEN)->EnableWindow(!exhaustEnable);

    GetDlgItem(IDC_EDIT_TOURSIZE)->EnableWindow(!exhaustEnable);

    GetDlgItem(IDC_EDIT_ELITISM)->EnableWindow(!exhaustEnable);

    GetDlgItem(IDC_EDIT_MUTATION)->EnableWindow(!exhaustEnable);

    GetDlgItem(IDC_EDIT_RECOMB)->EnableWindow(!exhaustEnable);
}

void 
EADlg::OnBnClickedRadioALPSEA()
{
    IntParam* algorType = getIntParam(IDC_RADIO_STDGA);
    algorType->value = ALPS;
    algorType->dirty = TRUE;
    UpdateValues();
}

void 
EADlg::handleEditChangeFloat(int editId)
{
    if (updating) return;

    FloatParam* p = this->getFloatParam(editId);
    CString buffer;
    ((CEdit*) GetDlgItem(editId))->GetWindowText(buffer);
    p->value = (float) atof(buffer);
    p->dirty = TRUE;
    this->UpdateValues();
}

FloatParam*
EADlg::getFloatParam(int editId)
{
    for (int i = 0; i < MAX_FLOAT_PARAMS; i++)
    {
        if (floatParams[i].textControl == editId
                || floatParams[i].otherControl == editId)
        {
            return floatParams + i;
        }
    }
    return NULL;
}

void 
EADlg::OnEnKillfocusEditMutation()
{
    handleEditChangeFloat(IDC_EDIT_MUTATION);
}

void 
EADlg::OnEnKillfocusEditRecomb()
{
    handleEditChangeFloat(IDC_EDIT_RECOMB);
}

void
EADlg::OnBnClickedCheckAutogen()
{
    CButton* button = (CButton*) GetDlgItem(IDC_CHECK_AUTOGEN);
    autogen = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}

void
EADlg::OnBnClickedButtonGenerate()
{
    IntParam* p = getIntParam(IDC_EDIT_SEED);
    p->value = (unsigned int) time(NULL);
    p->dirty = TRUE;
    UpdateValues();
}

void 
EADlg::OnCbnKillfocusComboRepr()
{
    CComboBox* box = (CComboBox*) GetDlgItem(IDC_COMBO_REPR);
    int sel = box->GetCurSel();
    IntParam* repr = getIntParam(IDC_COMBO_REPR);
    if (sel < repr->minValue || sel > repr->maxValue)
    {
        box->SetCurSel(repr->value);  // Don't change anything.
    }
    else
    {
        repr->value = sel;
        repr->dirty = TRUE;
    }
    UpdateValues();
}

void 
EADlg::OnEnKillfocusEditAgegap()
{
    handleEditChange(IDC_EDIT_AGEGAP);
}

void 
EADlg::OnEnKillfocusEditExpweight()
{
    handleEditChangeFloat(IDC_EDIT_EXPWEIGHT);
}

void
EADlg::OnBnClickedRadioExhaustive()
{
    IntParam* algorType = getIntParam(IDC_RADIO_STDGA);
    algorType->value = EXHAUST;
    algorType->dirty = TRUE;
    UpdateValues();
}


void
EADlg::OnBnClickedRadioGaSm()
{
	IntParam* mutationType = getIntParam(IDC_RADIO_GA_SM);
    mutationType->value = SIMPLE_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void
EADlg::OnBnClickedRadioGaNm()
{
	IntParam* mutationType = getIntParam(IDC_RADIO_GA_SM);
    mutationType->value = GLOBAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void 
EADlg::OnEnKillfocusEditGaLnm()
{
	CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_GA_LNM))->GetWindowText(buffer);
    mesh_resolution = (float) atof(buffer);
}

void 
EADlg::OnBnClickedRadioGaLnm()
{
	IntParam* mutationType = getIntParam(IDC_RADIO_GA_SM);
    mutationType->value = LOCAL_NEIGHBOURHOOD_MUTATION;
    mutationType->dirty = TRUE;
	UpdateValues();
}

void
EADlg::OnBnClickedCheckGaLnm()
{
	CButton* button = (CButton*) GetDlgItem(IDC_CHECK_GA_LNM);
    use_default_resolution = (button->GetCheck() == BST_CHECKED);
    UpdateValues();
}