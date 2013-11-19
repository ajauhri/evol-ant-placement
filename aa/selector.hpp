#ifndef SELECTOR_HPP_INCLUDED
#define SELECTOR_HPP_INCLUDED
#include <string>
#include <map>
#include <vector>
#include "algorithms.hpp"
#include "ant_config.hpp"

class selector
{
private:
	std::string aapot_xml;
	std::string filename;
	xml_node<> *node;
	typedef std::map<std::string, algorithms> a_map;
	static a_map create_algorithms_map();
	static a_map algorithms_map;


public:
	selector(std::string);
	int get_algorithm();
	~selector(void);
};
#endif
