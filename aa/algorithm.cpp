#include<iostream>
#include<exception>
#include<random>
#include<math.h>
#include<time.h>
#include<boost/filesystem.hpp>
#include "algorithm.hpp"
#include "aapot_resources.hpp"
#include "position.hpp"
#include "individual.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;
using namespace aapot_resources;

namespace 
{
	char const *aapot_parameters_s = "AAPOT_Parameters";
	char const *antenna_placements_s = "antenna_placements";
	char const *antenna_s = "antenna";
	char const *antenna_locator_s = "antenna_locator";
	char const *vehicle_configuration_s = "vehicle_configuration";
	char const *vehicle_model_s = "vehicle_model";
	char const *ancillary_s = "ancillary";
	char const *name_s = "name";
	char const *ancillary_locator_s = "ancillary_locator";
	char const *mount_object_s = "mount_object";
	char const *mount_object_locator_s = "mount_object_locator";
	char const *translation_s = "translation";
	char const *rotation_s = "rotation";
	char const *pattern_available_s = "pattern_available";
	char const *positions_s = "positions";
	char const *position_s = "position";
	char const *antenna_targets_s = "antenna_targets";
	char const *target_s = "target";
	char const *freq_s = "freq";
	char const *filename_s = "filename";

	char const *algorithm_s = "algorithm";
	char const *mutation_probability_s = "mutation_probability";
	char const *exp_weight_s = "exp_weight";
	char const *seed_s = "seed";

	char const *run_directory = "Runs";
}

/**
* @desc Loads the AAPOT configuration file into memory
* @param aapot_filename AAPOT configuration file 
*/
algorithm::algorithm(std::string aapot_file, std::string config_file)
{
	this->aapot_file = aapot_file;
	this->aapot_xml = aapot_resources::read_xml_file(aapot_file);
	this->config_xml = aapot_resources::read_xml_file(config_file);
	seek_algo_node();
	setup_ancillary_nodes();
}

/**
* @desc Traverses to position the read pointer to the algorithm parameter set in the stream buffer
*/
void algorithm::seek_algo_node() 
{
	this->aapot_doc.parse<parse_declaration_node | parse_no_data_nodes | parse_validate_closing_tags>(&this->aapot_xml[0]);
	this->aapot_root = aapot_doc.first_node(aapot_parameters_s);
	this->algo_node = aapot_resources::get_first_node(aapot_resources::get_first_node(this->aapot_root, algorithm_s));
	this->ant_node = aapot_resources::get_first_node(aapot_resources::get_first_node(this->aapot_root, antenna_placements_s), antenna_s);	
}

/**
* @desc Loads all ancillary node data
*/
void algorithm::setup_ancillary_nodes()
{
	config_doc.parse<parse_declaration_node | parse_no_data_nodes | parse_validate_closing_tags>(&this->config_xml[0]);
	config_node = config_doc.first_node(vehicle_configuration_s);
	vehicle_model = aapot_resources::get_first_node(config_node, vehicle_model_s)->value();
	config_node = aapot_resources::get_first_node(config_node, ancillary_s);
	while (config_node != NULL) 
	{
		ancillary_config_ptr config(new ancillary_config);
		config->name = aapot_resources::get_first_node(config_node,name_s)->value();
		config->ancilary_locator = aapot_resources::get_first_node(config_node, ancillary_locator_s)->value();
		config->mount_object = aapot_resources::get_first_node(config_node, mount_object_s)->value();
		config->mount_object_locator = aapot_resources::get_first_node(config_node, mount_object_locator_s)->value();
		config->translation = aapot_resources::get_first_node(config_node, translation_s)->value();
		config->rotation = aapot_resources::get_first_node(config_node, rotation_s)->value();
		ancillary_configs.push_back(config);
		config_node = config_node->next_sibling(ancillary_s);
	}
}


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

/**
* @desc Loads parameters common amongst all algorithms. 
*		Overriden by each algorithm class to load other parameters specific to the algorithm.
*/
void algorithm::setup_algo_params()
{
	this->mutation_probability = atof(aapot_resources::get_first_attribute(this->algo_node, mutation_probability_s)->value());
	this->exp_weight = atof(aapot_resources::get_first_attribute(this->algo_node, exp_weight_s)->value());
	this->auto_seed = aapot_resources::get_first_attribute(this->algo_node, seed_s, false) ? false : true;
	if (!auto_seed)
		this->seed = atoi(aapot_resources::get_first_attribute(this->algo_node, seed_s, false)->value());
}

/**
* @desc Load all antenna placements 
*/
void algorithm::setup_ant_placements()
{
	try
	{
		while (ant_node != NULL)
		{
			ant_config_ptr config (new ant_config);
			config->name = ant_node->first_attribute(name_s)->value();
			config->pattern_available = aapot_resources::to_bool(aapot_resources::get_first_attribute(ant_node, pattern_available_s)->value());
			config->antenna_locator = aapot_resources::get_first_node(ant_node, antenna_locator_s)->value();

			/* antenna_targets */
			xml_node<> *target_node = aapot_resources::get_first_node(aapot_resources::get_first_node(ant_node, antenna_targets_s), target_s);
			while (target_node != NULL)
			{
				target_ptr ant_target (new target);
				ant_target->freq = atof(aapot_resources::get_first_node(target_node, freq_s)->value()); 
				ant_target->filename = aapot_resources::get_first_node(target_node, filename_s)->value();
				config->targets.push_back(ant_target);
				target_node = target_node->next_sibling();
			}

			/* antenna_positions */
			xml_node<> *position_node = aapot_resources::get_first_node(aapot_resources::get_first_node(ant_node, positions_s), position_s);
			while (position_node != NULL) 
			{
				position_ptr ant_pos (new position);
				ant_pos->mount_object = aapot_resources::get_first_node(position_node, mount_object_s)->value();
				ant_pos->mount_object_locator = aapot_resources::get_first_node(position_node, mount_object_locator_s)->value();
				ant_pos->translation = aapot_resources::get_first_node(position_node,translation_s)->value();
				ant_pos->rotation = aapot_resources::get_first_node(position_node, rotation_s)->value();
				config->positions.push_back(ant_pos);
				position_node = position_node->next_sibling();
			}
			ant_configs.push_back(config);
			ant_node = ant_node->next_sibling();
		}
		std::cout<<"Completed loading antenna placements\n";
	}
	catch (const aapot_resources::XMLParseException &e)
	{
		std::cerr<<e.what()<<"\n";
		exit(0);
	}
}

/**
* @desc Reads all free space pattern files provided as <target> tags in AAPOT.xml 
*/
void algorithm::read_free_space_patterns()
{
	for (unsigned i=0; i<ant_configs.size(); i++)
	{
		for (unsigned j=0; j<ant_configs[i]->targets.size(); j++) {
			evaluate_ant_config(*ant_configs[i]->targets[j]);
		}
	}
	std::cout<<"Completed reading free space patterns\n";
}

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
		if(aapot_resources::randf(0, 1.0f) < mutation_probability)
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

algorithm::~algorithm(void)
{
}
