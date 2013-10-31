#pragma once

#include <vector>
#include <string.h>
#include <GL\gl.h>
#include "APOT6.H"
#include "cmdNEC.H"

using namespace std;

class antenna;

//#define Z	0.746f
#define MAX_NEC_DECK_LINES				1000
#define MAX_NEC_DECK_LINE_LENGTH		100

extern char	g_nec_deck[MAX_NEC_DECK_LINES][MAX_NEC_DECK_LINE_LENGTH];
extern int	g_nec_line;
extern int	g_nec_segment;

typedef struct analysis_params
{
    unsigned int min_theta;
    unsigned int step_theta;
    unsigned int incr_theta;
    unsigned int min_phi;
    unsigned int step_phi;
    unsigned int incr_phi;
    float min_freq;
    unsigned int step_freq;
    unsigned int incr_freq;
} analysis_params;

// Need to handle CP, GN, GE, GS, GD, NT, LD, LE, NE, TL
#define CONTROL_COUNT 10
static char* sControlCodes[CONTROL_COUNT] = { "GS", "GE", "GN", "CP", "GD", "NT", "LD", "LE", "NE", "TL" };
// WARNING - NEC is very sensitive to the ordering of these cards (e.g. GS must come before GE, GE before GN, etc.)
#define GS	0
#define GE	1
#define GN	2
#define CP	3
#define GD	4
#define NT	5
#define LD	6
#define LE	7
#define NE	8
#define TL	9

class point
{
public:
    point(void);
    point(float xx, float yy, float zz);
    point(const point& other);
    ~point(void)	{};

    void normalize(void);
    void cross(const point& pt);
    float dot(const point& other) const;
    void set(float xx, float yy, float zz);
    float length(void) const;
    float length2(void) const;
    float distance(const point& other) const;
    float distance2(const point& other) const;
    void add(const point& other);
    void sub(const point& other);
    void scale(float s);
    void negate(void);
    float angle(const point& v1) const;
    point& operator=(const point& other);
    bool operator==(const point& other) const;

    void normal(const point& p0, const point& p1, const point& p2);

    float x;
    float y;
    float z;
	float dist_from_centroid;
};

// This is Greg's test change.

class plane
{
public:
    plane(void);
    plane(point pts[]);
    ~plane(void)	{};

    bool includes(const point& pt) const;

    point orientation;
    float konstant;
};

class wire
{
public:
    wire(void)	{};
    wire(const wire& other);
    ~wire(void)	{};

    bool intersects(const wire& wr, point& pt) const;
    void offset(point& where);

    point a;
    point b;
    int segments;
    float diameter;
};

class displayhandle
{
public:
    displayhandle(void);
    displayhandle(GLuint lst);
    ~displayhandle(void);

    GLuint	list;
    int		count;
};

class pattern  		// radiation pattern at a given frequency
{
public:
    pattern(unsigned int n, float freq);
    pattern(const pattern& other);
    ~pattern(void);

    void print(void) const;
    void build_display(const antenna& ant, float min_db, float max_db, float scale = 1.0f);
    void draw(void);

    float*			db_gain;
    float*			quad_db;
    float			frequency;
    unsigned int	db_count;
    displayhandle*	dlist;
    unsigned int	id;
    bool			display;

private:
    pattern(void);

    static unsigned int	g_id;
};

class evaluation  	// set of radiation patterns over a range of frequencies
{
public:
    evaluation(void);
    evaluation(const evaluation& other);
    ~evaluation(void);

    void print(void) const;

    static float compare(const evaluation& first, const evaluation& second, float weight);

    vector<pattern*>	radiation;
    float				max_db;
    float				min_db;
};

class antenna
{
public:
    antenna(void);
    antenna(const antenna& other);
    ~antenna(void);

    void offset(point& where);
    void load(void);
    void lines2points(void);
    void translate2deck(const point& offset) const;
    void analyze(bool verbose = false);
    int  exists(const point& pt) const;
    void erase_points(void);
    void init_control_params(void);
    void append_analysis(int excitation) const;
	void append_coupling_cards(vector<int> *tags, unsigned int antenna_id) const;
	void calculate_centroid(void);
	void calculate_dist_from_centroid(void);
    point min_point(void) const;
    point max_point(void) const;

    int  num_polar(void) const;

    char				label[200];
    vector<wire*>		wires;
    vector<point*>		*points;
    evaluation			results;
    point				color;
    analysis_params		params;
    cmdNEC*				cmds[CONTROL_COUNT];
    vector<point*>		redo_points;
    vector<point*>		undo_points;
	point				centroid;
	bool isPlatform;
};

#define PI		3.14159265f
#define HALF_PI	1.570796325f
#define TWO_PI	6.2831853f

#define NUM_COLORS	5

extern antenna*			platform;
extern vector<antenna*>	antennas;
extern vector<antenna*>	antennasEA;
extern vector<antenna*> antennasSA;
extern vector<antenna*> antennasHC;
extern vector<antenna*> antennasES;
extern point			kColors[NUM_COLORS];

extern float mesh_resolution;
extern float default_mesh_resolution;

extern bool antennas_valid(void);
extern void clear_nec_deck(void);
extern void print_nec_deck(const char* filename);
extern unsigned int read_nec_results(const char* filename, evaluation& results, antenna& ant);
extern float read_coupling_results(const char *filename, int size);
extern void execute(const char* command);

inline 
void
DEBUG_TRACE(char *fmt, ...)
{
    va_list argptr;
    char buffer[100];

    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
    va_end(argptr);

    ::OutputDebugString(buffer);
}
