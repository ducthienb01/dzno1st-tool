#include "/Users/ducthien/C++/header/instagram.h"
#include <iostream>
#include "/Users/ducthien/C++/header/json.hpp"
#include <curl/curl.h>
#include <regex>
using json = nlohmann::json;
static size_t Write_cb(void* contents , size_t size,size_t nnemb,std::string* userp) {
  userp->append((char*)(contents), size * nnemb);
  return size * nnemb;
};
instagram::instagram(const std::string& cookies) :cookie(cookies) {
  headers = curl_slist_append(headers, "accept: '*/*");
  headers = curl_slist_append(headers, "accept-language:en-US,en;q=0.9,vi;q=0.8");
  headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
  headers = curl_slist_append(headers, "origin: https://www.instagram.com");
  headers = curl_slist_append(headers, "referer: https://www.instagram.com");
  headers = curl_slist_append(headers, "sec-ch-ua: \"Google Chrome\";v=\"143\", \"Chromium\";v=\"143\", \"Not A(Brand\";v=\"24\"");
  headers = curl_slist_append(headers, "sec-ch-ua-full-version-list': \"Google Chrome\";v=\"143.0.7499.41\", \"Chromium\";v=\"143.0.7499.41\", \"Not A(Brand\";v=\"24.0.0.0\"");
  headers = curl_slist_append(headers, "sec-ch-ua-platform-version': '\"26.2.0\"");
  headers = curl_slist_append(headers, "user-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/143.0.0.0 Safari/537.36");
  headers = curl_slist_append(headers, "x-asbd-id: 359341");
  headers = curl_slist_append(headers, "x-bloks-version-id: cc4d2103131ee3bbc02c20a86f633b7fb7a031cbf515d12d81e0c8ae7af305dd");
  std::string csrf_token;
  std::string key = "csrftoken=";
  size_t pos = cookie.find(key);
  if (pos != std::string::npos) {
    size_t pos_equal = pos + key.length();
    size_t end = cookie.find(';', pos_equal);
    if (end == std::string::npos) end = cookies.length();
        csrf_token = cookies.substr(pos_equal, end - pos_equal);
  }
  std::string csrf = "x-csrftoken:" + csrf_token;
  headers = curl_slist_append(headers, csrf.c_str());
  headers = curl_slist_append(headers, "x-fb-friendly-name: usePolarisLikeMediaLikeMutation");
  headers = curl_slist_append(headers, "x-ig-app-id: 936619743392459");
  headers = curl_slist_append(headers, "x-root-field-name: xdt_mark_media_like");
  std::string cookiess = "cookie: " + cookie;
  headers = curl_slist_append(headers, cookiess.c_str());
}
instagram::~instagram() {
    if (headers) {
      curl_slist_free_all(headers);
      headers = NULL;
    }
}
std::pair<std::string, std::string> instagram::Getdata() {
    std::string avID, Dtsg;
    CURL* curl = curl_easy_init();
    if (!curl) return {};
    std::string response;
    curl_easy_setopt(curl,CURLOPT_URL,"https://www.instagram.com");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
      std::cout << "API error" << std::endl;
    }
    try {
      std::regex av_Id("\"NON_FACEBOOK_USER_ID\":\"([^\"]+)\"");
      std::smatch match;
      if (std::regex_search(response, match, av_Id)) {
        avID = match[1];
      }
      std::regex tokenRegex(R"---("DTSGInitialData",\[\],\{"token":"([^"]+)")---");
      std::smatch match2;
      if (std::regex_search(response, match2, tokenRegex)) {
        Dtsg = match2[1];
      }
      return { avID, Dtsg };
      }
    catch (std::exception& e) {
      std::cout << "Lỗi : " << e.what() << std::endl;
      return {};
    }
}
std::string instagram::Like(const std::string& av_id, const std::string& dtsg, const std::string& media_id) {
  CURL* curl = curl_easy_init();
  if (!curl) return "";
  std::string response;
  char* escaped_dtsg = curl_easy_escape(curl,dtsg.c_str(),static_cast<int>(dtsg.length()));
  std::string variable = "{\"media_id\":\"" + media_id + "\","
                        "\"container_module\":\"single_post\"}";
  std::string body = 
    "av=" + av_id + "&"
    "fb_dtsg=" + std::string(escaped_dtsg) + "&"
    "fb_api_caller_class=RelayModern&"
    "fb_api_req_friendly_name=usePolarisLikeMediaLikeMutation&"
    "server_timestamps=true&"
    "variables=" + curl_easy_escape(curl, variable.c_str(),0) + "&"
    "doc_id=23951234354462179";
  curl_easy_setopt(curl,CURLOPT_URL,"https://www.instagram.com/graphql/query");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    std::cerr << "API Error" << std::endl;
    return "";
  }
  try {
    json d = json::parse(response);
    if (d.is_null()) return "";
    if (d["extensions"]["is_final"] == true && d["data"].contains("xdt_mark_media_like")) return "OK";
    if (d["data"].contains("xdt_api__v1__media__media_id__unlike")) return "False";
    // return response;
  } catch (std::exception& e) {
    std::cerr << "Lỗi api " << e.what() << std::endl;
    return "";
  };
}
std::string instagram::Follow(const std::string& target_id, const std::string& av_id, const std::string& dtsg) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    struct curl_slist* new_headers = nullptr;
    for (struct curl_slist* p = headers; p != nullptr; p = p->next) {
        new_headers = curl_slist_append(new_headers, p->data);
    }
    new_headers = curl_slist_append(new_headers, "x-fb-friendly-name: usePolarisFollowMutation"); 
    new_headers = curl_slist_append(new_headers, "x-root-field-name: xdt_create_friendship");
    std::string response;
    char* escaped_dtsg = curl_easy_escape(curl, dtsg.c_str(), static_cast<int>(dtsg.length()));
    std::string variable =
        "{\"target_user_id\":\"" + target_id + "\","
        "\"container_module\":\"profile\","
        "\"nav_chain\":\"PolarisProfilePostsTabRoot:profilePage:1:via_cold_start\"}";

    std::string escaped_variable = curl_easy_escape(curl, variable.c_str(), 0);

    std::string body =
        "av=" + av_id + "&"
        "fb_dtsg=" + std::string(escaped_dtsg) + "&"
        "fb_api_caller_class=RelayModern&"
        "fb_api_req_friendly_name=usePolarisFollowMutation&"
        "server_timestamps=true&"
        "variables=" + escaped_variable + "&"
        "doc_id=9740159112729312"; 

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.instagram.com/graphql/query");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, new_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(new_headers);
    curl_free(escaped_dtsg);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        return "";
    }
    try {
        json d = json::parse(response);
        if (d.contains("errors") && d["errors"].is_array() && !d["errors"].empty()) {
            std::string msg = d["errors"][0]["message"];
            std::cerr << "Instagram error: " << msg << std::endl;
            return "";
        }
        if (d.contains("data") && d["data"].contains("xdt_create_friendship")) {bool following = d["data"]["xdt_create_friendship"]["friendship_status"]["following"];
            return following ? "OK" : "False";
        }
        return d.dump();
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return "";
    }
}