#include "UnixEngine.h"

std::unique_ptr<Engine> Engine::Create (const std::filesystem::path &path)
{
    return std::make_unique<UnixEngine> (path.string());
}

void UnixEngine::Send (const std::string &message)
{}

bool UnixEngine::Receive (std::string &message)
{}
