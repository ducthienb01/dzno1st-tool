#include "../header/golike.h"
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include "../header/json.hpp"
using json = nlohmann::json;
static size_t write_callback(void* contents, size_t size, size_t nnemb, void* userp) {
  ((std::string*)(userp))->append((char*)(contents), size * nnemb);
  return size * nnemb;
};
  Golike::Golike(const std::string& authorization) :author(authorization) {
  headers = NULL;
  headers = curl_slist_append(headers, "application/json, text/plain, */*");
  headers = curl_slist_append(headers, "accept-language: en-US,en;q=0.9,vi;q=0.8");
  std::string aut = "authorization: " + author;
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
Golike::~Golike() {
  if (headers) {
    curl_slist_free_all(headers);
    headers = NULL;
  }
}
golikejob Golike::ME() const {
    CURL* curl = curl_easy_init();
    if (!curl) {
      return golikejob{};
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://gateway.golike.net/api/users/me");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cout << "API ERROR" << std::endl;
    }

    json d = json::parse(response);

    if (!d["success"]) {
        std::cerr << "Không có response " << std::endl;
        return golikejob{};
    }
    golikejob user{};
    try {
        user.id = d["data"]["id"].get<long long>();
        user.name = d["data"]["name"].get<std::string>();
        user.coin = d["data"].value("coin", 0);  // Cách an toàn nhất
    } catch (const std::exception& e) {
        std::cerr << "Lỗi khi gán user: " << e.what() << std::endl;
        return golikejob{};
    }

    return user;
}
std::vector<golikejob> Golike::Account(const std::string& platform) {
  std::vector<golikejob> Account;
  CURL* curl = curl_easy_init();
  if (!curl) return Account;
  std::string respose;
  std::string url = "https://gateway.golike.net/api/" + platform + "-account";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respose);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if(res != CURLE_OK) {
    std::cerr << "API Error" << std::endl;
  }
  json d = json::parse(respose);
  if (d["status"]!= 200 ) {
    std::cout << "API can't success" << std::endl;
  }
  static const std::unordered_map<std::string, std::string> name_field_map = {
      {"tiktok", "nickname"},
      {"instagram", "instagram_username"},
  };
  std::string namekey = "nickname";
  auto it = name_field_map.find(platform);
  if (it != name_field_map.end()) {
        namekey = it->second;
    }
  try {
    for(const auto& item : d["data"]) {
      golikejob u;
      u.id = item.value("id", 0);
      u.name = item.value(namekey, "");
      Account.push_back(u);
    }
  } catch (const std::exception& e) {
    std::cerr << "Error while processing items: " << e.what() << std::endl; 
  }
  return Account;
}
std::vector<golikejob> Golike::GETJOB(const std::string& platform, const long long& account_id) {
  std::vector<golikejob> get_job;
  CURL* curl = curl_easy_init();
  if (!curl) return get_job;
  std::string respose;
  std::string url = (platform == "tiktok") 
    ? "https://gateway.golike.net/api/advertising/publishers/" + platform + "/jobs?account_id=" + std::to_string(account_id) + "&data=null"
    : "https://gateway.golike.net/api/advertising/publishers/" + platform + "/jobs?instagram_account_id="+std::to_string(account_id)+"&data=null";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respose);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if(res != CURLE_OK) {
    std::cerr << "API Error" << std::endl;
    return get_job;
  }
  json d = json::parse(respose);
  if (d["status"] != 200) {
    std::cerr << "API Error" << std::endl;
  }
  golikejob u;
  try{  
  auto& data = d["data"];
  u.id   = data.value("id", 0LL);
  u.link = data.value("link", "");
  u.Type = data["type"];
  u.objectID = data.value("object_id", "");
  u.objectID3 = d["lock"]["object_id"];
  if (data.contains("object_data") && data["object_data"].is_string()) {
      std::string obj_data = data["object_data"].get<std::string>();
      json obj = json::parse(obj_data);
      u.objectID2 = obj.value("pk_id", "");
    }
  }catch (std::exception& e) {
      std::cerr << "GET False :" << e.what() << std::endl;
  }
  get_job.push_back(u);
  return get_job;
}
std::vector<golikejob> Golike::COMPLE(const std::string& platform , const int& ads_id , const int& account_id) {
  std::vector<golikejob> comp;
  CURL* curl = curl_easy_init();
  if (!curl) return comp;
  std::string response;
  std::string url = "https://gateway.golike.net/api/advertising/publishers/" + platform + "/complete-jobs";
  std::string body = "{"
    + (platform == "instagram"
        ? "\"instagram_users_advertising_id\":"+ std::to_string(ads_id) + ",\"instagram_account_id\":"+ std::to_string(account_id) + ","
        : "\"ads_id\":" + std::to_string(ads_id) + ",\"account_id\":" + std::to_string(account_id) + ",")
    + "\"async\":true"
    + "}";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    std::cerr << "API ERROR" << std::endl;
    return comp;
  }
  json d = json::parse(response);
  if(d["status"] != 200) {
    std::cerr << "API Error" << std::endl;
    return comp;
  }
  try {
    golikejob u;
    u.message = d.value("message", "");
    comp.push_back(u);
    return comp;
  }catch (std::exception& e) {
    std::cerr << "Parse Error : " << e.what() << std::endl;
  }
}
std::vector<golikejob> Golike::SKIP(const std::string& platform , const int& ads_id ,const std::string& object_id, const int& account_id,const std::string& type) {
  std::vector<golikejob> s;
  CURL* curl = curl_easy_init();
  if (!curl) return s;
  std::string response;
  std::string url = "https://gateway.golike.net/api/advertising/publishers/"+ platform +"/skip-jobs";
  std::string body =
          "{"
          "\"ads_id\":"+std::to_string(ads_id) + ","
          "\"object_id\":" + object_id + ","
          "\"account_id\":"+ std::to_string(account_id) + ","
          "\"type\":"+type + ","
          "}";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    std::cerr << "API ERROR" << std::endl;
    return s;
  }
  json d = json::parse(response);
  if(d["status"] != 200) {
    std::cerr << "API Error" << std::endl;
    return s;
  }
  try {
    golikejob u;
    u.message = d.value("message", "");
    s.push_back(u);
    return s;
  }catch (std::exception& e) {
    std::cerr << "Parse Error : " << e.what() << std::endl;
  }
}