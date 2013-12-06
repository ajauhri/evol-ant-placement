#ifndef ANT_CONFIG_HPP_INCLUDED
#define ANT_CONFIG_HPP_INCLUDED
#include "position.hpp"
#include "wire.hpp"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>

/* ant_config used to store possible positions of different antennas and aslo the platform. The platform will not have any positions */

class ant_config
{
	public:
		std::string nec_file; //name of nec file of antenna
		std::vector<position_ptr> positions; //positions provided on a platform by the user
		std::vector<wire_ptr> wires; //wires as provided in the nec file 
        std::vector<std::string> nec_wires; // wires stored as strings to be written to files

        ~ant_config(void)
        {
            positions.clear();
            positions.shrink_to_fit();

            wires.clear();
            wires.shrink_to_fit();

            nec_wires.clear();
            nec_wires.shrink_to_fit();
        }
};
typedef boost::shared_ptr<ant_config> ant_config_ptr;
#endif
