#if 0
#ifndef EAP_RESOURCES_HPP_INCLUDED
#define EAP_RESOURCES_HPP_INCLUDED
#include<iostream>
#include<vector>
#include<fstream>
#include<streambuf>
#include<string>

namespace eap_resources 
{
	using namespace std;

	//Go for forward declarations...http://stackoverflow.com/questions/2297567/where-should-include-be-put-in-c
	
	/**
	* Reading an ASCII file into C++ std::string
	*/
	std::string read_lua(const std::string path);

	/**
	* convert string to bool
	**/
	bool to_bool(std::string);

	xml_node<char>* get_first_node(xml_node<>*, char const *);
	xml_node<char>* get_first_node(xml_node<>*);

	xml_attribute<char>* get_first_attribute(xml_node<>*, char const *, bool mandatory=true);

	float randf(float, float);

	struct EAPException : public std::exception 
	{
		std::string s;
		EAPException(std::string name) : s(name) { } 
		char const* what() const throw() { return s.c_str(); }
		~EAPException() throw() {}
	};

	struct ParseException : public EAPException
	{
		ParseException(std::string msg) : EAPException("XML parse exception: " + msg) {}
	};

	struct FileCreationException : public EAPException
	{
		FileCreationException(std::string msg) : EAPException("File creation exception: " + msg) {}
	};

	struct InvalidStateException : public EAPException
	{
		InvalidStateException(std::string msg) : EAPException("Invalid state exception: " + msg) {}
	};

	const std::string generations_folder = "Generations";

	const std::string iterations_folder = "Iterations";

}
#endif
#endif
