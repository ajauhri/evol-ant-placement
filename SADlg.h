#pragma once

typedef struct IntSAParam
{
    unsigned int *valuePtr;
    unsigned int defaultValue;
    int textControl;
	int otherControl;
    unsigned int value;
    bool dirty;
} IntSAParam;

typedef struct FloatSAParam
{
    float *valuePtr;
    float defaultValue;
    int textControl;
    float value;
    bool dirty;
} FloatSAParam;

// SADlg dialog

class SADlg : public CDialogEx
{
    DECLARE_DYNAMIC(SADlg)

private:
    bool updating;
	bool autogen;
	bool ln_mutation;
	bool use_default_resolution;
	void HandleMutationType(unsigned int type);
public:
    SADlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~SADlg();

// Dialog Data
    enum { IDD = IDD_SA_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // Handles the integer parameters.
    IntSAParam* getIntParam(int editId);
    void handleEditChange(int editId);

    // Handles the float parameters.
    FloatSAParam* getFloatParam(int editId);
    void handleEditChangeFloat(int editId);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonSaApply();
    void UpdateValues(void);
    void ResetToDefaults(void);
    void ApplyValues(void);
    void ReadValues(void);
    virtual void OnOK();
    afx_msg void OnKillfocusEditSaConvergence();
    afx_msg void OnKillfocusEditSaTempFactor();
    afx_msg void OnKillfocusEditSaSeed();
    afx_msg void OnKillfocusEditSaFitnessCond();
    afx_msg void OnKillfocusEditSaFreq();
    afx_msg void OnKillfocusEditSaGenCond();
    afx_msg void OnKillfocusEditSaInitialTemp();
    afx_msg void OnKillfocusEditSaMutations();
    afx_msg void OnKillfocusEditSaIterations();
    afx_msg void OnKillfocusEditSaIterPerTemp();
    afx_msg void OnEnKillfocusEditSaExpweight();
	afx_msg void OnBnClickedRadioSaSm();
	afx_msg void OnBnClickedRadioSaNm();
	afx_msg void OnBnClickedCheckSaAutogen();
	afx_msg void OnBnClickedButtonSaGenerate();
	afx_msg void OnEnKillfocusEditSaLnm();
	afx_msg void OnBnClickedRadioSaLnm();
	afx_msg void OnBnClickedCheckSaLnm();
}; //class SAdlg




