#pragma once

typedef struct IntESParam
{
    unsigned int *valuePtr;
    unsigned int defaultValue;
    int textControl;
	int otherControl;
    unsigned int value;
    bool dirty;
} IntESParam;


typedef struct FloatESParam
{
    float *valuePtr;
    float defaultValue;
    int textControl;
    float value;
    bool dirty;
} FloatESParam;

// ESDlg dialog

class ESDlg : public CDialogEx
{
    DECLARE_DYNAMIC(ESDlg)

private:
    bool updating;
    bool autogen;
	bool ln_mutation;
	bool use_default_resolution;
	void HandleMutationType(unsigned int type);

public:
    ESDlg(CWnd* pParent = NULL);
    virtual ~ESDlg();

// Dialog Data
    enum { IDD = IDD_ES_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // Handles the integer parameters.
    IntESParam* getIntParam(int editId);
    void handleEditChange(int editId);

    // Handles the float parameters.
    FloatESParam* getFloatParam(int editId);
    void handleEditChangeFloat(int editId);

    DECLARE_MESSAGE_MAP()
public:
    void UpdateValues(void);
    void ResetToDefaults(void);
    void ApplyValues(void);
    void ReadValues(void);
    afx_msg void OnEnKillfocusEditEsExpweight();
    afx_msg void OnEnKillfocusEditEsMutation();
    afx_msg void OnEnKillfocusEditEsLambda();
    afx_msg void OnEnKillfocusEditEsMu();
    afx_msg void OnEnKillfocusEditEsGenerations();
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonEsApply();
    afx_msg void OnBnClickedCheckEsAutogen();
    afx_msg void OnBnClickedButtonEsGenerate();
    afx_msg void OnEnKillfocusEditEsSeed();
	afx_msg void OnBnClickedRadioEsSm();
	afx_msg void OnBnClickedRadioEsNm();
	afx_msg void OnBnClickedRadioEsLnm();
	afx_msg void OnBnClickedCheckEsLnm();
	afx_msg void OnEnKillfocusEditEsLnm();
}; // class ESDlg
