#include "DebugContext.h"

#include <sstream>

#include <glad/glad.h>

static void APIENTRY GlErrorCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::ostringstream error;

    error << "OPENGL ERROR: \n";

    switch (source) {
        case GL_DEBUG_SOURCE_API:             error << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   error << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: error << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     error << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     error << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           error << "Source: Other"; break;
    }

    error << "\n";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               error << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: error << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  error << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         error << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         error << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              error << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          error << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           error << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               error << "Type: Other"; break;
    }

    error << "\n";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         error << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       error << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          error << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: error << "Severity: notification"; break;
    }

    error << "\n";

    error << message << "\n";

    throw std::runtime_error(error.str().c_str());
}

namespace DebugContext {

    void Init() {
        int32_t flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GlErrorCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }

}
