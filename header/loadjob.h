#ifndef Load_job_h
#define Load_job_h
#include <iostream>
#include "./header/golike.h"
#include "./header/instagram.h"
#include "./header/json.hpp"
#include <thread>
#include <chrono>
class Loadjob {
private:
    Golike& golike;
    instagram insta;
public:
    Loadjob(Golike& g, const std::string& instagram_Cookie);
    void run(const std::string& platform, const long long& account_id);
};
#endif