#include <unordered_set>
#include <string>
#include <random>

class Sessions {
private:
    static std::string session_ids_path;
    static std::unordered_set<std::string> sessions;
    static std::random_device rd;
    static std::mt19937 generator;
public:
    static void init(std::string p_session_ids_path);
    static std::string get_session_id();
};