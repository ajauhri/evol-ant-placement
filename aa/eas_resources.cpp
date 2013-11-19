#if 0
#include "eap_resources.hpp"
#include<sstream>
#include<string>
#include<iomanip>
#include<algorithm>
#include<cctype>

namespace eap_resources
{
	std::string read_lua(const std::string path) 
	{
		std::string buffer;
		try {
			std::ifstream t(path.c_str());

			// to avoid several allocations and copies of data
			t.seekg(0, std::ios::end);
			buffer.reserve(t.tellg());
			t.seekg(0, std::ios::beg);
	
			buffer.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
		catch(const std::exception &ex) {
			std::cerr<<"Error in reading "<<path.c_str()<<std::endl;
			exit(0);
		}

		return buffer;
	}



	xml_node<char>* get_first_node(xml_node<> *node)
	{
		xml_node<> *ptr = node->first_node();
		if (ptr == 0)
			throw XMLParseException("No child node not found under node " + std::string(node->name()));
		else 
			return ptr;
	}


	xml_attribute<char>* get_first_attribute(xml_node<> *node, char const *name, bool mandatory)
	{
		xml_attribute<> *ptr = node->first_attribute(name);
		if (ptr == 0 && mandatory) 
			throw XMLParseException("No attribute found by the name " + std::string(name));
		else 
			return ptr;
	}


	float randf(float start, float end)
	{
		return (float) (((rand() % 10000) * (end - start))/10000.0 + start);
	}

	bool to_bool(std::string str) 
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		std::istringstream is(str);
		bool b;
		is >> std::boolalpha >> b;
		return b;
	}
}
#endif
