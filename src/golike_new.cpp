#include "../header/golikenew.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include "../header/adb.h"
#include <regex>
#include "../header/json.hpp"
#include <fstream>
#include <unordered_map>
#include <cstdio>
#include <memory>
#include <pybind11/embed.h>
#include <utility>
#include <tuple>
static size_t Write_cb(void* contents, size_t size, size_t nnemb, void* userp) {
  ((std::string*)(userp))->append((char*)(contents), size * nnemb);
  return size * nnemb;
};
namespace py = pybind11;
using json = nlohmann::json;
GolikeClient::GolikeClient(const std::string& token) {
    headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "accept-language: vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5");
    std::string aut = "authorization: " + token;
    headers = curl_slist_append(headers, aut.c_str());
    headers = curl_slist_append(headers, "content-type: application/json;charset=utf-8");
    headers = curl_slist_append(headers, "origin: https://app.golike.net");
    headers = curl_slist_append(headers, "priority: u=1, i");
    headers = curl_slist_append(headers, "sec-ch-ua: \"Chromium\";v=\"146\", \"Not-A.Brand\";v=\"24\", \"Google Chrome\";v=\"146\"");
    headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?1");
    headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Android\"");
    headers = curl_slist_append(headers, "sec-fetch-dest: empty");
    headers = curl_slist_append(headers, "sec-fetch-mode: cors");
    headers = curl_slist_append(headers, "sec-fetch-site: same-site");
    headers = curl_slist_append(headers, "t: VFZSak0wMTZVVFZOZWtsNVQxRTlQUT09");
    headers = curl_slist_append(headers, "user-agent: Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Mobile Safari/537.36");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
}
GolikeClient::~GolikeClient() {
  if (headers) curl_slist_free_all(headers);
  headers = NULL;
  if (curl) curl_easy_cleanup(curl);
}
std::string GolikeClient::performRequest (const std::string& url,const std::string& method,const std::string& postData) const {
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    if (method == "POST"){
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(postData.size()));
    }else {
      curl_easy_setopt(curl,CURLOPT_HTTPGET,1L);
    };
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) throw std::runtime_error(curl_easy_strerror(res));
    return response;
}
GolikeJob GolikeClient::ME() const {
    std::string url = "https://gateway.golike.net/api/users/me";
    std::string raw = performRequest(url);
    json j = json::parse(raw);
    auto d = j["data"]; 
    GolikeJob job;
    job.id = d.value("id", 0);
    job.name = d.value("name", "");
    job.coin = d.value("coin", 0);
    return job;
}
TIKTOK::TIKTOK(const GolikeClient& c) : client(c) {}
std::vector<GolikeJob> TIKTOK::ACCOUNT() const {
    std::string url = "https://gateway.golike.net/api/tiktok-account";
    std::string raw = client.performRequest(url);
    json d = json::parse(raw);
    std::vector<GolikeJob> account;
    account.reserve(d["data"].size());
    for (const auto& i : d["data"]) {
        account.push_back({
            i.value("id", 0),
            0,
            i.value("nickname", "")
        });
    }
    return account;
}
std::vector<GolikeJob>TIKTOK::GETJOB(long long account_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/tiktok/jobs?account_id=" + std::to_string(account_id) + "&data=null";
    std::string raw = client.performRequest(url);
    json j = json::parse(raw);
    auto d = j["data"];
    auto ob = d.value("object_data","");
    json obj = json::parse(ob);
    std::vector<GolikeJob> get;
    get.push_back({d.value("id", 0), 0, "", d.value("link", ""), d.value("object_id", ""), d.value("type", ""), d["lock"].value("object_id", ""),obj.value("pk_id","")});
    return get;
}
std::vector<GolikeJob> TIKTOK::COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const{
    std::string url = "https://gateway.golike.net/api/advertising/publishers/tiktok/complete-jobs";
    std::string body =
        std::string("{") +
        "\"ads_id\":" + std::to_string(ads_id) +
        ",\"account_id\":" + std::to_string(account_id) +
        "}";
    std::string raw = client.performRequest(url, "POST", body);
    json j = json::parse(raw);
    std::vector<GolikeJob> comple;
    GolikeJob i;
    i.message = j.value("message", "");
    comple.push_back(i);
    return comple;
} 
std::vector<GolikeJob> TIKTOK::SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const {
    std::vector<GolikeJob> skip;
    std::string url = "https://gateway.golike.net/api/advertising/publishers/tiktok/skip-jobs";
    std::string body =
          "{"
          "\"ads_id\":" + std::to_string(ads_id) + ","
          "\"object_id\":" + object_id + ","
          "\"account_id\":"+ std::to_string(account_id) + ","
          "\"type\":"+type + ","
          "}";
    std::string raw = client.performRequest (url,"POST",body);
    json d = json::parse(raw);
    GolikeJob i;
    i.message = d.value("message", "");
    skip.push_back(i);
    return skip;
}
int TIKTOK::FOLLOW(const std::string& devices) const {
    ADB adb(devices);
    auto dump = adb.ADB_DUMP();
    if (dump != 0) {
        std::cerr << "DUMP FALSE" << std::endl;
        return 1;
    }
    std::ifstream file("my_ui_dump.xml");
    std::string xml((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    if (xml.find("Following") != std::string::npos) {
        std::cout << "Already followed\n";
        return 0;
    }
    std::regex follow_regex(R"(Follow.*?bounds="\[(\d+),(\d+)\]\[(\d+),(\d+)\]")");
    std::smatch match;
    if (std::regex_search(xml, match, follow_regex)) {
        int x1 = std::stoi(match[1]);
        int y1 = std::stoi(match[2]);
        int x2 = std::stoi(match[3]);
        int y2 = std::stoi(match[4]);

        int x = (x1+x2)/2;
        int y = (y1+y2)/2;
        adb.ADB_CLICK(x,y);
    }
    return 0;
}
int TIKTOK::LIKE(const std::string& devices) const {
    ADB adb(devices);
    int width = 1080;
    int height = 1920;
    int x = width * 0.92;
    int y = height * 0.50;
    auto like = adb.ADB_CLICK(x, y);
    return 0;
}
// void TIKTOK::AUTO(const std::string& devices,long long account_id) const {
//     while(true) {
//         auto get = client.GETJOB(account_id);
//         if (get.empty()) {
//           std::cout << "Lấy job thất bại" << std::endl;
//         }
//         for (auto& info : get) {
//             if (info.Type == "Follow") {
//                auto follow = FOLLOW(devices);
//                if (follow != 0) {
//                  std::cout << "Follow false" << std::endl;
//                  auto skip = client.SKIP(info.id,(info.Type == "Follow" ? info.objectID : info.objectID2), info.id, info.Type);
//                  if (!skip.empty()) {
//                    std::cout << skip[0].message << std::endl;
//                  }
//                  continue;
//                }
//                std::cout << "Follow success\n";
//                auto comple = client.COMPLE(info.id, info.id);
//                if (!comple.empty()) {
//                  std::cout << comple[0].message << std::endl;
//                  continue;
//                }
//                std::cout << "Lỗi comple" << std::endl;
//                auto skip2 = client.SKIP(info.id,(info.Type == "Follow" ? info.objectID : info.objectID2), info.id, info.Type);
//                if (!skip2.empty()) {
//                    std::cout << skip2[0].message << std::endl;
//                  }
//                continue;
//             }
//             if (info.Type == "Like") {
//               auto like = LIKE(devices);
//                  if (like != 0) {
//                     std::cout << "Like false" << std::endl;
//                     auto skip = client.SKIP(info.id,(info.Type == "Like" ? info.objectID : info.objectID2), info.id, info.Type);
//                     if (!skip.empty()) {
//                     std::cout << skip[0].message << std::endl;
//                     }
//                     continue;
//                }
//                std::cout << "Like success\n";
//                auto comple = client.COMPLE(info.id, info.id);
//                if (!comple.empty()) {
//                  std::cout << comple[0].message << std::endl;
//                  continue;
//                }
//                std::cout << "Lỗi comple" << std::endl;
//                auto skip2 = client.SKIP(info.id,(info.Type == "Like" ? info.objectID : info.objectID2), info.id, info.Type);
//                if (!skip2.empty()) {
//                    std::cout << skip2[0].message << std::endl;
//                  }
//                continue;
//             }
//         }
//     }
// }
INSTAGRAM::INSTAGRAM(const GolikeClient& c,const std::string& cookie) : client(c) {
    headers = NULL;
    headers = curl_slist_append(headers, "accept: */*");
    headers = curl_slist_append(headers, "accept-language: vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5");
    headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "origin: https://www.instagram.com");
    headers = curl_slist_append(headers, "priority: u=1, i");
    headers = curl_slist_append(headers, "sec-ch-prefers-color-scheme: light");
    headers = curl_slist_append(headers, "sec-ch-ua: \"Chromium\";v=\"146\", \"Not-A.Brand\";v=\"24\", \"Google Chrome\";v=\"146\"");
    headers = curl_slist_append(headers, "sec-ch-ua-full-version-list: \"Chromium\";v=\"146.0.7680.80\", \"Not-A.Brand\";v=\"24.0.0.0\", \"Google Chrome\";v=\"146.0.7680.80\"");
    headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?0");
    headers = curl_slist_append(headers, "sec-ch-ua-model: \"\"");
    headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Windows\"");
    headers = curl_slist_append(headers, "sec-ch-ua-platform-version: \"10.0.0\"");
    headers = curl_slist_append(headers, "sec-fetch-dest: empty");
    headers = curl_slist_append(headers, "sec-fetch-mode: cors");
    headers = curl_slist_append(headers, "sec-fetch-site: same-origin");
    headers = curl_slist_append(headers, "x-asbd-id: 359341");
    headers = curl_slist_append(headers, "x-bloks-version-id: 61fc9465e13b77eaa110f317859102ba7fb93a0a2bcc08c46473da6713640739");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
};
INSTAGRAM::~INSTAGRAM() {
    if(headers) {
      curl_slist_free_all(headers);
      headers = NULL;
    }
    if (curl) curl_easy_cleanup(curl);
}
std::string INSTAGRAM::requestAPI(const std::string& url,const std::string& body) const {
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl,CURLOPT_POST,1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) throw std::runtime_error(curl_easy_strerror(res));
    return response;
}
std::pair<std::string,std::string> INSTAGRAM::GET_DATA() const {
    std::string url = "https://www.instagram.com/";
    std::string raw = client.performRequest(url);
    std::string avID, Dtsg;
    std::regex av("\"NON_FACEBOOK_USER_ID\":\"([^\"]+)\"");
    std::smatch match;
    if (std::regex_search(raw, match, av)) {
        avID = match[1];
    }
    std::regex tokenRegex(R"---("DTSGInitialData",\[\],\{"token":"([^"]+)")---");
    std::smatch match2;
    if (std::regex_search(raw, match2, tokenRegex)) {
        Dtsg = match2[1];
    }
    return { avID, Dtsg };
}
std::vector<GolikeJob>INSTAGRAM::ACCOUNT() const {
    std::string url = "https://gateway.golike.net/api/instagram-account";
    std::string raw = client.performRequest(url);
    json d = json::parse(raw);
    std::vector<GolikeJob> account;
    account.reserve(d["data"].size());
    for (const auto& i : d["data"]) {
        account.push_back({
            i.value("id", 0),
            0,
            i.value("instagramusername", "")
        });
    }
    return account;
}
std::vector<GolikeJob> INSTAGRAM::GETJOB(long long account_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/instagram/jobs?account_id=" + std::to_string(account_id) + "&data=null";
    std::string raw = client.performRequest(url);
    json j = json::parse(raw);
    auto d = j["data"];
    auto ob = d.value("object_data","");
    json obj = json::parse(ob);
    std::vector<GolikeJob> get;
    get.push_back({d.value("id", 0), 0, "", d.value("link", ""), d.value("object_id", ""), d.value("type", ""), d["lock"].value("object_id", ""),obj.value("pk_id","")});
    return get;
}
std::vector<GolikeJob> INSTAGRAM::COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/instagram/complete-jobs";
    std::string body =
        "{"
        "\"ads_id\":" + std::to_string(ads_id) + ","
        "\"account_id\":" + std::to_string(account_id) + ","
        "\"async\":true"
        "}";
    std::string raw = client.performRequest(url, "POST", body);
    json j = json::parse(raw);
    std::vector<GolikeJob> comple;
    GolikeJob i;
    i.message = j.value("message", "");
    comple.push_back(i);
    return comple;
}
std::vector<GolikeJob> INSTAGRAM::SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const {
    std::vector<GolikeJob> skip;
    std::string url = "https://gateway.golike.net/api/advertising/publishers/instagram/skip-jobs";
    std::string body =
          "{"
          "\"ads_id\":" + std::to_string(ads_id) + ","
          "\"object_id\":" + object_id + ","
          "\"account_id\":" + std::to_string(account_id) + ","
          "\"type\":"+type + ","
          "}";
    std::string raw = client.performRequest (url,"POST",body);
    json d = json::parse(raw);
    GolikeJob i;
    i.message = d.value("message", "");
    skip.push_back(i);
    return skip;
}
std::string INSTAGRAM::LIKE(const std::string& av_id, const std::string& dtsg, const std::string& media_id,const std::string& cookie, std::string type) const {
    char* escaped_dtsg = curl_easy_escape(NULL,dtsg.c_str(),static_cast<int>(dtsg.length()));
    std::string variable = "{\"media_id\":\"" + media_id + "\","
                            "\"container_module\":\"single_post\"}";
    char* escaped_variables = curl_easy_escape(NULL, variable.c_str(), 0);
    std::string body = 
    "av=" + av_id + "&"
    "fb_dtsg=" + std::string(escaped_dtsg) + "&"
    "fb_api_caller_class=RelayModern&"
    "fb_api_req_friendly_name=usePolarisLikeMediaLikeMutation&"
    "server_timestamps=true&"
    "variables=" + escaped_variables + "&"
    "doc_id=23951234354462179";
    curl_free(escaped_dtsg);
    curl_free(escaped_variables);
    std::string url = "https://www.instagram.com/graphql/query";
    std::string csrftoken;
    size_t start = cookie.find("csrftoken=");
    if (start != std::string::npos) {
        start += 10; 
        size_t end = cookie.find(";", start);
        csrftoken = cookie.substr(start, end - start);
    }
    if (!csrftoken.empty()) {
        std::string csrftokenn = "x-csrftoken: " + csrftoken;
        headers = curl_slist_append(headers, csrftokenn.c_str());
    }
    headers = curl_slist_append(headers, "x-fb-lsd: i86AlZ_H15GNJCoDaCpxaZ");
    headers = curl_slist_append(headers, "x-ig-app-id: 936619743392459");
    std::string cookiess = "cookie: " + cookie;
    headers = curl_slist_append(headers,cookiess.c_str());
    headers = curl_slist_append(headers, (type == "like" ? "x-fb-friendly-name: usePolarisLikeMediaLikeMutation" : "x-fb-friendly-name: usePolarisFollowMutation"));
    headers = curl_slist_append(headers,(type == "like" ? "x-root-field-name: xdt_mark_media_like" : "x-root-field-name: xdt_create_friendship" ));
    std::string raw = requestAPI(url, body);
    try {
        json d = json::parse(raw);
        if (d.is_null()) return "";
        if (d["extensions"]["is_final"] == true && d["data"].contains("xdt_mark_media_like")) return "OK";
        if (d["data"].contains("xdt_api__v1__media__media_id__unlike")) return "False";
    }catch(std::exception& e) {
        std::cerr << "PARSE FAILED" << e.what() << std::endl;
    }
    return "";
}
std::string INSTAGRAM::FOLLOW(const std::string& target_id, const std::string& av_id, const std::string& dtsg,const std::string& cookie, std::string type) const {
    char* escaped_dtsg = curl_easy_escape(NULL, dtsg.c_str(), static_cast<int>(dtsg.length()));
    std::string variable =
        "{\"target_user_id\":\"" + target_id + "\","
        "\"container_module\":\"profile\","
        "\"nav_chain\":\"PolarisProfilePostsTabRoot:profilePage:1:via_cold_start\"}";
    std::string escaped_variable = curl_easy_escape(NULL, variable.c_str(), 0);
    std::string body =
        "av=" + av_id + "&"
        "fb_dtsg=" + std::string(escaped_dtsg) + "&"
        "fb_api_caller_class=RelayModern&"
        "fb_api_req_friendly_name=usePolarisFollowMutation&"
        "server_timestamps=true&"
        "variables=" + escaped_variable + "&"
        "doc_id=9740159112729312";
    std::string url = "https://www.instagram.com/graphql/query";
    std::string csrftoken;
    size_t start = cookie.find("csrftoken=");
    if (start != std::string::npos) {
        start += 10; 
        size_t end = cookie.find(";", start);
        csrftoken = cookie.substr(start, end - start);
    }
    if (!csrftoken.empty()) {
        std::string csrftokenn = "x-csrftoken: " + csrftoken;
        headers = curl_slist_append(headers, csrftokenn.c_str());
    }
    headers = curl_slist_append(headers, "x-fb-lsd: i86AlZ_H15GNJCoDaCpxaZ");
    headers = curl_slist_append(headers, "x-ig-app-id: 936619743392459");
    std::string cookiess = "cookie: " + cookie;
    headers = curl_slist_append(headers,cookiess.c_str());
    headers = curl_slist_append(headers, (type == "like" ? "x-fb-friendly-name: usePolarisLikeMediaLikeMutation" : "x-fb-friendly-name: usePolarisFollowMutation"));
    headers = curl_slist_append(headers,(type == "like" ? "x-root-field-name: xdt_mark_media_like" : "x-root-field-name: xdt_create_friendship" ));
    std::string raw = requestAPI(url, body);
    try {
        json d = json::parse(raw);
        if (d.contains("errors") && d["errors"].is_array() && !d["errors"].empty()) {
            std::string msg = d["errors"][0]["message"];
            std::cerr << "Instagram error: " << msg << std::endl;
            return "";
        }
        if (d.contains("data") && d["data"].contains("xdt_create_friendship")) {bool following = d["data"]["xdt_create_friendship"]["friendship_status"]["following"];
            return following ? "OK" : "False";
        }
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return "";
    }
    return "";
}
FACEBOOK::FACEBOOK(const GolikeClient& c) : client(c) {};
std::tuple<std::string,std::string,std::string>FACEBOOK::GET_DATA (const std::string& cookies) const {
    struct curl_slist* headers2 = NULL;
    headers2 = NULL;
    headers2 = curl_slist_append(headers2, "accept: text/html");
    headers2 = curl_slist_append(headers2, "accept-language: vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5");
    headers2 = curl_slist_append(headers2, "priority: u=1, i");
    headers2 = curl_slist_append(headers2, "sec-ch-prefers-color-scheme: light");
    headers2 = curl_slist_append(headers2, "sec-ch-ua: \"Chromium\";v=\"146\", \"Not-A.Brand\";v=\"24\", \"Google Chrome\";v=\"146\"");
    headers2 = curl_slist_append(headers2, "sec-ch-ua-full-version-list: \"Chromium\";v=\"146.0.7680.80\", \"Not-A.Brand\";v=\"24.0.0.0\", \"Google Chrome\";v=\"146.0.7680.80\"");
    headers2 = curl_slist_append(headers2, "sec-ch-ua-mobile: ?0");
    headers2 = curl_slist_append(headers2, "sec-ch-ua-model: \"\"");
    headers2 = curl_slist_append(headers2, "sec-ch-ua-platform: \"Windows\"");
    headers2 = curl_slist_append(headers2, "sec-ch-ua-platform-version: \"10.0.0\"");
    headers2 = curl_slist_append(headers2, "sec-fetch-dest: empty");
    headers2 = curl_slist_append(headers2, "sec-fetch-mode: cors");
    headers2 = curl_slist_append(headers2, "sec-fetch-site: same-origin");
    headers2 = curl_slist_append(headers2, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36");
    std::string cookiess = "cookie: " + cookies;
    headers2 = curl_slist_append(headers2,cookiess.c_str());
    CURL* curl = curl_easy_init();
    if (!curl) return {"","",""};
    std::string response;
    curl_easy_setopt(curl,CURLOPT_URL,"https://www.facebook.com/");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "API ERROR" << std::endl;
      return {"", "",""};
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers2);
    std::string avID, Dtsg, session_id;
    std::regex av("\"accountId\":\"([^\"]+)\"");
    std::smatch match;
    if (std::regex_search(response, match, av)) {
        avID = match[1];
    }
    std::regex tokenRegex(R"---("DTSGInitialData",\[\],\{"token":"([^"]+)")---");
    std::smatch match2;
    if (std::regex_search(response, match2, tokenRegex)) {
        Dtsg = match2[1];
    }
    std::regex session(R"("context":"\{\\"session_id\\":\\"([^"]+))");
    std::smatch match3;
    if (std::regex_search(response,match3,session)) {
      session_id = match3[1];
      session_id.erase(std::remove(session_id.begin(), session_id.end(), '\\'), session_id.end());
    }
    return { avID, Dtsg ,session_id};
}
std::vector<GolikeJob> FACEBOOK::ACCOUNT() const {
    std::string url = "https://gateway.golike.net/api/fb-account?limit=200";
    std::string raw = client.performRequest(url);
    json d = json::parse(raw);
    std::vector<GolikeJob> account;
    account.reserve(d["data"]["data"].size());
    for (const auto& i : d["data"]["data"]) {
        account.push_back({
            i.value("id", 0),
            i.value("user_id",0),
            i.value("fb_id", ""),
            i.value("fb_name","")
        });
    }
    return account;
}
std::vector<GolikeJob> FACEBOOK::GETJOB(long long account_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/get-jobs-2026?fb_id="+std::to_string(account_id)+"&server=sv2";
    std::string raw = client.performRequest(url);
    json j = json::parse(raw);
    std::vector<GolikeJob> get_job;
    GolikeJob info_Job;
    for (auto& job : j["data"]) {
        info_Job.id = job.value("id",0);
        info_Job.objectID = job.value("object_id", "");
        info_Job.Type = job.value("type","");
        info_Job.link = job.value("link", "");
        get_job.push_back(info_Job);
    }
    return get_job;
}
std::vector<GolikeJob> FACEBOOK::COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/complete-jobs-2026";
    std::string body =
        "{"
        "\"object_id\":\"" + object_id + "\","
        "\"job_id\":" + std::to_string(ads_id) + ","
        "\"type\":\""+type+"\","
        "\"uid\":\""+user_id+"\","
        "\"users_fb_account_id\":" + std::to_string(account_id) + ","
        "\"users_advertising_id\":" + std::to_string(ads_id) + ","
        "\"message\":null"
        "}";
    std::string raw = client.performRequest(url,"POST", body);
    json j = json::parse(raw);
    std::vector<GolikeJob> comple;
    GolikeJob i;
    i.message = j.value("message", "");
    comple.push_back(i);
    return comple;
}
std::vector<GolikeJob> FACEBOOK::SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const {
    std::string url = "https://gateway.golike.net/api/report/send";
    std::string body =
        "{"
        "\"description\":\"Tôi không muốn làm Job này\","
        "\"users_advertising_id\":" + std::to_string(ads_id) + ","
        "\"type\":\"ads\","
        "\"fb_id\":" + std::to_string(account_id) + ","
        "\"error_type\":0,"
        "\"provider\":\"facebook\","
        "\"comment\":null"
        "}";
    std::string raw = client.performRequest(url,"POST",body);
    json d = json::parse(raw);
    std::vector<GolikeJob> mess;
    mess.push_back({.message = d["skip"]["msg"]});
    return mess;
}
std::pair<std::string, std::string> FACEBOOK::Feedback_ID(const std::string& url, const std::string& action,const std::string& profile) const {
    try { 
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")("D:/C++/dzno1st-tool/src");
        py::module_ mod = py::module_::import("get_feedback");
        std::string func_name = (action == "like") ? "get_feedback" : 
                                    (action == "follow") ? "get_target" : 
                                    throw std::invalid_argument("Action không hợp lệ");
        py::tuple result = mod.attr(func_name.c_str())(url,profile).cast<py::tuple>();
        std::string id1 = result[0].cast<std::string>();
        std::string id2 = result[1].cast<std::string>();
        return {id1, id2};
    }
    catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << std::endl;
        return {"error", e.what()};
    }
    catch (const std::exception& e) {
        std::cerr << "C++ error: " << e.what() << std::endl;
        return {"error", e.what()};
    }
}
std::string FACEBOOK::LIKE(const std::string& url,const std::string& reaction,const std::string& cookie,const std::string& profile) const {
    auto data = GET_DATA(cookie);
    auto media = Feedback_ID(url,"like",profile);
    static const std::unordered_map<std::string, std::string> reactionMap = {
        {"like", "1635855486666999"},{"like_page","1635855486666999"}, {"love", "1678524932434102"}, {"care", "613557422527858"}, {"haha", "115940658764963"}, {"wow", "478547315650144"}, {"sad", "908563459236466"}, {"angry", "444813342392137"},
    };
    auto it = reactionMap.find(reaction);
    if(it == reactionMap.end())
        return "invalid_reaction";
    std::string reaction_id = it->second;
    try {
       static py::object like_func = py::module_::import("mission").attr("like");
        std::string result = like_func(cookie,std::get<0>(data),std::get<1>(data),media.first,reaction_id,std::get<2>(data)).cast<std::string>();
        return result;
    }
    catch(const py::error_already_set& e){
        std::cerr << e.what() << std::endl;
        return "python_error";
    }
}
std::string FACEBOOK::FOLLOW(const std::string& url,const std::string& cookie,const std::string& profile) const {
  auto data = GET_DATA(cookie);
  auto target = Feedback_ID(url,"follow",profile);
  try {
    static py::object follow_func = py::module_::import("mission").attr("follow");
    std::string result = follow_func(cookie,std::get<0>(data),std::get<1>(data),target.first).cast<std::string>();
    return result;
  }catch(const py::error_already_set& e) {
    std::cerr << e.what() << std::endl;
    return "python_error";
  }
}
