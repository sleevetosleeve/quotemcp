#include <string>

class Args {
public:
    static std::string session_ids_file;

    static void parse(int argc, char* argv[]);
    
};