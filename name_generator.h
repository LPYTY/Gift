#pragma once
#include "framework.h"


class name_generator
{
public:
    name_generator();
    name_generator(const string& fname);
    virtual ~name_generator();
    bool init_file(const string& fname);
    string get_name(int* _pid = nullptr);
    void disable(int id);
    void reset();
    bool cancel();
    bool available();
    bool need_reset();
    string get_file_name();

protected:
    bool is_available;
    bool is_need_reset;
    ifstream name_file;
    string file_name;
    default_random_engine dre;
    int num;
    int available_num;
    vector<int> last;
    vector<string> name_list;
    vector<bool> flags;


protected:
    int get_rand();

private:
};

