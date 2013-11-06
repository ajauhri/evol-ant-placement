#ifndef AAPOT_RESOURCES_HPP_INCLUDED
#define AAPOT_RESOURCES_HPP_INCLUDED
#include<iostream>
#include<vector>
#include<fstream>
#include<streambuf>
#include<string>
#include "rapidxml/rapidxml.hpp"

namespace aapot_resources 
{
	using namespace rapidxml;
	using namespace std;

	//Go for forward declarations...http://stackoverflow.com/questions/2297567/where-should-include-be-put-in-c
	/**
	* Reading an ASCII file into C++ std::string
	*/
	std::string read_xml_file(const std::string path);

	/**
	* Get the node having initialization parameters for the algorithm node
	*/
	xml_node<>* algo_node(xml_document<>, std::vector<char>);

	/**
	* convert string to bool
	**/
	bool to_bool(std::string);

	xml_node<char>* get_first_node(xml_node<>*, char const *);
	xml_node<char>* get_first_node(xml_node<>*);

	xml_attribute<char>* get_first_attribute(xml_node<>*, char const *, bool mandatory=true);

	float randf(float, float);

	struct AAPOTException : public std::exception 
	{
		std::string s;
		AAPOTException(std::string name) : s(name) { } 
		char const* what() const throw() { return s.c_str(); }
		~AAPOTException() throw() {}
	};

	struct XMLParseException : public AAPOTException
	{
		XMLParseException(std::string msg) : AAPOTException("XML parse exception: " + msg) {}
	};

	struct FileCreationException : public AAPOTException
	{
		FileCreationException(std::string msg) : AAPOTException("File creation exception: " + msg) {}
	};

	struct InvalidStateException : public AAPOTException
	{
		InvalidStateException(std::string msg) : AAPOTException("Invalid state exception: " + msg) {}
	};

	const std::string generations_folder = "Generations";

	const std::string iterations_folder = "Iterations";

}
#endif
