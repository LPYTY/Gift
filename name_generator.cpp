#include "name_generator.h"

name_generator::name_generator()
{
	is_available = false;
	is_need_reset = false;
	num = 0;
	available_num = 0;
	dre.seed((unsigned)time(nullptr));
}

name_generator::name_generator(const string& fname):name_generator()
{
	if (init_file(fname)) is_available = true;
	reset();
}



name_generator::~name_generator()
{
}

bool name_generator::init_file(const string& fname)
{
	if (name_file.is_open())name_file.close();
	name_file.open(fname);
	if(!name_file.is_open()) return false;
	file_name = fname;
	is_available = false;
	is_need_reset = false;
	num = 0;
	available_num = 0;
	last.clear();
	flags.clear();
	name_list.clear();
	static string stemp;
	while (1)
	{
		stemp.clear();
		name_file >> stemp;
		if (!stemp.empty())
		{
			num++;
			name_list.push_back(stemp);
			flags.push_back(true);
		}
		if (name_file.eof()) break;
	}
	if (name_list.empty())
	{
		is_need_reset = true;
		return false;
	}
	is_available = true;
	available_num = num;
	return true;
}


string name_generator::get_name(int* _pid)
{
	if (need_reset() || !available())
	{
		//MessageBoxA(NULL, "RESET", "!", MB_OK);
		return string("");
		
	}
	int id = get_rand();
	if (_pid != nullptr)
		*_pid = id;
	return name_list[id];
}

void name_generator::disable(int id)
{
	if (id >= flags.size()) return;
	flags[id] = false;
	last.push_back(id);
	available_num--;
	if (available_num == 0)
		is_need_reset = true;
}

void name_generator::reset()
{
	for (auto i = flags.begin(); i < flags.end(); i++)
	{
		*i = true;
	}
	last.clear();
	available_num = num;
	is_need_reset = false;
}

bool name_generator::cancel()
{
	if(!last.size()) return false;
	flags[last[last.size()-1]] = true;
	last.pop_back();
	available_num++;
	if (is_need_reset)
		is_need_reset = false;
	return true;
}

bool name_generator::available()
{
	return is_available;
}

bool name_generator::need_reset()
{
	return is_need_reset;
}

string name_generator::get_file_name()
{
	return file_name;
}

int name_generator::get_rand()
{
	uniform_int_distribution<int> uid(0, available_num - 1);
	int k = uid(dre);
	int id = 0;
	while (id < flags.size() && (k > 0 || (k <= 0 && flags[id] == false)))
	{
		if (flags[id])
			k--;
		id++;	
	}
	return id;
}


