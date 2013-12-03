// main.cpp : Entry point for the console application.
#include<iostream>
#include<fstream>
#include<time.h>
#include<string>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>

#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<ga.hpp>
#include<sa.hpp>
#include<es.hpp>
#include<hc.hpp>
#include<algorithm.hpp>

namespace prg_opts = boost::program_options;

std::unique_ptr<algorithm> eap::algo = NULL;

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

        switch(eap::get_algorithm())
        {
            case GA:
                std::cout<<"Testing GA"<<std::endl;
                eap::algo.reset(new ga(lua_file));
                break;
            case SA:
                std::cout<<"Testing SA"<<std::endl;
                eap::algo.reset(new sa(lua_file));
                break;
            case HC:
                std::cout<<"Testing HC"<<std::endl;
                eap::algo.reset(new hc(lua_file));
                break;
            case ES:
                std::cout<<"Testing ES"<<std::endl;
                eap::algo.reset(new es(lua_file));
                break;
            default:
                std::cout<<"Not a valid algorithm"<<std::endl;	
        }

        /* load algorithm run parameters */
        eap::algo->setup_algo_params();

        /* load all possible antenna placements */
        eap::algo->setup_ant_placements();

        /* load all wires from nec files */
        eap::algo->load_nec_files();

        /*clean up previous output files */
        eap::algo->setup_run_context();

        /* create antenna free space patterns */
        eap::algo->write_freespace();

        /* run free space pattern files */

        /* read free space pattern results */
        eap::algo->read_freespace();

        //algo->run();
        eap::close_lua();
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what() << "\n";
    }
    return 0;
}



