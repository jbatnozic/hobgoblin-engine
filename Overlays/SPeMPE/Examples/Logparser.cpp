
#include <algorithm>
#include <regex>
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream file{"log.txt"};
    std::string line;
    const std::regex base_regex("StepAnnouncement: num = ([0-9]+), timestamp = ([0-9]+)us");
    std::smatch base_match;

    int prev_num = 999;
    long long prev_time = 165805794169;
    long long max_time_diff = 0;
    while (std::getline(file, line)) {
        if (std::regex_match(line, base_match, base_regex)) {
            if (base_match.size() > 1) {
                const auto num  = std::stoi(base_match[1]);
                const auto time = std::stoll(base_match[2]);

                if (num != (prev_num + 1)) {
                    std::cout << "break at num " << num << '\n';
                }
                prev_num = num;

                long long time_diff = time - prev_time;
                if (time_diff > max_time_diff) {
                    max_time_diff = time_diff;
                    std::cout << "large time diff at num " << num << '\n';
                    std::cout << "    " << time_diff << "us" << '\n';
                }
                prev_time = time;

                //std::cout << num << ' ' << time << '\n';
            }
        }
    }

}