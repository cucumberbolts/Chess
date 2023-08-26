#include "Platform/Unix/UnixEngine.h"

std::unique_ptr<Engine> Engine::Create(const std::filesystem::path &path) {
    return std::make_unique<UnixEngine>(path);
}

UnixEngine::UnixEngine(const std::string& path) {

}

UnixEngine::~UnixEngine() {

}

void UnixEngine::Send(const std::string& message) {

}

bool UnixEngine::Receive(std::string& message) {
    return false;
}
