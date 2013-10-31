// EADlg.h
//
// This file defines the EADlg class that is reponsible for handling the Evolutionary Algorithm Settings dialog.
// The parameters are divided into two arrays, one for integers, another for floats.  Using those two
// data structures, makes it easy to have a data-driven UI where one can plug in a new parameter, in a few
// steps described in in the beginning of EADlg.cpp

#pragma once

typedef struct IntParam
{
    // valuePtr points to the value that is causally relevant.
    unsigned int *valuePtr;
    // The following values are used exclusively by the GUI.
    // (Their manipulation doesn't change anything but the GUI; they're causally irrelevant.)
    unsigned int defaultValue;
    unsigned int minValue;
    unsigned int maxValue;
    int textControl;
    int otherControl;
    unsigned int value;
    // If dirty, the GUI should be updated.
    bool dirty;
} IntParam;

typedef struct FloatParam
{
    float *valuePtr;
    // These are the values the GUI uses.
    float defaultValue;
    float minValue;
    float maxValue;
    int textControl;
    int otherControl;
    float value;
    // If dirty, the GUI should be updated.
    bool dirty;
} FloatParam;

class EADlg : public CDialog
{
    DECLARE_DYNAMIC(EADlg)
public:
    EADlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~EADlg();

// Dialog Data
    enum { IDD = IDD_EA_DIALOG };

    void ReadValues(void);
    void UpdateValues();
    void ResetToDefaults(void);
    void ApplyValues(void);

private:
    bool updating;
    //bool stdGA;
    bool autogen;
	bool ln_mutation;
	bool use_default_resolution;
    void updatePopDepValues(void);
    void EnableSTDGA(unsigned representation);
	void HandleMutationType(unsigned int type);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK(void);
    DECLARE_MESSAGE_MAP()

    // Handles the integer parameters.
    IntParam* getIntParam(int editId);
    void handleEditChange(int editId);

    // Handles the float parameters.
    FloatParam* getFloatParam(int editId);
    void handleEditChangeFloat(int editId);
public:
    afx_msg void OnBnClickedButton2();
    afx_msg void OnEnChangeEditGen();
    afx_msg void OnEnChangeEditToursize();
    afx_msg void OnEnChangeEditElitism();
    afx_msg void OnEnChangeEditPopsize();
    afx_msg void OnEnChangeEditLayers();
    afx_msg void OnEnChangeEditLayersize();
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnBnClickedRadioSTDGA();
    afx_msg void OnBnClickedRadioALPSEA();
    afx_msg void OnEnKillfocusEditMutation();
    afx_msg void OnEnKillfocusEditRecomb();
    afx_msg void OnBnClickedCheckAutogen();
    afx_msg void OnBnClickedButtonGenerate();
    afx_msg void OnCbnKillfocusComboRepr();
    afx_msg void OnEnKillfocusEditAgegap();
    afx_msg void OnEnKillfocusEditExpweight();
    afx_msg void OnBnClickedRadioExhaustive();
	afx_msg void OnBnClickedRadioGaSm();
	afx_msg void OnBnClickedRadioGaNm();
	afx_msg void OnEnKillfocusEditGaLnm();
	afx_msg void OnBnClickedRadioGaLnm();
	afx_msg void OnBnClickedCheckGaLnm();
};
