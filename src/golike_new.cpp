#include "../header/golikenew.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <curl/curl.h>
static size_t Write_cb(char* contents,size_t size, size_t nnemb, std::string* userp) {
    userp -> append(contents,size * nnemb);
    return size * nnemb;
}
GolikeClient::GolikeClient(std::unique_ptr<PlatformStrategy> s, const std::string& token) : strategy(std::move(s)), authorization(token) {
    headers = NULL;
    headers = curl_slist_append(headers, "application/json, text/plain, */*");
    headers = curl_slist_append(headers, "accept-language: en-US,en;q=0.9,vi;q=0.8");
    std::string aut = "authorization: " + authorization;
    headers = curl_slist_append(headers, aut.c_str());
    headers = curl_slist_append(headers, "content-type: application/json;charset=utf-8");
    headers = curl_slist_append(headers, "origin: https://app.golike.net");
    headers = curl_slist_append(headers, "priority: u=1, i");
    headers = curl_slist_append(headers, "sec-ch-ua: \"Not=A?Brand\";v=\"8\", \"Chromium\";v=\"145\", \"Google Chrome\";v=\"145\"");
    headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?1");
    headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Android\"");
    headers = curl_slist_append(headers, "sec-fetch-dest: empty");
    headers = curl_slist_append(headers, "sec-fetch-mode: cors");
    headers = curl_slist_append(headers, "sec-fetch-site: same-site");
    headers = curl_slist_append(headers, "t:VFZSak0wMUVUWGRPUkUweFRsRTlQUT09");
    headers = curl_slist_append(headers, "user-agent: Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Mobile Safari/537.36");
}
GolikeClient::~GolikeClient() {
  if (headers) curl_slist_free_all(headers);
}
std::string GolikeClient::performRequest (const std::string& url,const std::string& method = "GET",const std::string& postData) const {
    thread_local CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("curl init failed");
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    if (method == "POST"){
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(postData.size()));
    };
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) throw std::runtime_error(curl_easy_strerror(res));
    return response;
}
GolikeJob GolikeClient::ME () const {
    std::string url = "https://api.golike.net/api/me";
    std::string raw = client.performRequest(url);
    json j = json::parse(raw);
    auto d = j["data"];
    return {d.value("id", 0), d.value("name", ""), d.value("coin", 0)};
}
TIKTOK::TIKTOK(const GolikeClient& c) : client(c) {}
TIKTOK::~TIKTOK() {}
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
std::vector<GolikeJob> TIKTOK::COMPLE(const int& ads_id, const int& account_id) const{
    std::string url = "https://gateway.golike.net/api/advertising/publishers/tiktok/complete-jobs";
    std::string body = "{"
     "\"ads_id\":" + std::to_string(ads_id) + ",\"account_id\":" + std::to_string(account_id) + ","
    + "\"async\":true"
    + "}";
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
          "\"ads_id\":"+std::to_string(ads_id) + ","
          "\"object_id\":" + object_id + ","
          "\"account_id\":"+ std::to_string(account_id) + ","
          "\"type\":"+type + ","
          "}";
    std::string raw = client.performRequest (url,"POST",body);
    json d = json::parse(raw);
    GolikeJob i;
    i.message = j.value("message", "");
    skip.push_back(i);
    return skip;
}
INSTAGRAM::INSTAGRAM(const GolikeClient& c) : client(c) {};
INSTAGRAM::~INSTAGRAM() {};
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
std::vector<GolikeJob> INSTAGRAM::COMPLE(const int& ads_id, const int& account_id) const {
    std::string url = "https://gateway.golike.net/api/advertising/publishers/instagram/complete-jobs";
    std::string body = "{"
     "\"ads_id\":" + std::to_string(ads_id) + ",\"account_id\":" + std::to_string(account_id) + ","
    + "\"async\":true"
    + "}";
    std::string raw = client.performRequest(url, "POST", body);
    json j = json::parse(raw);
    std::vector<GolikeJob> comple;
    GolikeJob i;
    i.message = j.value("message", "");
    comple.push_back(i);
    return comple;
}
std::Vector<GolikeJob>INSTAGRAM::SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const {
    std::vector<GolikeJob> skip;
    std::string url = "https://gateway.golike.net/api/advertising/publishers/tiktok/skip-jobs";
    std::string body =
          "{"
          "\"ads_id\":"+std::to_string(ads_id) + ","
          "\"object_id\":" + object_id + ","
          "\"account_id\":"+ std::to_string(account_id) + ","
          "\"type\":"+type + ","
          "}";
    std::string raw = client.performRequest (url,"POST",body);
    json d = json::parse(raw);
    GolikeJob i;
    i.message = j.value("message", "");
    skip.push_back(i);
    return skip;
}