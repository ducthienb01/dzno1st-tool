#ifndef Golike_head
#define Golike_head
#include <iostream>
#include <memory>
#include <vector>
#include "../header/json.hpp"
#include <curl/curl.h>
using json = nlohmann::json;
struct GolikeJob { 
    int id, coin;
    std::string name, link, objectID, Type, objectID2, objectID3, message;
};
class PlatformStrategy {
    public:
     virtual ~PlatformStrategy() = default;
     virtual std::vector<GolikeJob> ACCOUNT() const = 0;
     virtual std::vector<GolikeJob> GETJOB(long long account_id) const = 0;
     virtual std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const = 0;
     virtual std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const = 0;
};
class GolikeClient {
    private:
     std::unique_ptr<PlatformStrategy> strategy;
     struct curl_slist* headers = nullptr;
     mutable CURL* curl = NULL;

    public:
     explicit GolikeClient(const std::string& token);
     ~GolikeClient();
     GolikeJob ME() const;
     std::string performRequest(const std::string& url, const std::string& method ="GET" , const std::string& postdata = "") const;
     std::vector<GolikeJob> ACCOUNT() const {
        return strategy ->ACCOUNT();
    }
     std::vector<GolikeJob> GETJOB(long long account_id) const {
        return strategy->GETJOB(account_id);
    }
     std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id,const std::string& type="",const std::string& user_id="",const std::string& object_id="") const {
        return strategy ->COMPLE(ads_id,account_id,type,user_id,object_id);
    }
     std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id="", const int& account_id =0, const std::string& type="") const {
        return strategy ->SKIP(ads_id,object_id,account_id,type);
     }
     void setStrategy(std::unique_ptr<PlatformStrategy> s) {strategy = std::move(s);}
     PlatformStrategy* getStrategy() const { return strategy.get(); }
};  
class TIKTOK : public PlatformStrategy {
    private:
     const GolikeClient& client;
    public:
     explicit TIKTOK(const GolikeClient& c);
     ~TIKTOK() override = default;
     std::vector<GolikeJob> ACCOUNT() const override;
     std::vector<GolikeJob> GETJOB(long long account_id) const override;
     std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const override;
     std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const override;
     int FOLLOW(const std::string& devices) const;
     int LIKE(const std::string& devices) const;
    //  void AUTO(const std::string& devices,long long account_id) const;
};
class INSTAGRAM : public PlatformStrategy {
    private:
     const GolikeClient& client;
     mutable struct curl_slist* headers = NULL;
     mutable CURL* curl = NULL;
    public:
     explicit INSTAGRAM(const GolikeClient& c,const std::string& cookie);
     ~INSTAGRAM();
     std::pair<std::string, std::string> GET_DATA() const;
     std::vector<GolikeJob> ACCOUNT() const override;
     std::vector<GolikeJob> GETJOB(long long account_id) const override;
     std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const override;
     std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const override;
     std::string LIKE(const std::string& av_id, const std::string& dtsg, const std::string& media_id,const std::string& cookie, std::string type="like") const;
     std::string FOLLOW(const std::string& target_id, const std::string& av_id, const std::string& dtsg,const std::string& cookie, std::string type="like") const;
     std::string requestAPI(const std::string& url,const std::string& body)const;
};
class FACEBOOK : public PlatformStrategy {
    private:
     const GolikeClient& client;
     mutable struct curl_slist* headers = NULL;
    public:
     explicit FACEBOOK(const GolikeClient& c);
     ~FACEBOOK() override = default;
     std::tuple<std::string,std::string,std::string> GET_DATA(const std::string& cookies) const;
     std::vector<GolikeJob> ACCOUNT() const override;
     std::vector<GolikeJob> GETJOB(long long account_id) const override;
     std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id,const std::string& type,const std::string& user_id,const std::string& object_id) const override;
     std::vector<GolikeJob> SKIP( const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const override;
     std::pair<std::string,std::string> Feedback_ID(const std::string& url, const std::string& action) const;
     std::string LIKE(const std::string& url,const std::string& reaction,const std::string& cookie,const std::string& profile) const;
     std::string FOLLOW(const std::string& url,const std::string& cookie,const std::string& profile) const;
};
#endif
