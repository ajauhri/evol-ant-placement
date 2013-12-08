#include<iostream>
#include<exception>
#include<random>
#include<math.h>
#include<time.h>
#include<algorithm>
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
    catch (...)
    { 
        throw; 
    }
}


/**
 * @desc Cleaning of old runs from FS
 *       
 */
void algorithm::setup_run_context()
{
    try 
    {
        boost::filesystem::remove_all(eap::run_directory);
        boost::filesystem::create_directory(eap::run_directory);

        boost::filesystem::remove_all(eap::freespace_directory);
        boost::filesystem::create_directory(eap::freespace_directory);
    }
    catch (...)
    {
        throw;
    }
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
    catch (...)
    {
        throw;
    }
}

/**
 * @desc Load all wires in the nec file into the algorithm
 */
void algorithm::load_nec_files()
{
    try 
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
    catch (...)
    {
        throw;
    }
}

void algorithm::create_nec_strs()
{
    try
    {

        int plat_wire_id = 1;
        boost::format formatter(WIRE_NEC);
        for (wire_ptr w : this->platform->wires)
        {
            this->platform->nec_wires.push_back(str(formatter % plat_wire_id++ % w->segments % w->a->x % w->a->y % w->a->z % w->b->x % w->b->y % w->b->z % w->diameter));
        }
    }
    catch (...)
    {
        throw;
    }
}

std::vector<wire_ptr> algorithm::load_wires(const std::string& nec_file, const std::string& err_msg)
{
    std::ifstream infile(eap::input_directory + nec_file);
    std::vector<wire_ptr> wires;
    std::string line;
    float ax, ay, az, bx, by, bz, dia;
    int seg, m;
    char keyword[3];
    try 
    {
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
    }
    catch(...)
    { 
        infile.close();
        throw; 
    }
    return wires;
}

/**
 * @desc Setup all free space nec files using the platform file and antenna file
 */
void algorithm::write_freespace()
{
    std::ofstream outfile;
    try 
    {
        boost::format formatter(eap::freespace_directory + "ant%03d.nec");
        int ant_id = 0;

        for (ant_config_ptr ant : this->ant_configs)
        {
            std::string buffer = str(formatter % ant_id++);
            outfile.open(buffer);
            write_platform(outfile);
            write_ant(outfile, ant, 0, this->platform->nec_wires.size() + 1); //put at the first position, doesn't matter for free space

            //need to cout here and check if theser are deallocated at the end of each generation
            int excitation_id = this->platform->nec_wires.size() + 1;
            write_excitation(outfile, excitation_id);

            outfile.close();
        }
        std::cout<<"***completed writing free space nec files\n";
    }
    catch (...)
    {
        outfile.close();
        throw;
    }
}

void algorithm::write_platform(std::ofstream& outfile)
{
    try 
    {
        if (!outfile) throw eap::InvalidStateException("Write file not defiend for putting platform wires");
        for (unsigned int i = 0; i<this->platform->nec_wires.size(); i++)
            outfile << platform->nec_wires[i];
    }
    catch (...)
    {
        throw;
    }
}

void algorithm::write_ant(std::ofstream& outfile, ant_config_ptr &ant, unsigned int position, unsigned int wire_ind)
{
    try
    {
        if (!outfile || !ant || position < 0 || wire_ind < 0)
            throw eap::InvalidStateException("write_ant called incorrectly");

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
    catch (...)
    {
        throw;
    }
}

void algorithm::write_excitation(std::ofstream& outfile, unsigned int id)
{
    try 
    {
        if (!outfile || id < 0) throw eap::InvalidStateException("write_excitation called incorrectly");
        //the formatting is just BS
        outfile << "GS   0    0         1\n";
        outfile << "GE   0   -1         0\n";
        outfile << "GN  -1\n";
        outfile << str(boost::format("FR   0%5d    0    0%10.5f%10.5f\n") % step_freq % min_freq % incr_freq);
        outfile << str(boost::format("EX   0%5d    1    0%10.5f%10.5f\n") % id % 1.0f % 0.0f);
        outfile << str(boost::format("RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f\n") % step_theta % step_phi % min_theta % min_phi % incr_theta % incr_phi % 0.0f);
        outfile << "EN";
    }
    catch (...)
    {
        throw;
    } 
}


void algorithm::run_freespace()
{
    try 
    {
        boost::format formatter("./nec2++.exe -i " + eap::freespace_directory + "ant%03d.nec");
        for (unsigned int i=0; i<ant_configs.size(); i++)
            system(str(formatter % i).c_str());
        std::cout<<"***completed creating out files for free space patterns\n"; 
    }
    catch(...)
    { 
        throw;
    }
}

void algorithm::read_freespace()
{
    try
    {
        boost::format formatter(eap::freespace_directory + "ant%03d.out");
        for (unsigned int i=0; i<ant_configs.size(); i++)
        {
            individual_ptr ind(new individual);
            evaluation_ptr eval(new evaluation);
            ind->evals.push_back(eval);
            read_nou(str(formatter % i), ind->evals.back());
            free_inds.push_back(ind);
        }
    }
    catch (...)
    {
        throw;
    }
}

unsigned int algorithm::read_nou(const std::string results_file,
        const evaluation_ptr &eval)
{
    std::ifstream infile;
    try
    {
        infile.open(results_file);
        std::string line;
        unsigned int read = 0, j = 0;
        float theta, phi, vertdb, horizdb, totaldb;

        if (!infile) throw eap::InvalidStateException(results_file + " not found");
        do
        {
            pattern_ptr pat(new pattern);
            pat->frequency = min_freq + j*incr_freq;
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
                j++;
            }
        }
        while (std::getline(infile, line)); 
        infile.close();
        return read;
    }
    catch (const eap::InvalidStateException &e)
    {
        infile.close();
        throw;
    }
}

float algorithm::compare(const evaluation_ptr &first,
        const evaluation_ptr &second)
{
    try 
    {
        float diff = 0.0f;
        if (first->radiation.size() != second->radiation.size())
            throw eap::InvalidStateException("evaluation::radiation vector sizes don't match");
        for (unsigned int i=0; i<first->radiation.size(); ++i)
        {
            pattern_ptr p1 = first->radiation[i];
            pattern_ptr p2 = second->radiation[i];
            //why do we need to check for matching frequency?
            if (p1->frequency != p2->frequency) throw eap::InvalidStateException("frequencies don't match");
            for (unsigned int j=0; j<num_polar(); ++j)
            {
                diff += powf(fabs(p1->db_gain[j] - p2->db_gain[j]), exp_weight);
            }
        }
        return diff;
    }
    catch (...)
    {
        throw;
    }
}


/**
 * @desc Exchange of genotypes between two individuals
 * @param ind1, ind2 - two individuals
 * @return Vector of genetically bred individuals
 */
std::vector<individual_ptr> algorithm::breed(const individual_ptr &ind1, const individual_ptr &ind2)
{
    try
    {
    std::vector<individual_ptr> children;
    individual_ptr child1 (new individual);
    individual_ptr child2 (new individual);
    children.push_back(child1);
    children.push_back(child2);

    unsigned int xover = eap::rand(0, ant_configs.size()-1);
    for (unsigned i = 0; i < xover; i++)
    {	
        children[0]->positions.push_back(ind1->positions[i]);
        children[1]->positions.push_back(ind2->positions[i]);
    }

    for (unsigned i = xover; i < ant_configs.size(); i++)
    {
        children[0]->positions.push_back(ind2->positions[i]);
        children[1]->positions.push_back(ind1->positions[i]);
    }
    return children;
    }
    catch (...)
    {
         throw;
    }
}

/**
 * @desc Simple mutation of individual based on the mutation probability
 * @param ind The individual to be mutated
 */
void algorithm::simple_mutation(individual_ptr &ind)
{
    for(unsigned i=0; i<ant_configs.size(); i++)
    {
        if(eap::rand01() < mutation)
        {	
            int pos = eap::rand(0, ant_configs[i]->positions.size()-1);
            ind->positions.insert(ind->positions.begin(), ant_configs[i]->positions.at(pos));
        }
    }
}



inline unsigned int algorithm::num_polar(void)
{
    return step_theta * step_phi;
}

#if 0

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
