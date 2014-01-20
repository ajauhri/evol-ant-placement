//For reference on how to read lua cmds in C, look here - http://stackoverflow.com/questions/1438842/iterating-through-a-lua-table-from-c 
#include<iostream>
#include<stdio.h>
#include<string>
#include<limits>
#include<algorithm.hpp>
#include<ant_config.hpp>
#include<position.hpp>
#include<eap_resources.hpp>

extern "C"
{ 
#include<lua/lua.h>
#include<lua/lauxlib.h>
#include<lua/lualib.h>
}

namespace eap
{
	// reading the lua script multiple times (actually twice) since it's making the code look better
	static int load_platform(lua_State *L)
	{
		if (algo)
		{
			const char *p_platform_file = lua_tostring (L, 1);
			ant_config_ptr p_platform(new ant_config);
			p_platform->m_nec_file.assign(p_platform_file);
			algo->m_platform = p_platform;
			//std::cout << "load platform: "<< algo->platform->nec_file <<std::endl;
		}
		return 0;
	}

	static int add_antenna(lua_State *L)
	{
		if (algo)
		{
			const char *p_ant_file = lua_tostring (L, 1);
			ant_config_ptr p_config(new ant_config);
			p_config->m_nec_file.assign(p_ant_file);
			algo->m_ant_configs.push_back(p_config);
			//std::cout << "load antenna: " << algo->ant_configs.back()->nec_file << std::endl;
		}
		return 0;
	}

	static int add_point(lua_State *L)
	{
		if (algo)
		{
			position_ptr p_ant_pos(new position);
			p_ant_pos->m_x = lua_tonumber (L, 1);
			p_ant_pos->m_y = lua_tonumber (L, 2);
			p_ant_pos->m_z = lua_tonumber (L, 3);
			//std::cout<<"x="<<ant_pos->x<<"y="<<ant_pos->y<<"z="<<ant_pos->z<<"\n";
			algo->m_ant_configs.back()->m_positions.push_back(p_ant_pos);
		}
		return 0;
	}

	static lua_State *L = NULL;

	void init_lua()
	{
		if (L)
		{
			return;
		}

		L = luaL_newstate();
		luaL_openlibs(L);

		lua_pushcclosure(L, load_platform, 0);
		lua_setglobal(L, "load_platform");

		lua_pushcclosure(L, add_antenna, 0);
		lua_setglobal(L, "add_antenna");

		lua_pushcclosure(L, add_point, 0);
		lua_setglobal(L, "add_point");
	}

    //refer http://stackoverflow.com/questions/1438842/iterating-through-a-lua-table-from-c 
	float get_fvalue(const std::string key)
	{
        float value = std::numeric_limits<float>::max();
		if (key.empty() || L == NULL)
		{
			throw InvalidStateException("Invalid conditions for function call");
		}
		else
		{
			lua_getglobal(L, "params");
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				if (lua_tostring(L, -2) == key)
				{
					value = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : -1;
				}
				lua_pop(L, 1);
			}
            lua_pop(L, 1);
            if (value == std::numeric_limits<float>::max())
			    throw ParseException("No value found for key - " + key);
            else
                return value;
		}

	}

	std::string get_svalue(const std::string key)
	{
        std::string value;
		if (key.empty() || L == NULL)
		{
			throw InvalidStateException("Invalid conditions for function call");
		}
		else 
		{
			lua_getglobal(L, "params");
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				if (lua_tostring(L, -2) == key)
				{
					value = lua_isstring(L, -1) ? lua_tostring(L, -1) : ""; // `main.cpp` will throw error to state not a valid algorithm
				}
				lua_pop(L, 1);
			}
            lua_pop(L, 1);
            if (value.empty())
			    throw ParseException("No value found for key - " + key);
            else 
                return value;
		}
	}


	void load_lua_lib(const char *name)
	{
		if (luaL_dofile(L, name))
			throw InvalidStateException("Problem with loading lua file");

	}

	void close_lua()
	{
		lua_close(L);
		L = NULL;
	}

}
