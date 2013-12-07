#include<iostream>
#include<exception>
#include<random>
#include<math.h>
#include<time.h>
#include<boost/filesystem.hpp>
#include<boost/format.hpp>
#include<sstream>
#include<string>
#include<stdlib.h>
#include<algorithm.hpp>
#include<eap_resources.hpp>
#include<position.hpp>
#include<lua_cmds.hpp>
#include<wire.hpp>

namespace 
{
    char const *algorithm_s = "algorithm";
    char const *mutation_s = "mutation";
    char const *exp_weight_s = "exp_weight";
    char const *auto_seed_s  = "auto_seed";
    char const *seed_s = "seed";

    const std::string WIRE_NEC = "GW %3d%5d%10f%10f%10f%10f%10f%10f%10f\n";
}

/**
 * @param lua_filename 
 */
algorithm::algorithm(std::string lua_file)
{
    this->lua_file = lua_file;
}

// extern declard in eap_resources.hpp
std::mt19937 eap::gen;

void algorithm::setup_algo_params()
{
    try
    {
        this->mutation = eap::get_fvalue(mutation_s);
        this->exp_weight = eap::get_fvalue(exp_weight_s);
        this->auto_seed = eap::get_fvalue(auto_seed_s);
        if (this->auto_seed != 0.0f) 
            eap::gen.seed(time(NULL) + getpid()); //getpid() - Binaries executed one after the other have PRNGs initialized differently
        else
            eap::gen.seed(eap::get_fvalue(seed_s));

        std::cout<<"***completed generic algo parameter setup\n";
    }
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
    }
}


/**
 * @desc Cleaning of old runs from FS
 *       
 */
void algorithm::setup_run_context()
{
    boost::filesystem::remove_all(eap::run_directory);
    boost::filesystem::create_directory(eap::run_directory);

    //boost::filesystem::remove_all(eap::freespace_directory);
    //boost::filesystem::create_directory(eap::freespace_directory);
}

/**
 * @desc Load all antenna placements 
 */
void algorithm::setup_ant_placements()
{
    try
    {
        eap::load_lua_lib(this->lua_file.c_str());
        std::cout<<"***completed loading antenna placements\n";
    }
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
        exit(0);
    }
}

/**
 * @desc Load all wires in the nec file into the algorithm
 */
void algorithm::load_nec_files()
{
    // first load platform wires
    this->platform->wires = load_wires(this->platform->nec_file, "platform file corrupted");

    // then load all antenna wires
    for (ant_config_ptr ant : this->ant_configs)
    {
        ant->wires = load_wires(ant->nec_file, ant->nec_file + "file corrupted");
    }

    //since the nec files for GW card require a special format and have to be written multiple times for the platform, might as well do it once and keep in memory
    create_nec_strs();
}

void algorithm::create_nec_strs()
{
    int plat_wire_id = 1;
    boost::format formatter(WIRE_NEC);
    for (wire_ptr w : this->platform->wires)
    {
        this->platform->nec_wires.push_back(str(formatter % plat_wire_id++ % w->segments % w->a->x % w->a->y % w->a->z % w->b->x % w->b->y % w->b->z % w->diameter));
    }
}

std::vector<wire_ptr> algorithm::load_wires(const std::string& nec_file, const std::string& err_msg)
{
    try 
    {
        std::vector<wire_ptr> wires;
        std::ifstream infile(eap::input_directory + nec_file);
        std::string line;
        float ax, ay, az, bx, by, bz, dia;
        int seg, m;
        char keyword[3];

        if (!infile) throw eap::InvalidStateException(nec_file + " not found");

        while (std::getline(infile, line))
        {
            if (line[0] == 'G' && line[1] == 'W')
            {
                std::istringstream iss(line);
                if (!(iss >> keyword >> m >> seg >> ax >> ay >> az >> bx >> by >> bz >> dia)) 
                {
                    throw eap::ParseException(err_msg);
                }
                wire_ptr w(new wire); 
                position_ptr a(new position);
                position_ptr b(new position);
                a->x = ax; a->y = ay; a->z = az;
                b->x = bx; b->y = by; b->z = bz;
                w->a = a;
                w->b = b;
                w->segments = seg;
                w->diameter = dia;
                wires.push_back(w);
            }
        } 
        std::cout<<"***completed loading wires for "<<nec_file<<"\n";
        infile.close();
        return wires;
    }
    catch(const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
        exit(0);
    }
}

/**
 * @desc Setup all free space nec files using the platform file and antenna file
 */
void algorithm::write_freespace()
{

    int ant_id = 0;
    for (ant_config_ptr ant : this->ant_configs)
    {
        char buffer[100];
        // concatenate using sprintf (http://stackoverflow.com/questions/2674312/how-to-append-strings-using-sprintf) 
        sprintf(buffer, "%s", eap::freespace_directory.c_str());
        sprintf(buffer+strlen(buffer), "ant%03d.nec", ant_id++);

        boost::filesystem::remove(buffer);

        std::ofstream outfile(buffer);
        write_platform(outfile);
        write_ant(outfile, ant, 0, this->platform->nec_wires.size() + 1); //put at the first position, doesn't matter for free space
        
        //need to cout here and check if theser are deallocated at the end of each generation
        int excitation_id = this->platform->nec_wires.size() + 1;

        //the formatting is just BS
        outfile << "GS   0    0         1\n";
        outfile << "GE   0   -1         0\n";
        outfile << "GN  -1\n";
        outfile << str(boost::format("FR   0%5d    0    0%10.5f%10.5f\n") % step_freq % min_freq % incr_freq);
        outfile << str(boost::format("EX   0%5d    1    0%10.5f%10.5f\n") % excitation_id % 1.0f % 0.0f);
        outfile << str(boost::format("RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f\n") % step_theta % step_phi % min_theta % min_phi % incr_theta % incr_phi % 0.0f);
        outfile << "EN";
        outfile.close();
    }
    std::cout<<"***completed writing free space nec files\n";
}

void algorithm::write_platform(std::ofstream& outfile)
{
    for (unsigned int i = 0; i<this->platform->nec_wires.size(); i++)
        outfile << platform->nec_wires[i];
}

void algorithm::write_ant(std::ofstream& outfile, ant_config_ptr &ant, unsigned int position, unsigned int wire_ind)
{
    boost::format formatter(WIRE_NEC);
    for (wire_ptr w : ant->wires)
    {
        outfile << str(formatter % wire_ind++ % w->segments 
                % (w->a->x + ant->positions[position]->x)
                % (w->a->y + ant->positions[position]->y)
                % (w->a->z + ant->positions[position]->z)
                % (w->b->x + ant->positions[position]->x)
                % (w->b->y + ant->positions[position]->y)
                % (w->b->z + ant->positions[position]->z)
                % w->diameter);
    }
}


void algorithm::run_freespace()
{
    boost::format formatter("./nec2++.exe -i " + eap::freespace_directory + "ant%03d.nec");
    for (unsigned int i=0; i<ant_configs.size(); i++)
    {
        std::string f = str(formatter % i);
        std::cout<<f<<"\n";
        system(f.c_str());
    }
    std::cout<<"***completed creating out files for free space patterns\n"; 
}

void algorithm::read_freespace()
{
    boost::format formatter(eap::freespace_directory + "ant%03d.out");
    for (unsigned int i=0; i<ant_configs.size(); i++)
    {
        individual_ptr ind(new individual);
        evaluation_ptr eval(new evaluation);
        ind->eval = eval;
        read_nou(str(formatter % i), ind->eval);
    }
}

unsigned int algorithm::read_nou(const std::string results_file,
        const evaluation_ptr &eval)
{

    std::cout<<results_file<<"\n";
    try
    {
        std::ifstream infile(results_file);
        std::string line;
        unsigned int read = 0;
        float theta, phi, vertdb, horizdb, totaldb;

        if (!infile) throw eap::InvalidStateException(results_file + " not found");
        do
        {
            pattern_ptr pat(new pattern);
            while (std::getline(infile, line) && strncmp(line.c_str(), " DEGREES", 8));

            while (std::getline(infile, line))
            {
                std::istringstream iss(line);
                if (!(iss >> theta >> phi >> vertdb >> horizdb >> totaldb)) break;

                pat->db_gain.push_back(totaldb);
                if (totaldb > eval->max_db)
                    eval->max_db = totaldb;
                if (totaldb < eval->min_db) 
                    eval->min_db = totaldb;
            }
            if (pat->db_gain.size() > 0)
            {
                if (this->num_polar() != pat->db_gain.size()) throw eap::InvalidStateException("Problem with reading nec results for " + results_file);
                eval->radiation.push_back(pat);
                read += pat->db_gain.size();
            }
        }
        while (std::getline(infile, line)); 
        infile.close();
        return read;
    }
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
        exit(0);
    }
}

inline unsigned int algorithm::num_polar(void)
{
    return step_theta * step_phi;
}

#if 0
/**
 * @desc Initializes an individual
 * @return Returns a pointer to the initialized individual
 */
individual_ptr algorithm::setup_individual()
{
    individual_ptr ind (new individual);
    for (unsigned i_ant=0; i_ant<ant_configs.size(); i_ant++)
    {
        /* load all ant_configs into an individual */
        ant_config_ptr ant_config (clone(ant_configs.at(i_ant)));
        int rand = rand_integer(0, ant_configs.at(i_ant)->positions.size());
        while (overlaps_ant(ind, ant_configs.at(i_ant)->positions[rand]) || overlaps_ancillary(ant_configs.at(i_ant)->positions[rand]))
            rand = rand_integer(0, ant_configs.at(i_ant)->positions.size());
        ant_config->positions.push_back(ant_configs.at(i_ant)->positions.at(rand));
        ind->ant_configs.push_back(ant_config);	
    }
    //print_individual(ind);
    return ind;
}

/**
 * @desc Checks if antenna postion overlaps within an individual
 * @return true or false
 */ 
bool algorithm::overlaps_ant(individual_ptr ind, position_ptr p)
{
    for (unsigned p_i=0; p_i<ind->ant_configs.size(); p_i++)
    {
        if (!std::strcmp(p->mount_object.c_str(), ind->ant_configs[p_i]->positions[0]->mount_object.c_str()) &&
                !std::strcmp(p->mount_object_locator.c_str(), ind->ant_configs[p_i]->positions[0]->mount_object_locator.c_str())) {
            return true;
        }
    }
    return false;
}

/**
 * @desc Run the simulation & poll it completes
 */
void algorithm::run_simulation()
{
    //TODO this should run all files present under a certain location

}

/**
 * @desc Clones an antenna configuration. The clone is shallow as positions are not copied.
 * @param config The config to be cloned
 */ 
ant_config_ptr algorithm::clone(ant_config_ptr config)
{
    ant_config_ptr clone_config (new ant_config);
    clone_config->name = std::string(config->name);
    clone_config->antenna_locator = std::string(config->antenna_locator);
    return clone_config;
}

/**
 * @desc Exchange of genotypes between two individuals
 * @param ind_1 First individual
 * @param ind_2 Second indivdual
 * @return Vector of genetically bred individuals
 */
std::vector<individual_ptr> algorithm::breed(individual_ptr ind_1, individual_ptr ind_2)
{
    std::vector<individual_ptr> children;
    individual_ptr child_1 (new individual);
    individual_ptr child_2 (new individual);
    children.push_back(child_1);
    children.push_back(child_2);

    unsigned xover = rand_integer(0, ant_configs.size());
    for (unsigned i = 0; i < xover; i++)
    {	
        children[0]->ant_configs.push_back(clone(ind_1->ant_configs[i]));
        children[1]->ant_configs.push_back(clone(ind_2->ant_configs[i]));
        children[0]->ant_configs[i]->positions.push_back(ind_1->ant_configs[i]->positions.back());
        children[1]->ant_configs[i]->positions.push_back(ind_2->ant_configs[i]->positions.back());
    }

    for (unsigned i = xover; i < ant_configs.size(); i++)
    {

        children[0]->ant_configs.push_back(clone(ind_2->ant_configs[i]));
        children[1]->ant_configs.push_back(clone(ind_1->ant_configs[i]));
        children[0]->ant_configs[i]->positions.push_back(ind_2->ant_configs[i]->positions.back());
        children[1]->ant_configs[i]->positions.push_back(ind_1->ant_configs[i]->positions.back());
    }

    return children;
}

/**
 * @desc Simple mutation of individual based on the mutation probability
 * @param ind The individual to be mutated
 */
void algorithm::simple_mutation(individual_ptr ind)
{
    for(unsigned i=0; i<ant_configs.size(); i++)
    {
        if(aapot_resources::randf(0, 1.0f) < mutation)
        {	
            int rand = rand_integer(0, ant_configs[i]->positions.size());
            ind->ant_configs[i]->positions.insert(ind->ant_configs[i]->positions.begin(), ant_configs[i]->positions.at(rand));
        }
    }
}

/**
 * @desc Generates a random integer between [min, max)
 * @param min Lower limit
 * @param max Upper bound
 */
int algorithm::rand_integer(int min_value, int max_value)
{
    return (int) rand() % max_value;
}

/**
 * @desc Computes total gain 
 * @param gain_theta in dBi
 * @param gain_phi in dBi
 * @return total gain in dBi
 */
float algorithm::cal_totaldb(float gain_theta, float gain_phi)
{
    float tot_linear_gain = pow(10, gain_theta/10) + pow(10, gain_phi/10);
    return 10 * log10(tot_linear_gain);
}
#endif

algorithm::~algorithm(void)
{
    ant_configs.clear();
    ant_configs.shrink_to_fit();
}
