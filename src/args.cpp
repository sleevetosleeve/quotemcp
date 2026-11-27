#include "args.hpp"
#include <iostream>

std::string Args::session_ids_file("");

void Args::parse(int argc, char* argv[]) {
    session_ids_file = "session_ids";

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "--session-ids-file") {
            if (argc <= i + 1) {
                std::cerr << "Error: Missing session ids file path argument";
            } else {
                session_ids_file = argv[++i];
            }
        } else {
            std::cerr << "Error: Unknown option \"" + arg + "\"";
        }
    }
}