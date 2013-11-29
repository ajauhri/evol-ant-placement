//For reference on how to read lua cmds in C, look here - http://stackoverflow.com/questions/1438842/iterating-through-a-lua-table-from-c 
#include<iostream>
#include<stdio.h>
#include<string>
#include<algorithm.hpp>
#include<ant_config.hpp>
#include<position.hpp>
#include<eap_resources.hpp>

extern "C"
{ 
#include<lua5.2/lua.h>
#include<lua5.2/lauxlib.h>
#include<lua5.2/lualib.h>
}

namespace eap
{
	// reading the lua script multiple times (actually twice) since it's making the code look better
	static int load_platform(lua_State *L)
	{
		if (algo)
		{
			const char *platform_file = lua_tostring (L, 1);
			ant_config_ptr platform(new ant_config);
			platform->name.assign(platform_file);
			algo->platform = platform;
			std::cout << "load platform: "<< algo->platform->name <<std::endl;
		}
		return 0;
	}

	static int add_antenna(lua_State *L)
	{
		if (algo)
		{
			const char *ant_file = lua_tostring (L, 1);
			ant_config_ptr config(new ant_config);
			config->name.assign(ant_file);
			algo->ant_configs.push_back(config);
			std::cout << "load antenna: " << algo->ant_configs.back()->name << std::endl;
		}
		return 0;
	}

	static int add_point(lua_State *L)
	{
		if (algo)
		{
			position_ptr ant_pos(new position);
			ant_pos->x = lua_tonumber (L, 1);
			ant_pos->y = lua_tonumber (L, 2);
			ant_pos->z = lua_tonumber (L, 3);
			std::cout<<"x="<<ant_pos->x<<"y="<<ant_pos->y<<"z="<<ant_pos->z<<"\n";
			algo->ant_configs.back()->positions.push_back(ant_pos);
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

	float get_fvalue(const std::string key)
	{
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
					return lua_isnumber(L, -1) ? lua_tonumber(L, -1) : -1;
				}
				lua_pop(L, 1);
			}
			throw ParseException("No value found for key - " + key);
		}
	}

	std::string get_svalue(const std::string key)
	{
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
					return lua_isstring(L, -1) ? lua_tostring(L, -1) : ""; // `main.cpp` will throw error to state not a valid algorithm
				}
				lua_pop(L, 1);
			}
			throw ParseException("No value found for key - " + key);
		}
	}


	void load_lua_lib(const char *name)
	{
		int ret = luaL_dofile(L, name);
		if (ret == 1)
			throw InvalidStateException("Problem with loading lua file");

	}

	void close_lua()
	{
		lua_close(L);
		L = NULL;
	}

}
