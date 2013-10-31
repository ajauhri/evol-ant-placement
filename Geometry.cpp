#include "stdafx.h"
#include "Geometry.h"
#include "AntennaConfiguration.h"
#include <math.h>
#include <assert.h>
#include <float.h>

#define MIN_DISTANCE 0.0001f

antenna*			platform = NULL;
vector<antenna*>	antennas;
vector<antenna*>	antennasEA;
vector<antenna*>    antennasSA;
vector<antenna*>	antennasHC;
vector<antenna*>	antennasES;
point				kColors[NUM_COLORS];

/* for local neighborhood mutation */
float				mesh_resolution = 0.0f;
float				default_mesh_resolution = 0.0f;

char		g_nec_deck[MAX_NEC_DECK_LINES][MAX_NEC_DECK_LINE_LENGTH];
int			g_nec_line;
int			g_nec_segment;

inline 
float 
MIN(float a, float b)
{
    return (a < b) ? a : b;
};

inline 
float 
MAX(float a, float b)
{
    return (a > b) ? a : b;
};

inline 
float
DB2POWER(float db)
{
    return pow(10.0, db/10.0);		// converts from decibel back to the power ratio
}

inline 
float
POWER2DB(float power)
{
    return 10.0 * log10(power);		// converts a power ratio into decibels
}

inline 
float 
DEGREES2RADIANS(int degrees)
{
    return (float) degrees * PI / 180.0f;
}

point::point(void)
{
    x = y = z = 0.0f;
	dist_from_centroid = 0.0f;
}

point::point(float xx, float yy, float zz)
{
    x = xx;
    y = yy;
    z = zz;
	dist_from_centroid = 0.0f;
}

point::point(const point& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
	dist_from_centroid = other.dist_from_centroid;
}

void 
point::normalize(void)
{
    float mag = length();
    x /= mag;
    y /= mag;
    z /= mag;
}

float 
point::angle(const point& v1) const
{
    // return (float)Math.acos(dot(v1)/v1.length()/v.length());
    // Numerically, near 0 and PI are very bad condition for acos.
    // In 3-space, |atan2(sin,cos)| is much stable.
    float xx = y*v1.z - z*v1.y;
    float yy = z*v1.x - x*v1.z;
    float zz = x*v1.y - y*v1.x;
    float w = xx*xx + yy*yy + zz*zz;
    float cr = (w > 0.0f) ? sqrt(w) : 0.0f;

    return fabs(atan2(cr, dot(v1)));
}

float 
point::dot(const point& other) const
{
    return x * other.x + y * other.y + z * other.z;
}

void 
point:: cross(const point& b)
{
    float ax = x;
    float ay = y;
    float az = z;

    x = ay * b.z - az * b.y;
    y = az * b.x - ax * b.z;
    z = ax * b.y - ay * b.x;
}

float 
point::length(void) const
{
    return sqrt(length2());
}

float 
point::length2(void) const
{
    return (x*x+y*y+z*z);
}

float 
point::distance(const point& other) const
{
    return sqrt(distance2(other));
}

float 
point::distance2(const point& other) const
{
    float xx = x - other.x;
    float yy = y - other.y;
    float zz = z - other.z;
    return (xx*xx+yy*yy+zz*zz);
}

void 
point::scale(float s)
{
    x *= s;
    y *= s;
    z *= s;
}

void 
point::negate(void)
{
    x = -x;
    y = -y;
    z = -z;
}

void 
point::set(float xx, float yy, float zz)
{
    x = xx;
    y = yy;
    z = zz;
}

void 
point::add(const point& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

void 
point::sub(const point& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

point&
point::operator=(const point& other)
{
    if (this != &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return *this;
}

bool 
point::operator==(const point& other) const
{
    if (this != &other)
    {
        return (distance2(other) < MIN_DISTANCE);
#if 0
        return (x == other.x && y == other.y == z == other.z);
#endif
    }
    return true;
}

void 
point::normal(const point& p0, 
			const point& p1, 
			const point& p2)
{
    point v1(p1);
    point v2(p2);

    v1.sub(p0);
    v2.sub(p0);

    v1.cross(v2);
    *this = v1;
    normalize();
}


plane::plane(void)
{
    orientation.x = 0.0f;
    orientation.y = 1.0f;
    orientation.z = 0.0f;
    konstant = 0.0f;
}

plane::plane(point pts[])
{
    point v1(pts[1]);
    point v2(pts[2]);

    v1.sub(pts[0]);
    v2.sub(pts[0]);

    v1.cross(v2);

    orientation = v1;
    orientation.normalize();
    konstant = -(orientation.x * pts[0].x + orientation.y * pts[0].y + orientation.z * pts[0].z);
}

bool 
plane::includes(const point& pt) const
{
    return (fabs(orientation.x * pt.x + orientation.y * pt.y + orientation.z * pt.z + konstant) < MIN_DISTANCE);
}

wire::wire(const wire& other)
{
    a = other.a;
    b = other.b;
    segments = other.segments;
    diameter = other.diameter;
}

void 
wire::offset(point& where)
{
    a.add(where);
    b.add(where);
}

bool 
wire::intersects(const wire& wr, 
				point& pt) const
{
    /*
       Calculate the line segment PaPb that is the shortest route between
       two lines P1P2 and P3P4. Return FALSE if no solution exists.
    */
    point p13, p43, p21, p24, pa, pb;
    float d1343, d4321, d1321, d4343, d2121;
    float numer, denom, mua, mub;

    p13.x = a.x - wr.a.x;
    p24.x = b.x - wr.b.x;
    if ((p13.x > 0 && p24.x > 0) || (p13.x < 0 && p24.x < 0)) return false;
    p13.y = a.y - wr.a.y;
    p24.y = b.y - wr.b.y;
    if ((p13.y > 0 && p24.y > 0) || (p13.y < 0 && p24.y < 0)) return false;

    p13.z = a.z - wr.a.z;
    p24.z = b.z - wr.b.z;
    if ((p13.z > 0 && p24.z > 0) || (p13.z < 0 && p24.z < 0)) return false;

    p43.x = wr.b.x - wr.a.x;
    p43.y = wr.b.y - wr.a.y;
    p43.z = wr.b.z - wr.a.z;
    if (fabs(p43.x)  < MIN_DISTANCE && fabs(p43.y)  < MIN_DISTANCE && fabs(p43.z)  < MIN_DISTANCE) return false;

    p21.x = b.x - a.x;
    p21.y = b.y - a.y;
    p21.z = b.z - a.z;
    if (fabs(p21.x)  < MIN_DISTANCE && fabs(p21.y)  < MIN_DISTANCE && fabs(p21.z)  < MIN_DISTANCE) return false;

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;
    if (fabs(denom) < MIN_DISTANCE) return false;
    numer = d1343 * d4321 - d1321 * d4343;

    mua = numer / denom;
    mub = (d1343 + d4321 * mua) / d4343;

    pa.x = a.x + mua * p21.x;
    pa.y = a.y + mua * p21.y;
    pa.z = a.z + mua * p21.z;
    pb.x = wr.a.x + mub * p43.x;
    pb.y = wr.a.y + mub * p43.y;
    pb.z = wr.a.z + mub * p43.z;

    if (pb.distance2(pa) < MIN_DISTANCE) return false;

    pt = pa;
    return true;
}

displayhandle::displayhandle(void) :
    count(0),
    list(0)
{
}

displayhandle::displayhandle(GLuint lst) :
    count(0),
    list(0)
{
    if (lst != 0)
    {
        list = lst;
        count = 1;
    }
}

displayhandle::~displayhandle(void)
{
    assert(count == 0);
    glDeleteLists(list, 1);
}

unsigned int	pattern::g_id = 1;

pattern::pattern(void) :
    db_count(0),
    db_gain(NULL),
    quad_db(NULL),
    frequency(0.0f),
    dlist(NULL),
    display(true)
{
    id = pattern::g_id++;
}

pattern::pattern(unsigned int n, float freq) :
    db_count(n),
    db_gain(NULL),
    quad_db(NULL),
    frequency(freq),
    dlist(NULL),
    display(true)
{
    id = pattern::g_id++;
    if (db_count > 0)
    {
        db_gain = new float[db_count];
        quad_db = new float[db_count];
    }
}

pattern::pattern(const pattern& other) :
    db_count(other.db_count),
    db_gain(NULL),
    quad_db(NULL),
    frequency(other.frequency),
    display(other.display)
{
    dlist = other.dlist;	// pointer copy
    id = other.id;
    if (dlist != NULL) dlist->count++;

    if (other.db_gain != NULL)
    {
        db_gain = new float[db_count];
        quad_db = new float[db_count];
        for (unsigned int ii = 0; ii < db_count; ii++)
        {
            db_gain[ii] = other.db_gain[ii];
            quad_db[ii] = other.quad_db[ii];
        }
    }
}

pattern::~pattern(void)
{
    if (db_gain != NULL) 
		delete[] db_gain;
    if (quad_db != NULL) 
		delete[] quad_db;
    if ((dlist != NULL) && (--(dlist->count) == 0))
    {
        delete dlist;
    }
}

void 
pattern::print(void) const
{
    DEBUG_TRACE("\t%d kHz\n", frequency);
    DEBUG_TRACE("\tdisplay is %s\n", (display ? "ON" : "OFF"));
    DEBUG_TRACE("\tN pts = %d\n", db_count);
}

void 
pattern::build_display(const antenna& ant,
						float min_db, 
						float max_db, 
						float scale)
{
    assert(dlist == NULL);	// not already created
    dlist = new displayhandle;

    int nphi		= ant.params.step_phi;
    int ntheta		= ant.params.step_theta;
    int phi_inc		= ant.params.incr_phi;
    int theta_inc	= ant.params.incr_theta;
    int phi_step	= ant.params.step_phi;
    int theta_step	= ant.params.step_theta;

    dlist->list = glGenLists(1);
    assert(dlist->list != 0);
    glNewList(dlist->list, GL_COMPILE);
    dlist->count = 1;

//	float min_power = DB2POWER(min_db);
//	float max_power = DB2POWER(max_db);

//	float db_range = max_db - min_db;
    float max_range = exp((max_db) / 10.0) * scale;

    int npts = (nphi+1) * ntheta;

    point*	cartesian = new point[npts];
    point*	normals = new point[npts];
    float*	colors = new float[npts];
    point*	pts = cartesian;
    float*	cls = colors;

    int phi	= ant.params.min_phi;
    for (int phi_counter = 0; phi_counter < nphi; phi_counter++)
    {

        float tphi	= DEGREES2RADIANS(phi);
        float cp	= cos(tphi);
        float sp	= sin(tphi);

        int theta = ant.params.min_theta;
        for (int theta_counter = 0; theta_counter < ntheta; theta_counter++)
        {

            float ttheta = DEGREES2RADIANS(theta);

            float amplitude = exp(db_gain[phi_counter*ntheta+theta_counter] / 10.0) * scale;

            float x = sin(ttheta) * cp	* amplitude;
            float y = cos(ttheta) * cp	* amplitude;
            float z = sp				* amplitude;
            pts->set(x, y, z);

            *cls++ = amplitude / max_range;
            theta += theta_inc;
            pts++;
        }
        phi += phi_inc;
    }

    point*	nm = normals;
    int		idx0, idx1, idx2, idx3;
    point	pt0, pt1, pt2, pt3;
#if NET_ENVIRON
    for (int phi_counter = 0; phi_counter < nphi; phi_counter++)
    {
#else
    for (phi_counter = 0; phi_counter < nphi; phi_counter++)
    {
#endif
        phi = ant.params.min_phi;
        for (int theta_counter = 0; theta_counter < ntheta; theta_counter++)
        {

            idx0 = phi_counter * ntheta + theta_counter;
            pt0 = cartesian[idx0];

            if (phi_counter == nphi-1)	// below
                idx1 = theta_counter;
            else
                idx1 = idx0 + ntheta;
            /*
            			if (phi_counter == 0)	// above
            				idx2 = nphi * ntheta + theta_counter - ntheta;
            			else
            				idx2 = idx0 - ntheta;
            */
            if (theta_counter == ntheta-1) // right
                idx3 = phi_counter * ntheta;
            else
                idx3 = idx0 + 1;

            /*			if (theta_counter == 0) // left
            				idx4 = phi_counter * ntheta + ntheta-1;
            			else
            				idx4 = idx0 - 1;
            */
            pt1 = cartesian[idx1];
            pt3 = cartesian[idx3];

            nm->normal(pt0, pt1, pt3);
            nm->normalize();
            nm++;
        }
        phi += phi_inc;
    }

    idx0 = nphi * ntheta;
    for (int theta_counter = 0; theta_counter < ntheta; theta_counter++, idx0++)
    {
        cartesian[idx0] = cartesian[theta_counter];
        normals[idx0]	= normals[theta_counter];
        colors[idx0]	= colors[theta_counter];
    }

    float	cl0, cl1, cl2, cl3;
    point	nm0, nm1, nm2, nm3;
    phi = ant.params.min_phi;
    int quad_counter = 0;
#if NET_ENVIRON
    for (int phi_counter = 0; phi_counter < nphi; phi_counter++)
    {
#else
    for (phi_counter = 0; phi_counter < nphi; phi_counter++)
    {
#endif
        int theta = ant.params.min_theta;
        for (int theta_counter = 0; theta_counter < ntheta; theta_counter++)
        {

            idx0 = phi_counter * ntheta + theta_counter;
            idx1 = idx0 + ntheta;
            if (theta_counter == ntheta-1)
            {
                idx3 = phi_counter * ntheta;
                idx2 = idx3 + ntheta;
            }
            else
            {
                idx2 = idx1 + 1;
                idx3 = idx0 + 1;
            }

            pt0 = cartesian[idx0];
            pt1 = cartesian[idx1];
            pt2 = cartesian[idx2];
            pt3 = cartesian[idx3];

            nm0 = normals[idx0];
            nm1 = normals[idx1];
            nm2 = normals[idx2];
            nm3 = normals[idx3];

            cl0 = colors[idx0];
            cl1 = colors[idx1];
            cl2 = colors[idx2];
            cl3 = colors[idx3];

            // Unfortunately, we can't push names between glBegin and glEnd so the names have to be associated with quads.

            quad_db[quad_counter] = (DB2POWER(db_gain[idx0]) + DB2POWER(db_gain[idx1]) + DB2POWER(db_gain[idx2]) + DB2POWER(db_gain[idx3])) * 0.25f;
            quad_db[quad_counter] = POWER2DB(quad_db[quad_counter]);
            unsigned int key = (id << 16) | quad_counter;
            glPushName(key);
            quad_counter++;

            glBegin(GL_QUADS);

            if (phi < 90 || phi > 270)
            {
                glNormal3f(nm0.x, nm0.z, nm0.y);
                glTexCoord1f(cl0);
                glVertex3f(pt0.x, pt0.z, pt0.y);

                glNormal3f(nm1.x, nm1.z, nm1.y);
                glTexCoord1f(cl1);
                glVertex3f(pt1.x, pt1.z, pt1.y);

                glNormal3f(nm2.x, nm2.z, nm2.y);
                glTexCoord1f(cl2);
                glVertex3f(pt2.x, pt2.z, pt2.y);

                glNormal3f(nm3.x, nm3.z, nm3.y);
                glTexCoord1f(cl3);
                glVertex3f(pt3.x, pt3.z, pt3.y);
            }
            else
            {
                nm0.negate();
                nm1.negate();
                nm2.negate();
                nm3.negate();

                glNormal3f(nm0.x, nm0.z, nm0.y);
                glTexCoord1f(cl0);
                glVertex3f(pt0.x, pt0.z, pt0.y);

                glNormal3f(nm3.x, nm3.z, nm3.y);
                glTexCoord1f(cl3);
                glVertex3f(pt3.x, pt3.z, pt3.y);

                glNormal3f(nm2.x, nm2.z, nm2.y);
                glTexCoord1f(cl2);
                glVertex3f(pt2.x, pt2.z, pt2.y);

                glNormal3f(nm1.x, nm1.z, nm1.y);
                glTexCoord1f(cl1);
                glVertex3f(pt1.x, pt1.z, pt1.y);
            }
            glEnd();
            glPopName();

            theta += theta_inc;
        }
        phi += phi_inc;
    }
    glEndList();

    delete [] colors;
    delete [] normals;
    delete [] cartesian;
}

void 
pattern::draw(void)
{
    assert(dlist->list != 0);
    glCallList(dlist->list);
}

evaluation::evaluation(void) :
    max_db(0.0f),
    min_db(0.0f)
{
}

evaluation::evaluation(const evaluation& other) :
    max_db(other.max_db),
    min_db(other.min_db)
{
    for (unsigned int ii = 0; ii < other.radiation.size(); ii++)
        radiation.push_back(new pattern(*(other.radiation[ii])));
}

evaluation::~evaluation(void)
{
    for (unsigned int ii = 0; ii < radiation.size(); ii++)
		delete radiation[ii];
}

void 
evaluation::print(void) const
{
    DEBUG_TRACE("PATTERN COUNT = %d\n", radiation.size());
    for (unsigned int ii = 0; ii < radiation.size(); ii++)
    {
        DEBUG_TRACE("\tPATTERN %d:\n", ii);
        radiation[ii]->print();
    }
}



float 
evaluation::compare(const evaluation& first, 
					const evaluation& second,
					float weight)
{
    float diff = 0.0f;

    assert(first.radiation.size() == second.radiation.size());
    unsigned int count = first.radiation.size();
	    for (unsigned int ii = 0; ii < count; ii++)
    {
        pattern* p1 = first.radiation[ii];
        pattern* p2 = second.radiation[ii];
        assert((p1->frequency == p2->frequency) && (p1->db_count == p2->db_count));
        unsigned int n = p1->db_count;
        for (unsigned int jj = 0; jj < n; jj++)
        {
            // Square difference.  We don't necessarily want this to be linear.
            // Exponetial 1.0 or higher.
            diff += powf(fabs(p1->db_gain[jj] - p2->db_gain[jj]), weight);
            // exp([1.0, 2.0], fabs) if it's higher gain, assume it's got a diff of zero.
        }
	}
    return diff;
}

antenna::antenna(void) :
    color(1.0f, 0.0f, 0.0f)
{
    label[0] = 0;
    params.min_theta = 0;
    params.step_theta = 46;
    params.incr_theta = 4;
//	params.step_theta = 5;
//	params.incr_theta = 45;
    params.min_phi = 0;
    params.step_phi = 90;
    params.incr_phi = 4;
//	params.step_phi = 8;
//	params.incr_phi = 45;
    params.min_freq = 100;
    params.step_freq = 1;
    params.incr_freq = 10;

    points = &redo_points;

    init_control_params();
	isPlatform = false;
}

antenna::antenna(const antenna& other) :
    results(other.results)
{
    for (unsigned int ii = 0; ii < other.wires.size(); ii++)
    {
        wires.push_back(new wire(*(other.wires[ii])));
    }
#if NET_ENVIRON
    for (unsigned int ii = 0; ii < other.redo_points.size(); ii++)
    {
#else
    for (ii = 0; ii < other.redo_points.size(); ii++)
    {
#endif
        redo_points.push_back(new point(*(other.redo_points[ii])));
    }
#if NET_ENVIRON
    for (unsigned int ii = 0; ii < other.undo_points.size(); ii++)
    {
#else
    for (ii = 0; ii < other.undo_points.size(); ii++)
    {
#endif
        undo_points.push_back(new point(*(other.undo_points[ii])));
    }
    if (other.points == &(other.redo_points))
        points = &redo_points;
    else
        points = &undo_points;

    strcpy(label,other.label);
    color = other.color;
    params = other.params;

    for (unsigned int ii = 0; ii < CONTROL_COUNT; ii++)
    {
        cmds[ii] = other.cmds[ii];
    }
}

antenna::~antenna(void)
{
    /*abaj*/
    /*for (unsigned int ii = 0; ii < CONTROL_COUNT; ii++) {
    	delete cmds[ii];
    }*/

    for (unsigned int ii = 0; ii < wires.size(); ii++)
    {
        delete wires[ii];
    }

    for (unsigned int ii = 0; ii < redo_points.size(); ii++)
    {
        delete redo_points[ii];
    }
    redo_points.clear();

    for (unsigned int ii = 0; ii < undo_points.size(); ii++)
    {
        delete undo_points[ii];
    }
    undo_points.clear();
}

int 
antenna::num_polar(void) const
{
    return (params.step_theta * params.step_phi);
}

void 
antenna::erase_points(void)
{
    for (unsigned int ii = 0; ii < points->size(); ii++)
    {
        delete points->at(ii);
    }
    points->clear();
}

void 
antenna::offset(point& where)
{
    for (unsigned int ii = 0; ii < wires.size(); ii++)
    {
        wires[ii]->a.x += where.x;
        wires[ii]->a.y += where.y;
        wires[ii]->a.z += where.z;
        wires[ii]->b.x += where.x;
        wires[ii]->b.y += where.y;
        wires[ii]->b.z += where.z;
    }
}
void 
antenna::load(void)
{
    char buffer[200];

    FILE* fp = fopen(label, "r");
    if (fp != NULL)
    {

        float	ax, ay, az, bx, by, bz, dia;
        int		seg, m;
        char	keyword[3];
        wire*	w;

        while (fgets(buffer, 200, fp) != NULL)
        {

            if (buffer[0] == 'G' && buffer[1] == 'W')
            {
                sscanf(buffer, "%s %d %d %f %f %f %f %f %f %f", keyword, &m, &seg, &ax, &ay, &az, &bx, &by, &bz, &dia);

                w = new wire;
                w->a.x = ax;
                w->a.y = ay;
                w->a.z = az;
                w->b.x = bx;
                w->b.y = by;
                w->b.z = bz;
                w->segments = seg;
                w->diameter = dia;
                wires.push_back(w);
				/* stores the longest wire in the platform for local neighborhood mutation */
				if ( w->a.distance(w->b) > mesh_resolution && isPlatform == true )
					default_mesh_resolution = mesh_resolution = w->a.distance(w->b); 
            }
			
			if (buffer[0] == 'F' && buffer[1] == 'R') 
				sscanf(buffer, "%s %d %d %d %d %f", keyword, &ax, &params.step_freq, &ay, &az, &params.min_freq); 
		
			if (buffer[0] == 'R' && buffer[1] == 'P')
				sscanf(buffer, "%s %d %d %d %d %d %d %d %d %d", keyword, &ax, &params.step_theta, &params.step_phi, &ay, &params.min_theta, &params.min_phi, &params.incr_theta, &params.incr_phi, &az);
		
	
        };
        fclose(fp);
    }
}

void 
antenna::lines2points(void)
{
    point pt;
    for (unsigned int ii = 0; ii < wires.size(); ii++)
    {
        if (exists(wires[ii]->a) < 0) points->push_back(new point(wires[ii]->a));
        if (exists(wires[ii]->b) < 0) points->push_back(new point(wires[ii]->b));
        for (unsigned int jj = ii+1; jj < wires.size(); jj++)
        {
            // look for intersection point between the two wires
            if (wires[ii]->intersects(*(wires[jj]), pt))
                // check to see if point already exists
                if (exists(pt) < 0) points->push_back(new point(pt));
        }
    }
}

// Find the minimum of the x, y, and z components.
point 
antenna::min_point() const
{
    float x, y, z;
    x = y = z = FLT_MAX;
    for (unsigned i = 0; i < points->size(); i++)
    {
        if (points->at(i)->x < x)
            x = points->at(i)->x;
        if (points->at(i)->y < y)
            y = points->at(i)->y;
        if (points->at(i)->z < z)
            z = points->at(i)->z;
    }
    return point(x,y,z);
}

// Find the maximum of the x, y, and z components.
point 
antenna::max_point() const
{
    float x, y, z;
    x = y = z = FLT_MIN;
    for (unsigned i = 0; i < points->size(); i++)
    {
        if (points->at(i)->x > x)
            x = points->at(i)->x;
        if (points->at(i)->y > y)
            y = points->at(i)->y;
        if (points->at(i)->z > z)
            z = points->at(i)->z;
    }
    return point(x,y,z);
}

int 
antenna::exists(const point& pt) const
{
    for (unsigned int ii = 0; ii < points->size(); ii++)
    {
        if (*(points->at(ii)) == pt) return ii;
    }
    return -1;
}

void 
antenna::translate2deck(const point& offset) const
{
    // insert comment card
    /* abaj */

    //sprintf(g_nec_deck[g_nec_line++], "CM %s", label);

    for (unsigned int ii = 0; ii < wires.size(); ii++)
    {
        sprintf(g_nec_deck[g_nec_line++], "GW %3d%5d%10f%10f%10f%10f%10f%10f%10f",
                g_nec_segment++,
                wires[ii]->segments,
                wires[ii]->a.x + offset.x,
                wires[ii]->a.y + offset.y,
                wires[ii]->a.z + offset.z,
                wires[ii]->b.x + offset.x,
                wires[ii]->b.y + offset.y,
                wires[ii]->b.z + offset.z,
                wires[ii]->diameter);
    }
}

void 
antenna::analyze(bool verbose)
{
    int antenna_first_segment;
	point offset;
	
	clear_nec_deck();
    platform->translate2deck(offset);
	antenna_first_segment = g_nec_segment;
    for (unsigned int ii = 0; ii < results.radiation.size(); ii++)
    {
        delete results.radiation[ii];
    }
	translate2deck(*(points->at(0)));
    results.radiation.clear();
    append_analysis(antenna_first_segment);		// excite first segment
}

/*abaj*/
void 
antenna::calculate_centroid(void)
{
	centroid.set(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < points->size(); i++) 
	{
		centroid.x += points->at(i)->x;
		centroid.y += points->at(i)->y;
		centroid.z += points->at(i)->z;
	}
		
	centroid.x /= points->size();
	centroid.y /= points->size();
	centroid.z /= points->size();
}

/*abaj*/
void 
antenna::calculate_dist_from_centroid(void)
{
	for (int i = 0; i < points->size(); i++) 
		points->at(i)->dist_from_centroid = points->at(i)->distance(centroid);
}

void 
antenna::init_control_params(void)
{
    cmds[GS] = new cmdGS;
    cmds[GS]->valid = true;
    cmds[GE] = new cmdGE;
    cmds[GE]->valid = true;
    cmds[GN] = new cmdGN;
    cmds[GN]->valid = true;
    cmds[CP] = new cmdCP;
    cmds[GD] = new cmdGD;
    cmds[NT] = new cmdNT;
    cmds[LD] = new cmdLD;
    cmds[LE] = new cmdLE;
    cmds[NE] = new cmdNE;
    cmds[TL] = new cmdTL;
}

void 
antenna::append_analysis(int excitation) const
{
    /*abaj

    for (unsigned int ii = 0; ii < CONTROL_COUNT; ii++) {
    	if (platform->cmds[ii]->valid)
    		platform->cmds[ii]->write(g_nec_deck[g_nec_line++]);
    }
    */

    /* Added from older version of APOT abaj */
    sprintf(g_nec_deck[g_nec_line++], "GS   0    0         1");		// scale dimensions by one
    sprintf(g_nec_deck[g_nec_line++], "GE   0   -1         0");		// end of geometry - no ground plane
    sprintf(g_nec_deck[g_nec_line++], "GN  -1");			// free space conditions

    // specify frequency
    sprintf(g_nec_deck[g_nec_line++], "FR   0%5d    0    0%10.5f%10.5f", params.step_freq, (float) params.min_freq, (float) params.incr_freq);

    sprintf(g_nec_deck[g_nec_line++], "EX   0%5d    1    0%10.5f%10.5f", excitation, 1.0f, 0.0f);	// excitation

    // specify radiation pattern
    sprintf(g_nec_deck[g_nec_line++], "RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f",
            params.step_theta,
            params.step_phi,
            (float) params.min_theta,
            (float) params.min_phi,
            (float) params.incr_theta,
            (float) params.incr_phi,
            0.0f);

    sprintf(g_nec_deck[g_nec_line++], "EN");
}


void 
antenna::append_coupling_cards(vector<int> *tags, unsigned int antenna_id) const
{
	if (antenna_id == 0)
		sprintf(g_nec_deck[g_nec_line++], "GE   0");
    
	// specify frequency
    sprintf(g_nec_deck[g_nec_line++], "FR   0%5d    0    0%10.5f%10.5f", params.step_freq, (float) params.min_freq, (float) params.incr_freq);
	
	char string[100];
	sprintf(string, "CP  %d  1", tags->at(antenna_id));
	for (unsigned int i = 0; i < tags->size(); i++) 
	{	
		if (i != antenna_id)
		{
			if (strlen(string) == 0) 
				sprintf(string, "CP  %d  1", tags->at(i)); 
			else
			{
				sprintf(g_nec_deck[g_nec_line++], "%s  %d  1", string, tags->at(i));
				string[0] = 0;
			}
		}
	}

	if (strlen(string) != 0)
		sprintf(g_nec_deck[g_nec_line++],  "%s", string);

	sprintf(g_nec_deck[g_nec_line++], "EX   0%5d    1    0%10.5f%10.5f", tags->at(antenna_id), 1.0f, 0.0f);	// excitation

    // specify radiation pattern
    sprintf(g_nec_deck[g_nec_line++], "RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f",
            params.step_theta,
            params.step_phi,
            (float) params.min_theta,
            (float) params.min_phi,
            (float) params.incr_theta,
            (float) params.incr_phi,
            0.0f);
	
	for( unsigned int i = 0; i < tags->size(); i++ )
	{
		if (i != antenna_id)
		{
			sprintf(g_nec_deck[g_nec_line++], "EX   0%5d    1    0%10.5f%10.5f", tags->at(i), 1.0f, 0.0f);	// excitation
			sprintf(g_nec_deck[g_nec_line++], "XQ");
		}
	}

	/* put the EN card at the end */
	if (antenna_id == tags->size() - 1)
		sprintf(g_nec_deck[g_nec_line++], "EN");
}

bool 
antennas_valid(void)
{
    if (platform == NULL || antennas.size() == 0) return false;
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {
        if (antennas[ii]->points->size() == 0) return false;
    }
    return true;
}

void 
clear_nec_deck(void)
{
    // Because of how NEC4 parses the deck lines, we need to ensure that there are no extraneous characters.
    for (int ii = 0; ii < MAX_NEC_DECK_LINES; ii++)
        memset(g_nec_deck[ii],' ', MAX_NEC_DECK_LINE_LENGTH);
    g_nec_line = 0;
    g_nec_segment = 1;
}

void 
print_nec_deck(const char* filename)
{
    char buffer[100];
    FILE* fp = fopen(filename, "w");
    if (fp != NULL)
    {
        for (int ii = 0; ii < g_nec_line; ii++)
        {
            fprintf(fp, "%s\n", g_nec_deck[ii]);
        }
        fclose(fp);
        // convert carriage returns
        sprintf(buffer, "C:\\cygwin\\bin\\dos2unix %s", filename);
        execute(buffer);
    }
}

float
read_coupling_results(const char* results_file, int size)
{
	char *buffer = new char[301];
	int couplings_read = 0;
	float	coupling = 0.0f, resultant_coupling = 0.0f;
    if (results_file != NULL)
    {
        FILE* fp = fopen(results_file, "r");
        if (fp != NULL)
        {
			while (couplings_read < size)
			{
				while ((fgets(buffer, 300, fp) != NULL) && strncmp(buffer, " ------- COUPLING BETWEEN", 25));	// find the start of coupling data
				fgets(buffer, 300, fp);
				fgets(buffer, 300, fp);
				int read = 0;
				while( read < size - 1)
				{
					fgets(buffer, 300, fp);
					sscanf(buffer, "%*d %*d %*d %*d %*d %*d %f", &coupling);
					
					/* coupling to be bounded by 100 & -100 - Jason*/
					if (coupling >= 100)
						resultant_coupling += 100 * 2;
					else if ( !(coupling <= -100) )
						resultant_coupling += 100 + coupling;
					read++;
				}
				couplings_read++;
			}
		}
        fclose(fp);
     }
	delete[] buffer;
    return resultant_coupling;
}

unsigned int 
read_nec_results(const char* results_file,
				evaluation& results,
				antenna& ant)
{
    unsigned int read = 0;
    char *buffer = new char[301];
    if (results_file != NULL)
    {
        FILE* fp = fopen(results_file, "r");
        if (fp != NULL)
        {
            float	theta, phi, vertdb, horizdb, totaldb;
            unsigned int jj = 0;
            do
            {
                unsigned int count = 0;
                pattern *rad = new pattern(ant.num_polar(), ant.params.min_freq + jj *  ant.params.incr_freq);
                while ((fgets(buffer, 300, fp) != NULL) && strncmp(buffer, " DEGREES", 8));	// find the start of the radiation data
                while (fgets(buffer, 300, fp) != NULL)
                {
                    if (sscanf(buffer, "%f %f %f %f %f", &theta, &phi, &vertdb, &horizdb, &totaldb) != 5) break;
                    rad->db_gain[count++] = totaldb;
                    if (totaldb > results.max_db) results.max_db = totaldb;
                    if (totaldb < results.min_db) results.min_db = totaldb;
                }
                if (count > 0)
                {
                    assert(ant.num_polar() == count);
                    rad->db_count = count;
                    results.radiation.push_back(rad);
                    read += count;
                    jj++;
                }
            }
            while (!feof(fp));
            fclose(fp);
        }
    }
	delete[] buffer;
    return read;
}

void execute(const char* command)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
                        (char*) command,        // Command line
                        NULL,           // Process handle not inheritable
                        NULL,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        CREATE_NO_WINDOW,
                        NULL,           // Use parent's environment block
                        NULL,           // Use parent's starting directory
                        &si,            // Pointer to STARTUPINFO structure
                        &pi )           // Pointer to PROCESS_INFORMATION structure
      )
    {
        printf( "CreateProcess failed (%d)\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
