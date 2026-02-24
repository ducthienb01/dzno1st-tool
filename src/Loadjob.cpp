#include <iostream>
#include "./header/loadjob.h"
#include "./header/golike.h"
#include "./header/instagram.h"
#include "./header/json.hpp"
#include <thread>
#include <chrono>
Loadjob::Loadjob(Golike& g, const std::string& instagram_Cookie) : golike(g), insta(instagram_Cookie) {};
void Loadjob::run(const std::string& platform , const long long& account_id) {
        while (true) {
            auto job = golike.GETJOB(platform, account_id);
            if (job.empty()) {
                std::this_thread::sleep_for(std::chrono::seconds(30 + (rand() % 30)));
                continue;
            }
            if (platform == "instagram") {
                auto data = insta.Getdata();
                for (auto ins : job) {
                    if (ins.Type == "like") {
                        std::this_thread::sleep_for(std::chrono::seconds(3 + (rand() % 15) ));
                        auto like = insta.Like(data.first, data.second, ins.objectID2.empty() ? ins.objectID : ins.objectID3);
                        if (like == "OK") {
                            std::this_thread::sleep_for(std::chrono::seconds(1 + (rand() % 15)));
                            auto out = golike.COMPLE(platform, ins.id, account_id);
                            for (auto out1 : out) {
                                auto now = std::time(nullptr);
                                std::cout << "[" << std::put_time(std::localtime(&now),"%H:%M:%S") << "]" << out1.message << ins.link << std::endl;
                            }
                        }
                        else {
                            std::this_thread::sleep_for(std::chrono::seconds(1 + (rand() % 15) ));
                            auto ski = golike.SKIP(platform, ins.id, ins.objectID.empty() ? ins.objectID2 : ins.objectID3, account_id, ins.Type);
                            for (auto skip : ski) {
                                auto now = std::time(nullptr);
                                std::cout << "[" << std::put_time(std::localtime(&now),"%H:%M:%S") << "]" << skip.message  << std::endl;
                            }
                        }
                    }
                    else if (ins.Type == "follow") {
                        std::this_thread::sleep_for(std::chrono::seconds(3 + (rand() % 15) ));
                        auto follow = insta.Follow(ins.objectID2.empty() ? ins.objectID : ins.objectID3, data.first, data.second);
                        if (follow == "OK") {
                            std::this_thread::sleep_for(std::chrono::seconds(1 + (rand() % 15) ));
                            auto out = golike.COMPLE(platform, ins.id, account_id);
                            for (auto out1 : out) {
                                auto now = std::time(nullptr);
                                std::cout << "[" << std::put_time(std::localtime(&now),"%H:%M:%S") << "]" << out1.message << ins.link << std::endl;
                            }
                        }
                        else {
                            std::this_thread::sleep_for(std::chrono::seconds(1 + (rand() % 15) ));
                            auto ski = golike.SKIP(platform, ins.id, ins.objectID.empty() ? ins.objectID2 : ins.objectID3, account_id, ins.Type);
                            for (auto skip : ski) {
                                auto now = std::time(nullptr);
                                std::cout << "[" << std::put_time(std::localtime(&now),"%H:%M:%S") << "]" << skip.message  << std::endl;
                            }
                        }
                    }
                    else {
                        std::this_thread::sleep_for(std::chrono::seconds(3 + (rand() % 15) ));
                        auto ski = golike.SKIP(platform, ins.id, ins.objectID.empty() ? ins.objectID2 : ins.objectID3, account_id, ins.Type);
                        for (auto skip : ski) {
                            std::cout << "Skip comment : " << skip.message << std::endl;
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(5 + (rand() % 15)));
        }
    }