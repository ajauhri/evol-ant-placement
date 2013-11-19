#ifndef EAP_RESOURCES_HPP_INCLUDED
#define EAP_RESOURCES_HPP_INCLUDED
#include<iostream>
#include<vector>
#include<fstream>
#include<streambuf>
#include<string>

namespace eap
{
	//Go for forward declarations...http://stackoverflow.com/questions/2297567/where-should-include-be-put-in-c

	bool to_bool(std::string);

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

	typedef std::map<std::string, algorithms> a_map;

	static a_map create_algorithms_map();

	static a_map algorithms_map;

	int get_algorithm(void);
}
#endif
