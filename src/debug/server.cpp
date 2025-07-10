#include <iostream>
#include "../config.hpp"

int main(int argc, char *argv[]) {
    std::cout << "MOD_LOBBY_ID == " MOD_LOBBY_ID "\n";
    std::cout << "MOD_VERSION == " MOD_VERSION "\n";
    std::cout << "DEDICATED_PORT == " << DEDICATED_PORT << '\n';
    std::cout << "Hosting debug server on 127.0.0.1:" << DEDICATED_PORT << '\n';
}