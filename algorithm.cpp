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
    const std::string c_mutation = "mutation";
    const std::string c_exp_weight = "exp_weight";
    const std::string c_auto_seed  = "auto_seed";
    const std::string c_seed = "seed";

    const std::string WIRE_NEC = "GW %3d%5d%10f%10f%10f%10f%10f%10f%10f\n";
}


/**
 * @param lua_filename 
 */
algorithm::algorithm(std::string lua_file)
{
    m_lua_file = lua_file;
}

// extern declard in eap_resources.hpp
std::mt19937 eap::gen;

void algorithm::setup_algo_params()
{
    try
    {
        m_mutation = eap::get_fvalue(c_mutation);
        m_exp_weight = eap::get_fvalue(c_exp_weight);
        m_auto_seed = eap::get_fvalue(c_auto_seed);
        if (m_auto_seed != 0.0f) 
            eap::gen.seed(time(NULL) + getpid()); //getpid() - Binaries executed one after the other have PRNGs initialized differently
        else
            eap::gen.seed(eap::get_fvalue(c_seed));
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
        eap::load_lua_lib(m_lua_file.c_str());
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
        m_platform->m_wires = load_wires(m_platform->m_nec_file, "platform file corrupted");

        // then load all antenna wires
        for (ant_config_ptr ant : m_ant_configs)
        {
            ant->m_wires = load_wires(ant->m_nec_file, ant->m_nec_file + "file corrupted");
        }

        //since the nec files for GW card require a special format and have to be written multiple times for the platform, might as well do it once and keep in memory
        create_nec_strs();
        std::cout<<"***completed loading nec files\n";
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
        for (wire_ptr w : m_platform->m_wires)
        {
            m_platform->m_nec_wires.push_back(str(formatter % plat_wire_id++ % w->m_segments % w->mp_a->m_x % w->mp_a->m_y % w->mp_a->m_z % w->mp_b->m_x % w->mp_b->m_y % w->mp_b->m_z % w->m_diameter));
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
                a->m_x = ax; a->m_y = ay; a->m_z = az;
                b->m_x = bx; b->m_y = by; b->m_z = bz;
                w->mp_a = a;
                w->mp_b = b;
                w->m_segments = seg;
                w->m_diameter = dia;
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

        for (ant_config_ptr ant : m_ant_configs)
        {
            std::string buffer = str(formatter % ant_id++);
            outfile.open(buffer);
            write_platform(outfile);
            write_ant(outfile, ant, ant->m_positions[0], m_platform->m_nec_wires.size() + 1); //put at the first position, doesn't matter for free space

            //need to cout here and check if theser are deallocated at the end of each generation
            int excitation_id = m_platform->m_nec_wires.size() + 1;
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
        for (unsigned int i_wire = 0; i_wire<m_platform->m_nec_wires.size(); ++i_wire)
            outfile << m_platform->m_nec_wires[i_wire];
    }
    catch (...)
    {
        throw;
    }
}

void algorithm::write_ant(std::ofstream& outfile, ant_config_ptr &ant, position_ptr &pos, unsigned int wire_ind)
{
    try
    {
        if (!outfile || !ant || !pos || wire_ind < 0)
            throw eap::InvalidStateException("write_ant called incorrectly");

        boost::format formatter(WIRE_NEC);
        for (wire_ptr pi_w : ant->m_wires)
        {
            outfile << str(formatter % wire_ind++ % pi_w->m_segments 
                    % (pi_w->mp_a->m_x + pos->m_x)
                    % (pi_w->mp_a->m_y + pos->m_y)
                    % (pi_w->mp_a->m_z + pos->m_z)
                    % (pi_w->mp_b->m_x + pos->m_x)
                    % (pi_w->mp_b->m_y + pos->m_y)
                    % (pi_w->mp_b->m_z + pos->m_z)
                    % pi_w->m_diameter);
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
        outfile << str(boost::format("FR   0%5d    0    0%10.5f%10.5f\n") % m_step_freq % m_min_freq % m_incr_freq);
        outfile << str(boost::format("EX   0%5d    1    0%10.5f%10.5f\n") % id % 1.0f % 0.0f);
        outfile << str(boost::format("RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f\n") % m_step_theta % m_step_phi % m_min_theta % m_min_phi % m_incr_theta % m_incr_phi % 0.0f);
        outfile << "EN";
    }
    catch (...)
    {
        throw;
    } 
}

void algorithm::write_coupling(std::ofstream& outfile, std::vector<int> &start_ids)
{
    try
    {
        if (!outfile || start_ids.size() == 0) throw eap::InvalidStateException("write_coupling called incorrectly");
        // the formatting is again BS
        outfile << "GE   0\n";

        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
        {
            outfile << str(boost::format("FR   0%5d    0    0%10.5f%10.5f\n") % m_step_freq % m_min_freq % m_incr_freq);
            std::string line(str(boost::format("CP  %d  1") % start_ids[i_ant])); 
            for (unsigned int i_start=0; i_start<start_ids.size(); ++i_start)
            {
                if (i_start != i_ant)
                {
                    if (line.length() == 0)
                        line = str(boost::format("CP %d 1") % start_ids[i_start]);
                    else
                    {
                        outfile << str(boost::format("%s  %d  1\n") % line % start_ids[i_start]);
                        line = "";
                    }
                }
            }
            if (line.length() != 0)
                outfile << str(boost::format("%s\n") % line);

            outfile << str(boost::format("EX   0%5d    1    0%10.5f%10.5f\n") % start_ids[i_ant] % 1.0f % 0.0f);
            outfile << str(boost::format("RP   0%5d%5d 1000%10.5f%10.5f%10.5f%10.5f%10.5f\n") % m_step_theta % m_step_phi % m_min_theta % m_min_phi % m_incr_theta % m_incr_phi % 0.0f);

            for(unsigned int i_start=0; i_start<start_ids.size(); ++i_start)
            {
                if (i_start != i_ant)
                {
                    outfile << str(boost::format("EX   0%5d    1    0%10.5f%10.5f\n") % start_ids[i_start] % 1.0f % 0.0f); 
                    outfile << "XQ\n";
                }
            }
        }
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
        for (unsigned int i=0; i<m_ant_configs.size(); i++)
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
        for (unsigned int i=0; i<m_ant_configs.size(); i++)
        {
            individual_ptr p_ind(new individual);
            evaluation_ptr p_eval(new evaluation);
            p_ind->m_evals.push_back(p_eval);
            read_radiation(str(formatter % i), p_eval);
            m_free_inds.push_back(p_ind);
        }
    }
    catch (...)
    {
        throw;
    }
}

unsigned int algorithm::read_radiation(const std::string results_file,
        const evaluation_ptr &p_eval)
{
    std::ifstream infile;
    try
    {
        infile.open(results_file);
        std::string line;
        unsigned int read = 0, j = 0;
        float theta, phi, vertdb, horizdb, totaldb;

        if (!infile) throw eap::InvalidStateException("radiation pattern file " + results_file + " not found");
        do
        {
            pattern_ptr p_pat(new pattern);
            p_pat->m_frequency = m_min_freq + j*m_incr_freq;
            while (std::getline(infile, line) && strncmp(line.c_str(), " DEGREES", 8));

            while (std::getline(infile, line))
            {
                std::istringstream iss(line);
                if (!(iss >> theta >> phi >> vertdb >> horizdb >> totaldb)) break;

                p_pat->m_db_gain.push_back(totaldb);
                if (totaldb > p_eval->m_max_db)
                    p_eval->m_max_db = totaldb;
                if (totaldb < p_eval->m_min_db) 
                    p_eval->m_min_db = totaldb;
            }
            if (p_pat->m_db_gain.size() > 0)
            {
                if (num_polar() != p_pat->m_db_gain.size()) throw eap::InvalidStateException("Problem with reading nec results for " + results_file);
                p_eval->m_radiation.push_back(p_pat);
                read += p_pat->m_db_gain.size();
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

float algorithm::read_coupling(const std::string results_file, unsigned int size)
{
    std::ifstream infile;
    try
    {
        infile.open(results_file);
        std::string line;
        unsigned int count_couplings = 0;
        float coupling = 0.0f, resultant_coupling = 0.0f;

        if (!infile) throw eap::InvalidStateException("coupling file " + results_file + " not found");
        while (count_couplings < size)
        {
            while (std::getline(infile, line) && strncmp(line.c_str(), " ------- COUPLING BETWEEN", 25));
            std::getline(infile, line);
            std::getline(infile, line);

            unsigned int read = 0;
            while (read < size - 1)
            {
                sscanf(line.c_str(), "%*d %*d %*d %*d %*d %*d %f", &coupling);

                if (coupling >= 100)
                    resultant_coupling += 100*2;
                else if (!(coupling <= -100))
                    resultant_coupling += 100 + coupling;
                read++;
            }
            count_couplings++;
        }
        infile.close();
        return resultant_coupling;
    }
    catch (const eap::InvalidStateException &e)
    {
        infile.close();
        throw;
    }
}



float algorithm::compare(const evaluation_ptr &p_first,
        const evaluation_ptr &p_second)
{
    try 
    {
        float diff = 0.0f;
        if (p_first->m_radiation.size() != p_second->m_radiation.size())
            throw eap::InvalidStateException("evaluation::radiation vector sizes don't match");
        for (unsigned int i=0; i<p_first->m_radiation.size(); ++i)
        {
            pattern_ptr p_p1 = p_first->m_radiation[i];
            pattern_ptr p_p2 = p_second->m_radiation[i];
            //why do we need to check for matching frequency?
            if (p_p1->m_frequency != p_p2->m_frequency) throw eap::InvalidStateException("frequencies don't match");
            for (unsigned int j=0; j<num_polar(); ++j)
            {
                diff += powf(fabs(p_p1->m_db_gain[j] - p_p2->m_db_gain[j]), m_exp_weight);
            }
        }
        return diff;
    }
    catch (...)
    {
        throw;
    }
}

individual_ptr algorithm::create_individual(std::string path, std::vector<position_ptr> &placements)
{
    std::ofstream outfile;
    try
    {
        boost::format formatter(path);
        if (m_ant_configs.size() != placements.size())
            throw eap::InvalidStateException("placements size should match #of antennas");

        individual_ptr p_ind(new individual);
        p_ind->m_positions = placements;

        // create radiation file
        for (unsigned int j=0; j<m_ant_configs.size(); ++j)
        {
            outfile.open(str(formatter % j));
            write_platform(outfile);

            int count = m_platform->m_nec_wires.size();
            int excitation_id;
            for (unsigned int k=0; k<m_ant_configs.size(); ++k)
            {
                if (k==j)
                    excitation_id = count+1;
                write_ant(outfile, m_ant_configs[k], placements[k], count+1);
                count += m_ant_configs[k]->m_wires.size();
            }
            write_excitation(outfile, excitation_id);
            outfile.close();
        }

        // create coupling file 
        outfile.open(str(formatter % m_ant_configs.size()));
        write_platform(outfile);
        int count = m_platform->m_nec_wires.size();
        std::vector<int> start_ids;
        for (unsigned int k=0; k<m_ant_configs.size(); ++k)
        {
            start_ids.push_back(count+1);
            write_ant(outfile, m_ant_configs[k], placements[k], count+1);
            count += m_ant_configs[k]->m_wires.size();
        }
        write_coupling(outfile, start_ids);
        

        return p_ind;
    }
    catch (...)
    {
        outfile.close();
        throw;
    }
}


/**
 * @desc Exchange of genotypes between two individuals
 * @param ind1, ind2 - two individuals
 * @return Vector of genetically bred individuals
 */
std::vector<individual_ptr> algorithm::breed(const individual_ptr &p_ind1, const individual_ptr &p_ind2)
{
    try
    {
        std::vector<individual_ptr> children;
        individual_ptr p_child1 (new individual);
        individual_ptr p_child2 (new individual);
        children.push_back(p_child1);
        children.push_back(p_child2);

        unsigned int xover = eap::rand(0, m_ant_configs.size()-1);
        for (unsigned i = 0; i < xover; i++)
        {	
            children[0]->m_positions.push_back(p_ind1->m_positions[i]);
            children[1]->m_positions.push_back(p_ind2->m_positions[i]);
        }

        for (unsigned i = xover; i < m_ant_configs.size(); i++)
        {
            children[0]->m_positions.push_back(p_ind2->m_positions[i]);
            children[1]->m_positions.push_back(p_ind1->m_positions[i]);
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
 * @param individual_ptr
 */
void algorithm::simple_mutation(individual_ptr &p_ind)
{
    try
    {
        int bit = eap::rand(0, m_ant_configs.size() - 1);
        int new_bit = eap::rand(0, m_ant_configs[bit]->m_positions.size()-1);
        p_ind->m_positions[bit] = m_ant_configs[bit]->m_positions[new_bit];
    }
    catch (...)
    {
        throw;
    }

}

void algorithm::save_best_nec(const std::string &algo_id, individual_ptr &p_ind)
{
    std::ofstream outfile;
    try
    {
        std::string path(eap::best_directory + algo_id + std::to_string(eap::rand()) + ".nec");
        outfile.open(path);
        outfile << std::string("CM " + m_platform->m_nec_file + "\n");
        for (ant_config_ptr i_ant : m_ant_configs)
            outfile << std::string("CM " + i_ant->m_nec_file + "\n");
        outfile << std::string("CM fitness=" + std::to_string(p_ind->m_fitness) + "\n");
        write_platform(outfile);
        int count = m_platform->m_nec_wires.size();
        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
        {
            write_ant(outfile, m_ant_configs[i_ant], p_ind->m_positions[i_ant], count+1);
            count += m_ant_configs[i_ant]->m_wires.size();
        }
        outfile.close();
    }
    catch (...)
    {
        outfile.close();
        throw;
    }
}

/**
 * @desc Checks whether new antenna position overlaps a vector of positions
 */ 
bool algorithm::overlap(std::vector<position_ptr> &existing, position_ptr &p_new_pos)
{
    for (position_ptr p_pos : existing)
    {
        if ((p_pos->m_x != p_new_pos->m_x) && (p_pos->m_y != p_new_pos->m_y) && (p_pos->m_z != p_new_pos->m_z)) 
            return true;
    }
    return false;
}

algorithm::~algorithm(void)
{
    m_ant_configs.clear();
    m_ant_configs.shrink_to_fit();
}
