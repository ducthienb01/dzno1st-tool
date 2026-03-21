#ifndef Golike_h
#define Golike_h
#include <iostream>
#include <curl/curl.h>
#include <vector>
struct golikejob {
  int id, coin;
  std::string name, link, objectID, Type, objectID2, objectID3, message;
};
class Golike {
 private:
  std::string author;
  struct curl_slist* headers;

 public:
  explicit Golike(const std::string& authorization);
  ~Golike();
  golikejob ME() const ;
  std::vector<golikejob> Account(const std::string& platform);
  std::vector<golikejob> GETJOB(const std::string& platform , const long long& account_id);
  std::vector<golikejob> COMPLE(const std::string& platform , const int& ads_id , const int& account_id );
  std::vector<golikejob> SKIP(const std::string& platform , const int& ads_id ,const std::string& object_id, const int& account_id,const std::string& type);
};
#endif