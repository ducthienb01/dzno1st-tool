#ifndef Golike_head
#define Golike_head
#include <iostream>
#include <memory>
#include <vector>
#include "../header/json.hpp"
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
     virtual std::vector<GolikeJob> COMPLE(const int& ads_id, const int& account_id) const = 0;
     virtual std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const = 0;
     
};
class GolikeClient {
    private:
     std::unique_ptr<PlatformStrategy> strategy;
     std::string authorization;
     struct curl_slist* headers = nullptr;
    protected:
     GolikeJob ME() const;
     std::string performRequest(const std::string& url, const std::string& method, const std::string& postdata = "") const;
    public:
     explicit GolikeClient(std::unique_ptr<PlatformStrategy> s,const std::string& token);
     ~GolikeClient();
     std::vector<GolikeJob> ACCOUNT() const {
        return strategy ->ACCOUNT();
    }
     std::vector<GolikeJob> GETJOB(long long account_id) const {
        return strategy->GETJOB(account_id);
    }
     std::vector<GolikeJob> COMPLE(const int& ads_id,const int& account_id) const {
        return strategy ->COMPLE(ads_id,account_id);
    }
     std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const {
        return strategy ->SKIP(ads_id,object_id,account_id,type);
     }
     PlatformStrategy* getStrategy() const { return strategy.get(); }
};
class TIKTOK : public PlatformStrategy {
    private:
     const GolikeClient& client;
    public:
     explicit TIKTOK(const GolikeClient& c);
     ~TIKTOK();
     std::vector<GolikeJob> ACCOUNT() const override;
     std::vector<GolikeJob> GETJOB(long long account_id) const override;
     std::vector<GolikeJob> COMPLE(const int& ads_id, const int& account_id) const override;
     std::vector<GolikeJob> SKIP(const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const override;
     int FOLLOW() const;
     int LIKE() const;
};
class INSTAGRAM : public PlatformStrategy {
    private:
     const GolikeClient& client;
    public:
     explicit INSTAGRAM(const GolikeClient& c);
     ~INSTAGRAM();
     std::vector<GolikeJob> ACCOUNT() const override;
     std::vector<GolikeJob> GETJOB(long long account_id) const override;
     std::vector<GolikeJob> COMPLE(const int& ads_id, const int& account_id) const override;
     std::vector<GolikeJob> SKIP( const int& ads_id, const std::string& object_id, const int& account_id, const std::string& type) const override;
};
#endif