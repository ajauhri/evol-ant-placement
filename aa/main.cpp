// main.cpp : Entry point for the console application.
#include<iostream>
#include<fstream>
#include<time.h>
#include<string>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>

#include "eap_resources.hpp"
#include "lua_cmds.hpp"
#if 0
#include "es.hpp"
#include "hc.hpp"
#include "algorithms.hpp"
#endif
#include "ga.hpp"
#include "sa.hpp"
#include "algorithm.hpp"

namespace prg_opts = boost::program_options;

int main(int argc, char* argv[])
{	
	try 
	{
		std::string lua_file;
		
		prg_opts::options_description desc("\nUsage");
		desc.add_options()
			("help,h", "view valid options")
			("input,i", prg_opts::value<std::string>(&lua_file), "Input file");
		
		prg_opts::variables_map vm;
		prg_opts::store(prg_opts::parse_command_line(argc, argv, desc), vm);
		prg_opts::notify(vm);

		if (vm.count("help") || !vm.count("input") )
		{
				std::cout << desc << "\n";
				return 0;
		}
		
		if (!std::ifstream(lua_file.c_str()))
		{
			std::cout << "Can not open input file" << "\n";
			return 0;
		}
		eap::init_lua();
		eap::load_lua_lib(lua_file.c_str());
		algorithm *algo;

		eap::load_lua_lib(lua_file.c_str());
		switch(eap::get_algorithm())
		{
		case GA:
			std::cout<<"Testing GA"<<std::endl;
			algo = new ga();
			break;
		case SA:
			std::cout<<"Testing SA"<<std::endl;
			algo = new sa();
			break;
		case HC:
			std::cout<<"Testing HC"<<std::endl;
			//algo = new hc(lua_file);
			break;
		case ES:
			std::cout<<"Testing ES"<<std::endl;
			//algo = new es(lua_file);
			break;
		default:
			std::cout<<"Not a valid algorithm"<<std::endl;	
		}

		/* load algorithm run parameters */
		algo->setup_algo_params();

		/* load all possible antenna placements */
		//algo->setup_ant_placements();

		/* load all antenna free space patterns */
		//algo->read_free_space_patterns();

		//algo->run();
		eap::close_lua();
	}
	catch (const std::exception &e)
	{
		std::cerr<<e.what() << "\n";
        return 1;
	}

	return 0;
}



