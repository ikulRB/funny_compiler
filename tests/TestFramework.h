#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>

namespace Color {
    const char *RESET  = "\033[0m";
    const char *GREEN  = "\033[32m";
    const char *RED    = "\033[31m";
    const char *YELLOW = "\033[33m";
    const char *CYAN   = "\033[36m";
}

class TestRunner {
public:
    void test(const std::string &name, std::function<void()> fn) {
        total_++;
        
        std::string status = Color::GREEN + std::string("[ OK ]") + Color::RESET;
        std::string errMsg = "";

        try {
            fn();
            passed_++;
        } catch (const std::exception &e) {
            failed_++;
            status = Color::RED + std::string("[FAIL]") + Color::RESET;
            errMsg = std::string(" ") + Color::YELLOW + e.what() + Color::RESET;
        }

        // Ширина колонки для имени — 35 символов.
        // Если имя длиннее, оно займет больше места, и текст ошибки сдвинется вправо.
        std::cout << status << " " 
                  << std::left << std::setw(35) << name 
                  << errMsg << "\n";
    }

    int summary() const {
        std::cout << "\n"
                  << Color::CYAN << "===========================================\n" << Color::RESET
                  << "Total:  " << total_  << "\n"
                  << Color::GREEN << "Passed: " << passed_ << Color::RESET << "\n";
        if (failed_ > 0) {
            std::cout << Color::RED << "Failed: " << failed_ << Color::RESET << "\n";
        }
        std::cout << Color::CYAN << "===========================================\n" << Color::RESET;
        return failed_ == 0 ? 0 : 1;
    }

private:
    int total_  = 0;
    int passed_ = 0;
    int failed_ = 0;
};

inline void check(bool cond, const std::string &msg) {
    if (!cond) throw std::runtime_error(msg);
}

#endif // TEST_FRAMEWORK_H