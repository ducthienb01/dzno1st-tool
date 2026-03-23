#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include "../header/golikenew.h"
#include <windows.h>
#include <limits>
// ================= INPUT AUTH =================
void show_authorization_input(std::string& authorization) {
    while (true) {
        std::cout << "Nhập authorization: ";
        std::getline(std::cin, authorization);
        if (!authorization.empty() &&
            authorization.rfind("Bearer ", 0) == 0) {
            std::system("cls");
            break;
        }
        std::cout << "Authorization không hợp lệ!\n";
    }
}
// ================= SHOW INFO =================
std::string show_welcome_and_account_info(const GolikeClient& client) {
    auto me = client.ME();
    std::cout << std::string(20, ' ') << "Welcome\n";
    std::cout << std::string(30, '-') << "\n";
    std::cout << "Thông Tin Tài Khoản\n";
    std::cout << "ID   : " << me.id << "\n";
    std::cout << "Name : " << me.name << "\n";
    std::cout << "Coin : " << me.coin << "\n";
    std::cout << std::string(30, '-') << "\n";
    std::string choice;
    std::cout << "Loại Hình JOB:\n";
    std::cout << "1. Instagram\n";
    std::cout << "2. Tiktok\n";
    std::cout << "3. Facebook\n";
    std::cout << "Nhập lựa chọn: ";
    std::cin >> choice;
    std::cin.ignore();
    std::system("cls");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (choice == "1") return "instagram";
    if (choice == "2") return "tiktok";
    if (choice == "3") return "facebook";
    return "";
}
// ================= CHOOSE ACCOUNT =================
size_t choose_account_index(size_t max) {
    std::string input;
    size_t index = 0;
    while (true) {
        std::cout << "Nhập tài khoản chạy JOB Hoặc Dừng Chương Trình Gõ EXIT : " << std::flush;
        std::cin >> input;
        if (input == "exit"|| input == "Exit" || input == "EXIT") {
            std::exit(0);
        }
        try {
            index = std::stoul(input);
            if (index >= 1 && index <= max) {
                return index - 1;
            }
        } catch (...) {}
        std::cout << "Không hợp lệ, nhập lại!\n";
        continue;
    }
}
// // ================= INPUT COOKIE =================
std::string input_cookie(const std::string& name) {
    std::string cookie;
    while (true) {
        std::cout << "Nhập cookie cho tài khoản [" << name << "] Hoặc Dừng Chương Trình (Gõ EXIT): " << std::flush;
        std::getline(std::cin, cookie);
        if (cookie == "exit" || cookie == "Exit" || cookie == "EXIT") {
            std::exit(0);
        }
        if (!cookie.empty() && cookie.length() > 100) {
            std::system("cls");
            return cookie;
        }
        std::cout << "Cookie không hợp lệ!\n";
    }
}
// // ================= MAIN ACCOUNT FLOW =================
void handle_account(GolikeClient& g,int& account_id,std::string& account_name,std::string& cookie_out) {
    auto accounts = g.ACCOUNT();
    if (accounts.empty()) {
        std::cout << "Không có tài khoản!\n";
        return;
    }
    // show list
    for (size_t i = 0; i < accounts.size(); ++i) {
        std::cout << i + 1 << ". [" << accounts[i].id << "] - [" << accounts[i].link << "]\n";
    }
    std::cout << std::string(30, '-') << "\n";
    // chọn account
    size_t index = choose_account_index(accounts.size());
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    const auto& selected = accounts[index];

    account_id = selected.id;
    account_name = selected.link;

    std::system("cls");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // nhập cookie
    cookie_out = input_cookie(account_name);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
void AUTO_FB(const GolikeClient& client,FACEBOOK* fb,const int& account_id,const std::string& cookie,const std::string& user_id) {
    if (!fb) {
    std::cerr << "fb is nullptr\n";
    return;
    }
    std::vector<std::string> profile = {
        "ProfileTT",
        "ProfileTT1",
        "ProfileTT2"
    };
    while (true) {
        std::vector<std::thread> threads;

        auto job = client.GETJOB(account_id);

        if (job.empty()) {
            continue;
        }
        int i = 0;
        for (const auto& one_job : job) {
            std::string selected_profile = profile[i % profile.size()];
            threads.emplace_back([&, one_job, selected_profile,i]() {
                std::this_thread::sleep_for(std::chrono::seconds(2 * i));
                if (one_job.Type == "like" || one_job.Type == "like_page") {
                    auto like = fb->LIKE(one_job.link, one_job.Type, cookie, selected_profile);
                    if (like.empty() || like == "false" || like == "parse_error") {
                        auto skip = client.SKIP(one_job.id,"",account_id);
                        for (auto& mes : skip) {
                            std::cout << mes.message << '\n';
                        }
                        return;
                    }
                    auto success = client.COMPLE(one_job.id, account_id, one_job.Type, user_id, one_job.objectID);
                    if (!success.empty()) {
                        for (auto& mes : success) {
                            std::cout << mes.message << '\n';
                        }
                    }
                }else if (one_job.Type == "follow" || one_job.Type == "follow_corona_0") {
                    auto follow = fb->FOLLOW(one_job.link,cookie,selected_profile);
                    if (follow.empty() || follow == "false" || follow == "parse_error") {
                        auto skip = client.SKIP(one_job.id,"",account_id);
                        for (auto& mes : skip) {
                            std::cout << mes.message << '\n';
                        }
                        return;
                    }
                    auto success = client.COMPLE(one_job.id, account_id, one_job.Type, user_id, one_job.objectID);
                    if (!success.empty()) {
                        for (auto& mes : success) {
                            std::cout << mes.message << '\n';
                        }
                    }
                }
            });
            ++i;
        }
        for (auto& t : threads) {
            t.join();
        }
    }
}
