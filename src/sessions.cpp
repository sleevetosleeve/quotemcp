#include "sessions.hpp"
#include <bits/stdc++.h>
#include <string>

std::string Sessions::session_ids_path;
std::unordered_set<std::string> Sessions::sessions;
std::random_device Sessions::rd;
std::mt19937 Sessions::generator(rd());

const std::string WHITESPACE = " \t\n\r\f\v";

std::string rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void Sessions::init(std::string p_session_ids_path) {
    session_ids_path = p_session_ids_path;
    std::ifstream sessions_file(session_ids_path);
    std::string id;
    while (std::getline(sessions_file, id)) {
        sessions.insert(rtrim(id));
    }
}

std::string Sessions::get_session_id() {
        // Define the list of possible characters
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    // Create a distribution to uniformly select from all
    // characters
    std::uniform_int_distribution<> distribution(
        0, CHARACTERS.size() - 1);

    // Generate the random string
    std::string id;
    do {
        id = "";
        for (int i = 0; i < 32; ++i) {
            id += CHARACTERS[distribution(generator)];
        }
    } while (sessions.find(id) != sessions.end());
    std::ofstream sessions_file(session_ids_path, std::ios_base::app | std::ios_base::out);

    sessions_file << id << "\n";    
    sessions.insert(id);   
    return id;
}