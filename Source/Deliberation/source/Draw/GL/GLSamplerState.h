#pragma once

#include <glbinding/gl/types.h>

namespace deliberation
{

class GLStateManager;

class GLSamplerState final
{
public:
    GLSamplerState();
    ~GLSamplerState();

    gl::GLuint name() const;

    void setWrap(unsigned int index, gl::GLenum wrap);

private:
    gl::GLuint       m_name;
    gl::GLenum       m_wrap[3];
};

}
