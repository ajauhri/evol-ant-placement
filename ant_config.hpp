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
		std::string m_nec_file; //name of nec file of antenna
		std::vector<position_ptr> m_positions; //positions provided on a platform by the user
		std::vector<wire_ptr> m_wires; //wires as provided in the nec file 
        std::vector<std::string> m_nec_wires; // wires stored as strings to be written to files

};
typedef boost::shared_ptr<ant_config> ant_config_ptr;
#endif
