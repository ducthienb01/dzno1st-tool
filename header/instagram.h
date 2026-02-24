#ifndef Instagram_h
#define Instagram_h
#include <iostream>
#include <vector>
#include <curl/curl.h>
struct User
{
  std::string status;
  bool final;
};

class instagram {
    private:
        std::string cookie;
        struct curl_slist* headers = NULL;
       public:
        instagram(const std::string& cookies);
        ~instagram();
        std::pair<std::string,std::string> Getdata();
        std::string Like(const std::string& av_id, const std::string& dtsg, const std::string& media_id);
        std::string Follow(const std::string& target_id, const std::string& av_id,const std::string& dtsg);
};
#endif