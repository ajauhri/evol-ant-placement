// NECCommand.h
//

#pragma once

using namespace std;

/**********************************************************
cmdNEC
**********************************************************/
class cmdNEC
{
public:
    virtual			~cmdNEC(void)	{};

    virtual void	write(char* buffer) const = 0;
    void			operator=(const cmdNEC& other);

    bool			valid;
    int				iparams[4];
    float			fparams[6];

protected:
    cmdNEC(void);

};

inline cmdNEC::cmdNEC(void) :
    valid(false)
{
    for (unsigned int ii = 0; ii < 4; ii++) iparams[ii] = 0;
    for (unsigned int ii = 0; ii < 6; ii++) fparams[ii] = 0.0f;
}

inline void cmdNEC::operator=(const cmdNEC& other)
{
    if (this == &other) return;

    valid = other.valid;
    for (unsigned int ii = 0; ii < 4; ii++) iparams[ii] = other.iparams[ii];
    for (unsigned int ii = 0; ii < 6; ii++) fparams[ii] = other.fparams[ii];
}

/**********************************************************
GS
**********************************************************/
class cmdGS : public cmdNEC
{
public:
    cmdGS(void);
    virtual			~cmdGS(void) {};

    virtual void	write(char* buffer) const;
};

inline cmdGS::cmdGS(void) : cmdNEC()
{
    fparams[0] = 1.0f;
}

inline void cmdGS::write(char* buffer) const
{
    sprintf(buffer, "GS   0    0%10.6f", fparams[0]);
}

/**********************************************************
GN
**********************************************************/
class cmdGN : public cmdNEC
{
public:
    cmdGN(void);
    virtual			~cmdGN(void) {};

    virtual void	write(char* buffer) const;
};

inline cmdGN::cmdGN(void) : cmdNEC()
{
    iparams[0] = -1;
}

inline void cmdGN::write(char* buffer) const
{
    if (iparams[0] == -1)
        sprintf(buffer, "GN%4d", iparams[0]);
    else
        sprintf(buffer, "GN%4d%5d    0    0%10.6f%10.6f%10.6f%10.6f%10.6f%10.6f",
                iparams[0], iparams[1], fparams[0], fparams[1], fparams[2], fparams[3], fparams[4], fparams[5]);
}

/**********************************************************
GE
**********************************************************/
class cmdGE : public cmdNEC
{
public:
    cmdGE(void);
    virtual			~cmdGE(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdGE::cmdGE(void) : cmdNEC()
{
}

inline void cmdGE::write(char* buffer) const
{
    sprintf(buffer, "GE   0   -1         0");
}

/**********************************************************
CP
**********************************************************/
class cmdCP : public cmdNEC
{
public:
    cmdCP(void);
    virtual			~cmdCP(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdCP::cmdCP(void) : cmdNEC()
{
}

inline void cmdCP::write(char* buffer) const
{
    sprintf(buffer, "CP%4d%5d%5d%5d", iparams[0], iparams[1], iparams[2], iparams[3]);
}

/**********************************************************
GD
**********************************************************/
class cmdGD : public cmdNEC
{
public:
    cmdGD(void);
    virtual			~cmdGD(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdGD::cmdGD(void) : cmdNEC()
{
}

inline void cmdGD::write(char* buffer) const
{
    sprintf(buffer, "GD%4d    0    0    0%10.6f%10.6f%10.6f%10.6f", iparams[0], fparams[0], fparams[1], fparams[2], fparams[3]);
}

/**********************************************************
NT
**********************************************************/
class cmdNT : public cmdCP
{
public:
    cmdNT(void);
    virtual			~cmdNT(void)	{};

    virtual void	write(char* buffer) const;

    float y11r;
    float y11i;
    float y12r;
    float y12i;
    float y22r;
    float y22i;
};

inline cmdNT::cmdNT(void) : cmdCP()
{
}

inline void cmdNT::write(char* buffer) const
{
    sprintf(buffer, "NT%4d%5d%5d%5d%10.6f%10.6f%10.6f%10.6f%10.6f%10.6f",
            iparams[0], iparams[1], iparams[2], iparams[3], fparams[0], fparams[1], fparams[2], fparams[3], fparams[4], fparams[5]);
}

/**********************************************************
LD
**********************************************************/
class cmdLD : public cmdNEC
{
public:
    cmdLD(void);
    virtual			~cmdLD(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdLD::cmdLD(void) : cmdNEC()
{
}

inline void cmdLD::write(char* buffer) const
{
    sprintf(buffer, "LD%4d%5d%5d%5d%10.6f%10.6f%10.6f", iparams[0], iparams[1], iparams[2], iparams[3], fparams[0], fparams[1], fparams[2]);
}

/**********************************************************
LE
**********************************************************/
class cmdLE : public cmdNEC
{
public:
    cmdLE(void);
    virtual			~cmdLE(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdLE::cmdLE(void) : cmdNEC()
{
}

inline void cmdLE::write(char* buffer) const
{
    sprintf(buffer, "LE%4d%5d%5d%5d%10.6f%10.6f%10.6f%10.6f%10.6f%10.6f",
            iparams[0], iparams[1], iparams[2], iparams[3], fparams[0], fparams[1], fparams[2], fparams[3], fparams[4], fparams[5]);
}

/**********************************************************
NE
**********************************************************/
class cmdNE : public cmdNEC
{
public:
    cmdNE(void);
    virtual			~cmdNE(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdNE::cmdNE(void) : cmdNEC()
{
}

inline void cmdNE::write(char* buffer) const
{
    sprintf(buffer, "NE%4d%5d%5d%5d%10.6f%10.6f%10.6f%10.6f%10.6f%10.6f",
            iparams[0], iparams[1], iparams[2], iparams[3], fparams[0], fparams[1], fparams[2], fparams[3], fparams[4], fparams[5]);
}

/**********************************************************
TL
**********************************************************/
class cmdTL : public cmdNEC
{
public:
    cmdTL(void);
    virtual			~cmdTL(void)	{};

    virtual void	write(char* buffer) const;
};

inline cmdTL::cmdTL(void) : cmdNEC()
{
}

inline void cmdTL::write(char* buffer) const
{
    sprintf(buffer, "TL%4d%5d%5d%5d%10.6f%10.6f%10.6f%10.6f%10.6f%10.6f",
            iparams[0], iparams[1], iparams[2], iparams[3], fparams[0], fparams[1], fparams[2], fparams[3], fparams[4], fparams[5]);
}

