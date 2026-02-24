#include <iostream>
#include "./header/json.hpp"
#include <vector>
struct JOB {
    std::string id, link, type, message;
    std::string msg;
    int xu, job_success;
};
class TDS {
private:
    std::string access_token;
public:
    TDS(std::string access_token);
    std::string GET_ACCOUNT();
    std::string CONFIGURATION(const std::string& id_platform);
    std::vector<JOB> JOB_LIST(const std::string& type_job);
    std::string BROWSER_TASK(const std::string& type, const std::string& id_job);
    std::vector <JOB> RECEIVE_COINS(const std::string& type , const std::string& id_job);
};