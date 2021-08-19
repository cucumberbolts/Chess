#include <Chess/Engine.h>

#include <iostream>

int main() {
    Engine* engine = Engine::Create("stockfish_14_x64_avx2.exe");

    std::string message;
    engine->Receive(message);

    // Revceiving when no data is sent will halt the program (so don't do it)
    //engine->Receive(message);

    std::cout << "Engine sent: " << message << "\n";

    engine->Send("isready\n");
    engine->Receive(message);

    std::cout << "Engine sent: " << message << "\n";

    engine->Send("uci\n");
    engine->Receive(message);

    std::cout << "Engine sent: " << message << "\n";

    delete engine;

    std::cin.get();
}

