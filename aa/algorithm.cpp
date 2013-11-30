#include<iostream>
#include<exception>
#include<random>
#include<math.h>
#include<time.h>
#include<boost/filesystem.hpp>
#include<sstream>

#include<algorithm.hpp>
#include<eap_resources.hpp>
#include<position.hpp>
#include<individual.hpp>
#include<lua_cmds.hpp>
#include<wire.hpp>

namespace 
{
    char const *algorithm_s = "algorithm";
    char const *mutation_s = "mutation";
    char const *exp_weight_s = "exp_weight";
    char const *run_directory = "Runs";
}

/**
 * @desc Loads the AAPOT configuration file into memory
 * @param aapot_filename AAPOT configuration file 
 */
algorithm::algorithm(std::string lua_file)
{
    this->lua_file = lua_file;
    //setup_ancillary_nodes();
}

void algorithm::setup_algo_params()
{
    try
    {
        this->mutation = eap::get_fvalue(mutation_s);
        this->exp_weight = eap::get_fvalue(exp_weight_s);
        std::cout<<"Completed generic algo parameter setup"<<std::endl;
    }
    /*
       this->auto_seed = aapot_resources::get_first_attribute(this->algo_node, seed_s, false) ? false : true;
       if (!auto_seed)
       this->seed = atoi(aapot_resources::get_first_attribute(this->algo_node, seed_s, false)->value());*/
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
    }
}


#if 0
/**
 * @desc 1. Cleaning of old runs from FS
 *       2. Seed for pseudo-random number generator
 */
void algorithm::setup_run_context()
{
    if (this->auto_seed)
        srand(time(NULL));
    else
        srand(this->seed);

    boost::filesystem::remove_all(run_directory);
    boost::filesystem::create_directory(run_directory);
}
#endif
/**
 * @desc Load all antenna placements 
 */
void algorithm::setup_ant_placements()
{
    try
    {
        eap::load_lua_lib(this->lua_file.c_str());
        std::cout<<"Completed loading antenna placements\n";
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
void algorithm::load_wires()
{
    // first load platform wires
    this->platform->wires = load_wire(this->platform->nec_file, "platform file corrupted");

    // then load all antenna wires
    for (ant_config_ptr ant : this->ant_configs)
    {
        ant->wires = load_wire(ant->nec_file, ant->nec_file + "file corrupted");
    }
}

std::vector<wire_ptr> algorithm::load_wire(const std::string& nec_file, const std::string& err_msg)
{
    try 
    {
        std::vector<wire_ptr> wires;
        std::ifstream infile(nec_file);
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
                std::cout<<"GW "<<wires.back()->a->x<<" "<<wires.back()->a->y<<"\n";
            }
        } 
       std::cout<<"Completed loading wires for "<<nec_file<<"\n";
       return wires;
    }
    catch(const eap::ParseException &e)
    {
        std::cerr<<e.what()<<"\n";
        exit(0);
    }
}

/**
 * @desc Setup all free space pattern files  
 */
void algorithm::setup_free_space_patterns()
{
    /*
       for (unsigned i=0; i<ant_configs.size(); i++)
       {
       for (unsigned j=0; j<ant_configs[i]->targets.size(); j++) {
       evaluate_ant_config(*ant_configs[i]->targets[j]);
       }
       }*/
    std::cout<<"Completed reading free space patterns\n";
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
 * @desc Checks if antenna position overlaps with any of the ancillary points 
 * @return true or false
 */
bool algorithm::overlaps_ancillary(position_ptr p)
{
    for (unsigned i=0; i<ancillary_configs.size(); i++)
    {
        if (!std::strcmp(p->mount_object.c_str(), ancillary_configs[i]->mount_object.c_str()) &&
                !std::strcmp(p->mount_object_locator.c_str(), ancillary_configs[i]->mount_object_locator.c_str())) {
            return true;
        }
    }
    return false;
}



/**
 * @desc Serializes an individual into a file readable by simulator
 * @param ind Individual (an individual represents the config file)
 * @param file_path 
 */
void algorithm::write_to_file(individual_ptr ind, std::string file_path)
{
    /* generating the XML hierarchy */
    xml_document<> doc;

    try 
    {
        xml_node<>* decl = doc.allocate_node(node_declaration);
        decl->append_attribute(doc.allocate_attribute("version", "1.0"));
        decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        doc.append_node(decl);

        /* root node */
        xml_node<>* root = doc.allocate_node(node_element, vehicle_configuration_s);
        doc.append_node(root);

        /* vehicle name node */
        xml_node<>* vehicle_node = doc.allocate_node(node_element, vehicle_model_s);
        vehicle_node->value(vehicle_model.c_str());
        root->append_node(vehicle_node);

        for (unsigned i_ancillary=0; i_ancillary<ancillary_configs.size(); i_ancillary++)
        {
            /* ancillary node */
            xml_node<>* ancillary_node = doc.allocate_node(node_element, ancillary_s);
            root->append_node(ancillary_node);

            /* name node */
            xml_node<>* name_node = doc.allocate_node(node_element, name_s);
            name_node->value(ancillary_configs[i_ancillary]->name.c_str());
            ancillary_node->append_node(name_node);

            /* ancillary locator node */
            xml_node<>* ancillary_locator_node = doc.allocate_node(node_element, ancillary_locator_s);
            ancillary_locator_node->value(ancillary_configs[i_ancillary]->ancilary_locator.c_str());
            ancillary_node->append_node(ancillary_locator_node);

            /* mount object node */
            xml_node<>* mount_object_node = doc.allocate_node(node_element, mount_object_s);
            mount_object_node->value(ancillary_configs[i_ancillary]->mount_object.c_str());
            ancillary_node->append_node(mount_object_node);

            /* mount object locator node */
            xml_node<>* mount_object_locator_node = doc.allocate_node(node_element, mount_object_locator_s);
            mount_object_locator_node->value(ancillary_configs[i_ancillary]->mount_object_locator.c_str());
            ancillary_node->append_node(mount_object_locator_node);

            /* translation node */
            xml_node<>* translation_node = doc.allocate_node(node_element, translation_s);
            translation_node->value(ancillary_configs[i_ancillary]->translation.c_str());
            ancillary_node->append_node(translation_node);

            /* rotation node */
            xml_node<>* rotation_node = doc.allocate_node(node_element, rotation_s);
            rotation_node->value(ancillary_configs[i_ancillary]->rotation.c_str());
            ancillary_node->append_node(rotation_node);
        }


        for (unsigned i_ant=0; i_ant<ant_configs.size(); i_ant++) 
        {
            /* antenna node */
            xml_node<>* ant_node = doc.allocate_node(node_element, antenna_s);
            root->append_node(ant_node);

            /* name node */
            xml_node<>* name_node = doc.allocate_node(node_element, name_s);
            name_node->value(ind->ant_configs[i_ant]->name.c_str());
            ant_node->append_node(name_node);

            /*antenna_locator node*/
            xml_node<>* ant_loc_node = doc.allocate_node(node_element, antenna_locator_s);
            ant_loc_node->value(ind->ant_configs[i_ant]->antenna_locator.c_str());
            ant_node->append_node(ant_loc_node);

            /*mount_object node*/
            xml_node<>* m_obj_node = doc.allocate_node(node_element, mount_object_s);
            m_obj_node->value(ind->ant_configs[i_ant]->positions.front()->mount_object.c_str());
            ant_node->append_node(m_obj_node);

            /*mount_object_locator node*/
            xml_node<>* m_obj_loc_node = doc.allocate_node(node_element, mount_object_locator_s);
            m_obj_loc_node->value(ind->ant_configs[i_ant]->positions.front()->mount_object_locator.c_str());
            ant_node->append_node(m_obj_loc_node);

            /*translation node*/
            xml_node<>* trans_node = doc.allocate_node(node_element, translation_s);
            trans_node->value(ind->ant_configs[i_ant]->positions.front()->translation.c_str());
            ant_node->append_node(trans_node);

            /*translation node*/
            xml_node<>* rota_node = doc.allocate_node(node_element, rotation_s);
            rota_node->value(ind->ant_configs[i_ant]->positions.front()->rotation.c_str());
            ant_node->append_node(rota_node);
        }
    }
    catch(const std::bad_alloc &ba)
    {
        std::cerr<<"Bad allocation exception at creating config.xml for an individual"<<ba.what()<<"\n";
        exit(0);
    }


    std::string xml_as_string;
    /* flag print_no_indenting removes whitespaces */
    print(std::back_inserter(xml_as_string), doc, print_no_indenting);

    std::ofstream os (file_path, std::ios::trunc);

    xml_as_string.erase(std::remove(xml_as_string.begin(), xml_as_string.end(), '\t'), xml_as_string.end());
    os << xml_as_string;
    os.close();
}

/**
 * @desc Run the simulation & poll it completes
 */
void algorithm::run_simulation()
{
    //TODO this should run all files present under a certain location

}

/**
 * @desc Evaluate the fitness for a given configuration id
 * @param config_id Configuration ID
 */
void algorithm::evaluate_ant_config(target& ant_target)
{
    //Read the results and store the fitness
    char buffer[300];
    FILE* fp = fopen(ant_target.filename.c_str(), "r");
    if (fp != NULL)
    {
        float theta, phi, gain_theta, gain_phi, phase_theta, phase_phi;        
        do
        {
            while ((fgets(buffer, 300, fp) != NULL) && strncmp(buffer, "end_<parameters>", 16));	// find the start of the radiation data
            while (fgets(buffer, 300, fp) != NULL)
            {
                if (sscanf(buffer, "%f %f %f %f %f %f", &theta, &phi, &gain_theta, &gain_phi, &phase_theta, &phase_phi) != 6) break;
                ant_target.db_gain.push_back(cal_totaldb(gain_theta, gain_phi));
            }
        }
        while (!feof(fp));
        fclose(fp);
    }
    else 
    {
        std::cerr<<"Target file not found: "<<ant_target.filename<<std::endl;
        exit(0);
    }
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

/**
 * @desc For debugging...
 */
void algorithm::print_individual(individual_ptr ind)
{
    for (unsigned i=0; i<ind->ant_configs.size(); i++)
    {
        std::cout<<"Ant configs "<<ind->ant_configs.at(i)->antenna_locator<<" "<<ind->ant_configs.at(i)->name<<std::endl;
        std::cout<<"Position "<<ind->ant_configs.at(i)->positions.back()->mount_object<<" "<<ind->ant_configs.at(i)->positions.back()->mount_object_locator<<" "<<ind->ant_configs.at(i)->positions.back()->rotation<<" "<<ind->ant_configs.at(i)->positions.back()->translation<<std::endl;
    }
}
#endif
algorithm::~algorithm(void)
{
}
