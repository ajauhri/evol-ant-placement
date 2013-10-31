#include "stdafx.h"
#include "AntennaConfiguration.h"
#include "APOT6Dlg.h"
#include "SA.h"
#include "HC.h"
#include "EvolutionaryStrategy.h"

extern "C"
{
#include "luaShell.c"

    int printMessage (lua_State *lua);
    int getGAInfo (lua_State *lua);
    int setGAInfo (lua_State *lua);
    int loadPlatform (lua_State *lua);
    int addAntenna (lua_State *lua);

    int setSAInfo (lua_State *lua);
    int getSAInfo (lua_State *lua);

    int setHCInfo (lua_State *lua);
    int getHCInfo (lua_State *lua);

    int setESInfo (lua_State *lua);
    int getESInfo (lua_State *lua);


    //int setAlgorithm(lua_State *lua);
}

static CAPOT6Dlg* myInstance;

// simple command that integrates with the Lua environment.
static int printMessage (lua_State *lua)
{
    assert (lua_isstring (lua,1));

    const char *msg = lua_tostring (lua, 1);

    cout << "script: " << msg << endl;
    return 0;
}

double getfield (lua_State *L, const char *index)
{
    lua_pushstring(L, index);
    lua_gettable(L, 1);   // This pops "size", gets the value of "size" in the table and pushes the value
    double value = lua_tonumber(L, -1);   // Get the returned value off the stack
    lua_pop(L, 1);
    return value;
}

bool getboolfield (lua_State *L, const char *index)
{
    lua_pushstring(L, index);
    lua_gettable(L, 1);   // This pops "size", gets the value of "size" in the table and pushes the value
    bool value = lua_toboolean(L, -1);   // Get the returned value off the stack
    lua_pop(L, 1);
    return value;
}

char* getstringfield (lua_State *L, const char *index)
{
    lua_pushstring(L, index);
    lua_gettable(L, 1);   // This pops "size", gets the value of "size" in the table and pushes the value
    char* value = (char *) lua_tostring(L, -1);   // Get the returned value off the stack
    lua_pop(L, 1);
    return value;
}

/* assume that table is at the top */
void setfield (lua_State *L, const char *index, double value)
{
    lua_pushstring(L, index);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}
void setboolfield (lua_State *L, const char *index, bool value)
{
    lua_pushstring(L, index);
    lua_pushboolean(L, value);
    lua_settable(L, -3);
}

void setstringfield (lua_State *L, const char *index, char *value)
{
    lua_pushstring(L, index);
    lua_pushstring(L, value);
    lua_settable(L, -3);
}

static int setAlgorithm(lua_State *L)
{
    assert (lua_isnumber (L,1));
    unsigned int algo_type = lua_tonumber (L,1);
    cout << "load algortithm: " << algo_type << endl;
    myInstance->SetAlgorithm(algo_type);
    return 0;
}




static int getSAInfo (lua_State *L)
{
    lua_newtable(L);
    setfield(L, "iterations", SAInfo.iterations);
    setfield(L, "mutationProbability", SAInfo.mutationProbability);
    setfield(L, "seed", SAInfo.seed);
	
    setfield(L, "initialTemp", SAInfo.initialTemp);

    setfield(L, "tempFactor", SAInfo.tempFactor);
    setfield(L, "iterationsPerTempChng", SAInfo.iterationsPerTempChng);
    setfield(L, "outputFreq", SAInfo.outputFreq);
    setfield(L, "convergenceFactor", SAInfo.convergenceFactor);

    setfield(L, "genCondition", SAInfo.genCondition);
    setfield(L, "fitnessCondition", SAInfo.fitnessCondition);
    setfield(L, "expWeight", SAInfo.expWeight);
	setfield(L, "mutationType", SAInfo.mutationType);
	return 1;
}


static int setSAInfo (lua_State *L)
{
    /* 1st argument must be a table (t) */
    luaL_checktype(L, 1, LUA_TTABLE);
    SAInfo.iterations = getfield(L, "iterations");
    SAInfo.mutationProbability = getfield(L, "mutationProbability");
    SAInfo.seed = getfield(L, "seed");
	SAInfo.initialTemp = getfield(L, "initialTemp");

    SAInfo.tempFactor = getfield(L, "tempFactor");
    SAInfo.iterationsPerTempChng = getfield(L, "iterationsPerTempChng");
    SAInfo.outputFreq = getfield(L, "outputFreq");
    SAInfo.convergenceFactor = getfield(L, "convergenceFactor");

    SAInfo.genCondition = getfield(L, "genCondition");
    SAInfo.fitnessCondition = getfield(L, "fitnessCondition");
    SAInfo.expWeight = getfield(L, "expWeight");
	SAInfo.mutationType = getfield(L, "mutationType");
	return 0;  /* no results */
}

static int setGAInfo (lua_State *L)
{
    /* 1st argument must be a table (t) */
    luaL_checktype(L, 1, LUA_TTABLE);
    EAInfo.popSize = getfield(L, "popSize");
    EAInfo.generations = getfield(L, "generations");
    EAInfo.elitism = getfield(L, "elitism");
    EAInfo.tournamentSize = getfield(L, "tournamentSize");

    EAInfo.algorType = getfield(L, "algorType");
    EAInfo.layers = getfield(L, "layers");
    EAInfo.layerSize = getfield(L, "layerSize");
    EAInfo.ageGap = getfield(L, "ageGap");

    EAInfo.mutation = getfield(L, "mutation");
    EAInfo.recombination = getfield(L, "recombination");
    EAInfo.expWeight = getfield(L, "expWeight");
    EAInfo.autoSeed = getboolfield(L, "autoSeed");
    EAInfo.seed = (unsigned) getfield(L, "seed");
	EAInfo.mutationType = getfield(L, "mutationType");
    return 0;  /* no results */
}

static int getGAInfo (lua_State *L)
{
    lua_newtable(L);
    setfield(L, "popSize", EAInfo.popSize);
    setfield(L, "generations", EAInfo.generations);
    setfield(L, "elitism", EAInfo.elitism);
    setfield(L, "tournamentSize", EAInfo.tournamentSize);

    setfield(L, "algorType", EAInfo.algorType);
    setfield(L, "layers", EAInfo.layers);
    setfield(L, "layerSize", EAInfo.layerSize);
    setfield(L, "ageGap", EAInfo.ageGap);

    setfield(L, "mutation", EAInfo.mutation);
    setfield(L, "recombination", EAInfo.recombination);
    setfield(L, "expWeight", EAInfo.expWeight);

    setboolfield(L, "autoSeed", EAInfo.autoSeed);
	setfield(L, "seed", EAInfo.seed);
	setfield(L, "mutationType", EAInfo.mutationType);
    setstringfield(L, "username", (char *) (LPCTSTR) EAInfo.username);
    setstringfield(L, "password", (char *) (LPCTSTR) EAInfo.password);
    return 1;
}



static int setHCInfo (lua_State *L)
{
    /* 1st argument must be a table (t) */
    luaL_checktype(L, 1, LUA_TTABLE);
    HCInfo.iterations = getfield(L, "iterations");
    HCInfo.mutationProbability = getfield(L, "mutationProbability");
    HCInfo.seed = getfield(L, "seed");
	  

    HCInfo.outputFreq = getfield(L, "outputFreq");
    HCInfo.convergenceFactor = getfield(L, "convergenceFactor");

    HCInfo.genCondition = getfield(L, "genCondition");
    HCInfo.fitnessCondition = getfield(L, "fitnessCondition");
	HCInfo.mutationType = getfield(L, "mutationType");

    HCInfo.expWeight = getfield(L, "expWeight");
    return 0;  /* no results */
}



static int getHCInfo (lua_State *L)
{
    lua_newtable(L);
    setfield(L, "iterations", HCInfo.iterations);
    setfield(L, "mutationProbability", HCInfo.mutationProbability);
    setfield(L, "seed", HCInfo.seed);
	
    setfield(L, "outputFreq", HCInfo.outputFreq);
    setfield(L, "convergenceFactor", HCInfo.convergenceFactor);

    setfield(L, "genCondition", HCInfo.genCondition);
    setfield(L, "fitnessCondition", HCInfo.fitnessCondition);
	setfield(L, "mutationType", HCInfo.mutationType);

    setfield(L, "expWeight", HCInfo.expWeight);

    return 1;
}

static int setESInfo (lua_State *L)
{
    /* 1st argument must be a table (t) */
    luaL_checktype(L, 1, LUA_TTABLE);
	ESInfo.generations = getfield(L, "generations");
    ESInfo.mu = getfield(L, "mu");
    ESInfo.mutation_probability = getfield(L, "mutation_probability");
    ESInfo.seed = getfield(L, "seed");
    ESInfo.lambda = getfield(L, "lambda");
    ESInfo.autoSeed = getfield(L, "autoSeed");
	ESInfo.mutationType = getfield(L, "mutationType");
    ESInfo.expWeight = getfield(L, "expWeight");
	return 0;
}

static int getESInfo (lua_State *L)
{
    lua_newtable(L);
    setfield(L, "generations", ESInfo.generations);
    setfield(L, "mutation_probability", ESInfo.mutation_probability);
    setfield(L, "seed", ESInfo.seed);
    setfield(L, "mu", ESInfo.mu);
    setfield(L, "lambda", ESInfo.lambda);
    setfield(L, "expWeight", ESInfo.expWeight);
    setboolfield(L, "autoSeed", ESInfo.autoSeed);
	setfield(L, "mutationType", ESInfo.mutationType);
	return 1;
}


static int loadPlatform(lua_State *L)
{
    assert (lua_isstring (L,1));
    const char *file = lua_tostring (L, 1);
    cout << "load platform: " << file << endl;
    myInstance->LoadPlatform(CString(file));
    return 0;
}

static int addAntenna(lua_State *L)
{
    assert (lua_isstring (L,1));
    const char *file = lua_tostring (L, 1);
    cout << "load antenna: " << file << endl;
    myInstance->AddAntenna(CString(file));
    return 0;
}

static int addPoint(lua_State *L)
{
    double x = lua_tonumber (L, 1);
    double y = lua_tonumber (L, 2);
    double z = lua_tonumber (L, 3);

    point p(x, y, z);
    myInstance->AddPointToLastAntenna(&p);
    return 0;
}

static int setLocalRun(lua_State *L)
{
    bool b = lua_toboolean (L, 1);
    myInstance->SetLocalRun(b);
    return 0;
}


static int startRun(lua_State *L)
{
    myInstance->StartEA();
    return 0;
}

static int stopRun(lua_State *L)
{
    myInstance->StopSearch();
    return 0;
}

static lua_State *L = NULL;

void initLua()
{
    if (L)
    {
        // Already initialized.
        return;
    }
    //shell_main(1, argv);
    /* Create a new Lua state. luaL_newstate is an utility function that calls lua_newstate() properly */
    L = luaL_newstate();
    /* Open the standard Lua libraries... such as table, string, math, and the base functions. You don't really need this... but you most definitely will want to call this */
    luaL_openlibs(L);
    //luaL_loadfile(L, "tprint.lua");
//	lua_pushcclosure (L, printMessage, 0);
//	lua_setglobal (L, "trace");

    lua_pushcclosure (L, getGAInfo, 0);
    lua_setglobal (L, "getGAInfo");

    lua_pushcclosure (L, setGAInfo, 0);
    lua_setglobal (L, "setGAInfo");

    lua_pushcclosure (L, getSAInfo, 0);
    lua_setglobal (L, "getSAInfo");

    lua_pushcclosure (L, setSAInfo, 0);
    lua_setglobal (L, "setSAInfo");

    lua_pushcclosure (L, getHCInfo, 0);
    lua_setglobal (L, "getHCInfo");

    lua_pushcclosure (L, setHCInfo, 0);
    lua_setglobal (L, "setHCInfo");

    lua_pushcclosure (L, getESInfo, 0);
    lua_setglobal (L, "getESInfo");

    lua_pushcclosure (L, setESInfo, 0);
    lua_setglobal (L, "setESInfo");

    lua_pushcclosure (L, loadPlatform, 0);
    lua_setglobal (L, "loadPlatform");

    lua_pushcclosure (L, addAntenna, 0);
    lua_setglobal (L, "addAntenna");

    lua_pushcclosure (L, addPoint, 0);
    lua_setglobal (L, "addPoint");

    lua_pushcclosure (L, setLocalRun, 0);
    lua_setglobal (L, "setLocalRun");

    lua_pushcclosure (L, startRun, 0);
    lua_setglobal (L, "startRun");

    lua_pushcclosure (L, stopRun, 0);
    lua_setglobal (L, "stopRun");

    lua_pushcclosure (L, setAlgorithm, 0);
    lua_setglobal (L, "setAlgorithm");

}

void closeLua()
{
    /* Clean up. This must be called at end */
    lua_close(L);
    L = NULL;
}

void loadLuaLibrary(char *name)
{
    dofile(L, name);
}


UINT RunCLI(LPVOID pParam)
{
    myInstance = (CAPOT6Dlg*) pParam;
    initLua();
    print_version();
    dotty(L);  // This loops and blocks on IO.
    /*
    	char line[255];
    	while(1) {
    		puts("> ");
    		fflush(stdout);
    		gets(line);
    		//int status = luaL_loadbuffer(L, line, strlen(line), "cli") || docall(L, 0, 1);
    		//printf("status = %d\n", status);
    		fflush(stdout);
    	}
    */
    /* Load & run hello.lua, without error checking. This will generate a warning in gcc because you're not checking the return value (luaL_dofile is a macro) */
    //luaL_dofile(L, "hello.lua"); /* Note: all lua functions begin with L as the first arg - think of this as self in Lua scripts. this is so you can have several lua instances at the same time */
    closeLua();

    return 0;
}