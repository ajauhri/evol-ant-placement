#ifndef LUA_CMDS_HPP_INCLUDED
#define LUA_CMDS_HPP_INCLUDED
#include<string>
namespace eap
{
	void init_lua(void);
	void close_lua(void);
	void load_lua_lib(const char*);
	float get_fvalue(const std::string);
	std::string get_svalue(const std::string);
}
#endif
