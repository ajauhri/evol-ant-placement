#include "StdAfx.h"
#include "NECSimulator.h"
#include "Geometry.h"
#include <assert.h>

/*=============================================
NECSimulator
=============================================*/
void NECSimulator::Initialize(void)
{
    char buffer[100];
    sprintf(buffer, "C:\\cygwin\\bin\\rm best*.nec");
    execute(buffer);
	sprintf(buffer, "C:\\cygwin\\bin\\rm -r ./GEN*");
    execute(buffer);
    sprintf(buffer, "C:\\cygwin\\bin\\rm -r ./Iterations");
    execute(buffer);
}

#if 0
void NECSimulator::Cleanup(unsigned int generation)
{
    char	buffer[100];
    sprintf(buffer, "C:\\cygwin\\bin\\rm ./GEN%04d/I*.nec", generation);
    execute(buffer);

    sprintf(buffer, "C:\\cygwin\\bin\\rm ./GEN%04d/I*.nou", generation);
    execute(buffer);
}
#endif

/*=============================================
NEC2Simulator
=============================================*/
void NEC2Simulator::Initialize(void)
{
    NECSimulator::Initialize();

    szAppDirectory[0]= 0;
    char szAppPath[MAX_PATH]= "";

    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

    // Extract directory
    unsigned int len = strrchr(szAppPath, '\\') - szAppPath;
    strncpy(szAppDirectory, szAppPath, len);
    szAppDirectory[len] = '\0';

}

void NEC2Simulator::Run(unsigned int index)
{
    char buffer[500];
    sprintf(buffer, "%s\\nec2++ -i ANT%02d.nec -o ANT%02d.nou", szAppDirectory, index, index);
    execute(buffer);
}

// What is this buying me?  I might need to rip this out.
void NEC2Simulator::RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count)
{
    char buffer[500];
    for (unsigned int ii = elitism; ii < batch_count; ii++)
    {
        for (unsigned int jj = 0; jj < antennas.size(); jj++)
        {
            sprintf(buffer, "%s\\nec2++ -i ./GEN%04d/I%09dA%02d.nec -o ./GEN%04d/I%09dA%02d.nou", szAppDirectory, generation, ii, jj, generation, ii, jj);
            execute(buffer);
        }
		/* for coupling */
		//sprintf(buffer, "%s\\nec2++ -i ./GEN%04d/I%09dA%02d.nec -o ./GEN%04d/I%09dA%02d.nou", szAppDirectory, generation, ii, antennas.size(), generation, ii, antennas.size());
        //execute(buffer);
    }
}

void NEC2Simulator::RunIndividual(char* infile, char* outfile)
{
    char buffer[500];
    sprintf(buffer, "%s\\nec2++ -i %s -o %s", szAppDirectory, infile, outfile);
    execute(buffer);
}

/*abaj*/
void NEC2Simulator::CopyEliteConfigurations(unsigned int generation, unsigned int elitism)
{
    char buffer[500];
    for (unsigned int ii = 0; ii < elitism; ii++)
    {
        for (unsigned int jj = 0; jj < antennas.size(); jj++)
        {
            sprintf(buffer,"C:\\cygwin\\bin\\cp ./GEN%04d/I%04d%02d.* ./GEN%04d/", generation-1, ii, jj, generation);
            execute(buffer);
        }
    }
}

/*=============================================
GNECSimulator
=============================================*/
void GNECSimulator::Initialize(void)
{
    NECSimulator::Initialize();

    char buffer[100];
    sprintf(buffer, "C:\\cygwin\\bin\\cp C:\\GNEC\\BIN\\DNEC4DMA.PAR .");
    execute(buffer);
}

void GNECSimulator::Run(unsigned int index)
{
    char buffer[100];
    sprintf(buffer, "C:\\GNEC\\BIN\\DNEC4DMA ANT%02d.nec", index);
    execute(buffer);
}

void GNECSimulator::RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count)
{
    char buffer[100];
    for (unsigned int ii = elitism; ii < batch_count; ii++)
    {
        for (unsigned int jj = 0; jj < antennas.size(); jj++)
        {
            sprintf(buffer, "C:\\GNEC\\BIN\\DNEC4DMA .\\GEN%04d\\I%04dA%02d.nec", generation, ii, jj);
            execute(buffer);
        }
    }
}

/*=============================================
HalleNECSimulator
=============================================*/
void HalleNECSimulator::Run(unsigned int index)
{
    assert(false);
}

void HalleNECSimulator::RunBatch(unsigned int generation, unsigned int elitism, unsigned int batch_count)
{


    char buffer[200];
    sprintf(buffer, "C:\\cygwin\\bin\\ssh -l ajauhri 209.129.244.12 'rm ~/tmp/*.*;");
    execute(buffer);
    sprintf(buffer, "C:\\cygwin\\bin\\scp -C ./GEN%04d/*.nec ajauhri@209.129.244.12:~/tmp/",generation);
    execute(buffer);
	    sprintf(buffer, "C:\\cygwin\\bin\\ssh -l ajauhri 209.129.244.12 'cd ~/tmp; ls *.nec | parallel -j+0 ~/nec2++.exe -i {} -o ~/tmp/{.}.nou'");
    execute(buffer);


}

void HalleNECSimulator::CreateBatchFiles(unsigned int generation, unsigned int batch_count)
{
    char	filename[40];
    char	buffer[100];
    for (unsigned int id = 0; id < batch_count; id++)
    {
        sprintf(filename, "./GEN%04d/I%04d.ll", generation, id);
        FILE* fp = fopen(filename, "w");
        if (fp != NULL)
        {
            fprintf(fp, "#!/bin/csh\n");
            fprintf(fp, "#\n");
            fprintf(fp, "# @ error = $(executable).$(jobid).err\n");
            fprintf(fp, "# @ output = $(executable).$(jobid).out\n");
            fprintf(fp, "# @ notification = never\n");
            fprintf(fp, "# @ class = serial\n");
            fprintf(fp, "# @ queue\n\n");
            for (unsigned int ii = 0; ii < antennas.size(); ii++)
            {
                fprintf(fp, "/opt/freeware/bin/echo -e \"I%04dA%02d.nec\\012I%04dA%02d.nou\" > args\n", id, ii, id, ii);	// must put the arguments in a file
                fprintf(fp, "~/NEC4/nec4dst/nec4d < args\n");
            }
            // create this file after all NEC analyses are done for a given individual
            // we use this as a flag to determine when the run is done
            fprintf(fp, "touch %04d.status\n", id);
            fclose(fp);
            // convert carriage returns
            sprintf(buffer, "C:\\cygwin\\bin\\dos2unix %s", filename);
            execute(buffer);
        }
    }
}

void HalleNECSimulator::CreateBatchScript(unsigned int batch_count)
{
    char	filename[40];
    char	buffer[100];
    sprintf(filename, "batch");
    FILE* fp = fopen(filename, "w");
    if (fp != NULL)
    {
        for (unsigned int ii = 0; ii < batch_count; ii++)
        {
            fprintf(fp, "llsubmit I%04d.ll\n", ii);
        }
        fclose(fp);
        // convert carriage returns
        sprintf(buffer, "C:\\cygwin\\bin\\dos2unix %s", filename);
        execute(buffer);
    }
}

void HalleNECSimulator::RunIndividual(char* infile, char* outfile)
{
    char buffer[500];
    sprintf(buffer, "C:\\cygwin\\bin\\ssh -l ajauhri 209.129.244.12 'rm ~/tmp/*.*;");
    execute(buffer);
    sprintf(buffer, "C:\\cygwin\\bin\\scp -C %s ajauhri@209.129.244.12:~/tmp/",infile);
    execute(buffer);

    sprintf(buffer, "C:\\cygwin\\bin\\ssh -l ajauhri 209.129.244.12 'cd ~/tmp; ls | parallel -j+0 ~/nec2++.exe -i {} -o ~/tmp/{.}.nou'");
    execute(buffer);
}


/*abaj*/
void HalleNECSimulator::CopyEliteConfigurations(unsigned int generation, unsigned int elitism)
{
    char buffer[500];
    for (unsigned int ii = 0; ii < elitism; ii++)
    {
        for (unsigned int jj = 0; jj < antennas.size(); jj++)
        {
            sprintf(buffer,"C:\\cygwin\\bin\\cp ./GEN%04d/I%09dA%02d.* ./GEN%04d/",generation-1, ii, jj, generation);
            execute(buffer);
        }
    }
}


void HalleNECSimulator::Retrieve(unsigned int generation)
{
    char	buffer[100];
    // copy NEC results file back from HALLE
    sprintf(buffer, "C:\\cygwin\\bin\\scp -C ajauhri@209.129.244.12:/home/ajauhri/tmp/*.nou ./GEN%04d/", generation);
    execute(buffer);
}

void HalleNECSimulator::Retrieve(void)
{
    char buffer[150];
    sprintf(buffer, "C:\\cygwin\\bin\\scp -C ajauhri@209.129.244.12:~/tmp/*.nou ./Iterations/");
    execute(buffer);
}
