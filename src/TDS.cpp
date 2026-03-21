#include <iostream>
#include "./header/TDS.h"
#include <curl/curl.h>
#include "./header/json.hpp"
#include <thread>
#include <chrono>
using json = nlohmann::json;
static size_t write_cb(char* contents, size_t size, size_t nnemb, std::string* userp) {
    userp->append(contents, size * nnemb);
    return size * nnemb;
}
TDS::TDS(const std::string access_token) : access_token(access_token) {};
std::string TDS::GET_ACCOUNT() {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    std::string url = "https://traodoisub.com/api/?fields=profile&access_token="+access_token+"";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
    std::cerr << "Error" << std::endl;
    return "";
    }
    json d = json::parse(response);
    return response;
}
std::string TDS::CONFIGURATION(const std::string& id_platform) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    std::string url = "https://traodoisub.com/api/?fields=instagram_run&id="+id_platform+"&access_token="+access_token+"";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
    std::cerr << "Error" << std::endl;
    return "";
    }
    json d = json::parse(response);
    return response;
}
std::vector<JOB> TDS::JOB_LIST(const std::string& type_job) {
    std::vector<JOB> list;
    CURL* curl = curl_easy_init();
    if (!curl) return list;

    std::string response;
    std::string url = "https://traodoisub.com/api/?fields=" + type_job + "&access_token=" + access_token;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return list;
    }
    try {
        json d = json::parse(response);
        if (d.contains("data")) {
            auto& data = d["data"];
            for (const auto& ad : data) {
                JOB u;
                u.id   = ad.value("id",   "");
                u.link = ad.value("link", "");
                u.type = ad.value("type", "");
                list.push_back(u);
                }
        } else {
            JOB u;
            u.message = d["error"];
            list.push_back(u);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    return list;
}
std::string TDS::BROWSER_TASK(const std::string& type, const std::string& id_job) {
    CURL* curl = curl_easy_init();
    if (!curl) return "[]";
    std::string response;
    std::string url = "https://traodoisub.com/api/coin/?type=" + type + "&id=" + id_job + "&access_token=" + access_token + "";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "API Error " << std::endl;
        return "[]";
    }
    try {
        json d = json::parse(response);
        if (d["success"] != 200) {
            std::cerr << "Error" << std::endl;
        }
        return response;
    }
    catch (std::exception& e) {
        std::cerr << "Lỗi khi parse response : " << e.what() << std::endl;
    }
}
std::vector<JOB> TDS::RECEIVE_COINS(const std::string& type, const std::string& id_job) {
    std::vector<JOB> receive;
    CURL* curl = curl_easy_init();
    if (!curl) return receive;
    std::string response;
    std::string url = "https://traodoisub.com/api/coin/?type=" + type + "&id=" + id_job + "&access_token=" + access_token + "";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "API ERROR" << std::endl;
        return receive;
    }
    try {
        json d = json::parse(response);
        if (d["success"] != 200 && !d.contains("data")) {
            std::cerr << "Lỗi API khi parse" << std::endl;
        }
        JOB u;
        u.xu = d["data"].value("xu", 0);
        u.job_success = d["data"].value("job_success", 0);
        u.msg = d["data"].value("msg", "");
        receive.push_back(u);
        return receive;
    }
    catch (std::exception& e) {
        std::cerr << "Lỗi khi parse response : " << e.what() << std::endl;
    }
}