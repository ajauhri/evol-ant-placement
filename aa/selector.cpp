#include "selector.hpp"
#include "aapot_resources.hpp"
#include <vector>
#include <iterator>
#include <iostream>
#include<boost/algorithm/string.hpp>

selector::a_map selector::algorithms_map = selector::create_algorithms_map();

/**
* @desc Parses the AAPOT.xml to 
*/
selector::selector(std::string filename)
{
	this->filename = filename;
	this->aapot_xml = aapot_resources::read_xml_file(filename);
}

/**
* @desc Creates a static map of available algoritms
* @return a map with a mapping of algorithm names to its ALGO_ID
*/
selector::a_map selector::create_algorithms_map() 
{
	a_map map;
	map["GA"] = GA;
	map["SA"] = SA;
	map["HC"] = HC;
	map["ES"] = ES;
	return map;
}

/**
* @desc Identifies the stochastic algorithm as provided in the AAPOT configuration file
* @return ALGO_ID
*/
//#TODO: Test for invalid algorithm identifier
int selector::get_algorithm() 
{
	std::string algo_name;
	std::vector<char> xml_copy(this->aapot_xml.begin(), this->aapot_xml.end());
	xml_copy.push_back('\0');
	xml_document<> doc;
	
	//skip parsing CDATA nodes
	doc.parse<parse_declaration_node | parse_no_data_nodes>(&xml_copy[0]);

	this->node = aapot_resources::get_first_node(aapot_resources::get_first_node(doc.first_node("AAPOT_Parameters"), "algorithm"));
	algo_name = boost::to_upper_copy(std::string(this->node->name()));
	
	return selector::algorithms_map[algo_name];
}

selector::~selector(void)
{
}
