#pragma once

typedef struct IntHCParam
{
    unsigned int *valuePtr;
    unsigned int defaultValue;
    int textControl;
	int otherControl;
    unsigned int value;
    bool dirty;
} IntHCParam;


typedef struct FloatHCParam
{
    float *valuePtr;
    float defaultValue;
    int textControl;
    float value;
    bool dirty;
} FloatHCParam;


// HCDlg dialog

class HCDlg : public CDialogEx
{
    DECLARE_DYNAMIC(HCDlg)
private:
    bool updating;
	bool autogen;
	bool ln_mutation;
	bool use_default_resolution;
	void HandleMutationType(unsigned int type);

public:
    HCDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~HCDlg();

// Dialog Data
    enum { IDD = IDD_HC_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // Handles the integer parameters.
    IntHCParam* getIntParam(int editId);
    void handleEditChange(int editId);

    // Handles the float parameters.
    FloatHCParam* getFloatParam(int editId);
    void handleEditChangeFloat(int editId);


    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnKillfocusEditHcIterations();
    afx_msg void OnEnKillfocusEditHcMutations();
    afx_msg void OnEnKillfocusEditHcSeed();
    afx_msg void OnEnKillfocusEditHcFreq();
    afx_msg void OnEnKillfocusEditHcConvergence();
    afx_msg void OnEnKillfocusEditHcGenCond();
    afx_msg void OnEnKillfocusEditHcFitnessCond();
    void UpdateValues(void);
    void ResetToDefaults(void);
    void ApplyValues(void);
    void ReadValues(void);

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedButtonHcApply();
    afx_msg void OnEnKillfocusEdiHcExpweight();
	afx_msg void OnBnClickedRadioHcSm();
	afx_msg void OnBnClickedRadioHcNm();
	afx_msg void OnEnKillfocusEditHcLnm();
	afx_msg void OnBnClickedRadioHcLnm();
	afx_msg void OnBnClickedCheckHcLnm();
	afx_msg void OnBnClickedCheckHcAutogen();
	afx_msg void OnBnClickedButtonHcGenerate();
};
