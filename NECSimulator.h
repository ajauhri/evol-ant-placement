#pragma once
#include <assert.h>

class NECSimulator
{
public:
    virtual ~NECSimulator(void)		{};

    virtual void Initialize(void);
    virtual void CopyEliteConfigurations(unsigned int generation, unsigned int elitism) = 0;
    virtual void Run(unsigned int index) = 0;
    virtual void RunIndividual(char* infile, char* outfile)
    {
        assert(FALSE);
    }
    virtual void RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count) = 0;
    virtual bool Poll(unsigned int generation, unsigned int batch_count)
    {
        return TRUE;
    }
    virtual void Retrieve(unsigned int generation)	{};
    virtual void Retrieve(void) {};

protected:
    NECSimulator(void)		{};
};

class NEC2Simulator : public NECSimulator
{
public:
    NEC2Simulator(void)		{};
    virtual ~NEC2Simulator(void)	{};

    virtual void Initialize(void);
    virtual void CopyEliteConfigurations(unsigned int generation, unsigned int elitism);
    virtual void Run(unsigned int index);
    virtual void RunIndividual(char* infile, char* outfile);
    virtual void RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count);

private:
    char szAppDirectory[MAX_PATH];
};

class GNECSimulator : public NECSimulator
{
public:
    GNECSimulator(void)		{};
    virtual ~GNECSimulator(void)	{};

    virtual void Initialize(void);
    virtual void Run(unsigned int index);
    virtual void RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count);
};

class HalleNECSimulator : public NECSimulator
{
public:
    HalleNECSimulator(void)	{};
    virtual ~HalleNECSimulator(void)	{};

    virtual void CopyEliteConfigurations(unsigned int generation, unsigned int elitism);
    virtual void Run(unsigned int index);
    virtual void RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count);
    virtual bool Poll(unsigned int generation, unsigned int batch_count);
    virtual void Retrieve(unsigned int generation);
    virtual void Retrieve(void);
    virtual void RunIndividual(char* infile, char* outfile);

private:
    void CreateBatchFiles(unsigned int generation, unsigned int batch_count);
    void CreateBatchScript(unsigned int batch_count);
};
