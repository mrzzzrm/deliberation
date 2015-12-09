#include "DrawExecution.h"

#include <iostream>

#include <glbinding/gl/boolean.h>
#include <glbinding/gl/functions.h>
#include <glbinding/Meta.h>

#include <Deliberation/Core/Assert.h>

#include <Deliberation/Draw/GL/GLStateManager.h>
#include <Deliberation/Draw/Buffer.h>
#include <Deliberation/Draw/Draw.h>
#include <Deliberation/Draw/Program.h>

#include "DrawImpl.h"
#include "ProgramImpl.h"
#include "TextureImpl.h"

namespace deliberation
{

namespace detail
{

DrawExecution::DrawExecution(GLStateManager & glStateManager,
                             const Draw & draw):
    m_glStateManager(glStateManager),
    m_drawImpl(*draw.m_impl)
{
}

void DrawExecution::perform()
{
    m_glStateManager.enableTextureCubeMapSeamless(true);

    applyDepthState();
    applyBlendState();
    applyCullState();
    applyRasterizerState();
    applyStencilState();

    gl::glUseProgram(m_drawImpl.program.m_impl->glProgramName);

    // Setup texture units
    for (auto b = 0u; b < m_drawImpl.textureBindings.size(); b++)
    {
        auto & binding = m_drawImpl.textureBindings[b];
        auto * texture = binding.texture;

        Assert(texture, "");

        gl::glActiveTexture(gl::GL_TEXTURE0 + b);
        gl::glBindTexture(texture->type(), texture->m_impl->glName);
        gl::glUniform1i(binding.location, b);
    }

    // Setup MRT
//    m_command.m_output.get().apply();

    // Set uniforms
    {
        /*
            TODO
                Port to GLStateManager
        */

        for (auto & uniform : m_drawImpl.uniforms)
        {
            Assert(uniform.isAssigned, "Uniform " + m_drawImpl.program.interface().uniformByLocation(uniform.location).name() + " not set");

            auto * data = uniform.blob.ptr();
            auto location = uniform.location;

            switch (uniform.type)
            {
            case gl::GL_INT:
                gl::glUniform1i(location, *((const gl::GLint*)data));
                break;
            case gl::GL_UNSIGNED_INT:
                gl::glUniform1ui(location, *((const gl::GLuint*)data));
                break;
            case gl::GL_INT_VEC2:
            {
                auto idata = (const gl::GLint*)data;
                gl::glUniform2i(location, idata[0], idata[1]);
                break;
            }
            case gl::GL_FLOAT:
                gl::glUniform1f(location, *((const gl::GLfloat*)data));
                break;
            case gl::GL_FLOAT_VEC2:
            {
                auto fdata = (const gl::GLfloat*)data;
                gl::glUniform2f(location, fdata[0], fdata[1]);
                break;
            }
            case gl::GL_FLOAT_VEC3:
            {
                auto fdata = (const gl::GLfloat*)data;
                gl::glUniform3f(location, fdata[0], fdata[1], fdata[2]);
                break;
            }
            case gl::GL_FLOAT_VEC4:
            {
                auto fdata = (const gl::GLfloat*)data;
                gl::glUniform4f(location, fdata[0], fdata[1], fdata[2], fdata[3]);
                break;
            }
            case gl::GL_FLOAT_MAT4:
                gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, (const gl::GLfloat*)data);
                break;
            default:
                Fail("Not implemented for type " + glbinding::Meta::getString(uniform.type));
            }
        }
    }

    // Dispatch draw
    if (m_drawImpl.indexBuffer)
    {
        if (!m_drawImpl.instanceBuffers.empty())
        {
            drawElementsInstanced();
        }
        else
        {
            drawElements();
        }
    }
    else
    {
        if (!m_drawImpl.instanceBuffers.empty())
        {
            drawArraysInstanced();
        }
        else
        {
            drawArrays();
        }
    }
}

/*
    TODO
        Use GLStateManager for VAO
*/

void DrawExecution::drawElementsInstanced() const
{
    gl::glBindVertexArray(m_drawImpl.glVertexArray);
    gl::glDrawElementsInstanced(m_drawImpl.state.primitive(),
                                elementCount(),
                                elementType(),
                                nullptr,
                                instanceCount());
}

void DrawExecution::drawElements() const
{
    gl::glBindVertexArray(m_drawImpl.glVertexArray);
    gl::glDrawElements(m_drawImpl.state.primitive(),
                       elementCount(),
                       elementType(),
                       nullptr);
}

void DrawExecution::drawArrays() const
{
    gl::glBindVertexArray(m_drawImpl.glVertexArray);
    gl::glDrawArrays(m_drawImpl.state.primitive(),
                     0u,
                     vertexCount());
}

void DrawExecution::drawArraysInstanced() const
{
    gl::glBindVertexArray(m_drawImpl.glVertexArray);
    gl::glDrawArraysInstanced(m_drawImpl.state.primitive(),
                                        0u,
                                        vertexCount(),
                                        instanceCount());
}

unsigned int DrawExecution::elementCount() const
{
    Assert(m_drawImpl.indexBuffer, "No index buffer set");
    Assert(m_drawImpl.indexBuffer->count() > 0, "Index buffer is empty");

    return m_drawImpl.indexBuffer->count();
}

unsigned int DrawExecution::vertexCount() const
{
    Assert(!m_drawImpl.vertexBuffers.empty(), "No vertex buffer bound to command");

    auto ref = m_drawImpl.vertexBuffers[0].ranged ?
               m_drawImpl.vertexBuffers[0].count :
               m_drawImpl.vertexBuffers[0].buffer.get().count();

    Assert(ref > 0, "Vertex buffer is empty");

    {
        for (auto b = 1u; b < m_drawImpl.vertexBuffers.size(); b++)
        {
            auto cmp = m_drawImpl.vertexBuffers[b].ranged ?
                       m_drawImpl.vertexBuffers[b].count :
                       m_drawImpl.vertexBuffers[b].buffer.get().count();
            Assert(cmp == ref, "");
        }
    }

    return ref;
}

unsigned int DrawExecution::instanceCount() const
{
    assert(!m_drawImpl.instanceBuffers.empty());

    auto & buffer = m_drawImpl.instanceBuffers[0].buffer.get();
    auto divisor = m_drawImpl.instanceBuffers[0].divisor;

    Assert(buffer.count() > 0, "Instance buffer is empty");
    Assert(divisor > 0, "Divisor of instance buffer is zero");

    auto ref = buffer.count() * divisor;

    for (auto b = 1u; b < m_drawImpl.instanceBuffers.size(); b++)
    {
        auto & buffer = m_drawImpl.instanceBuffers[b].buffer.get();
        auto divisor = m_drawImpl.instanceBuffers[b].divisor;

        auto test = buffer.count() * divisor;

        Assert(test == ref, "Differing instance count in buffer " + std::to_string(b) + ": " + std::to_string(test) + " != " + std::to_string(ref));
    }

    return ref;
}

gl::GLenum DrawExecution::elementType() const
{
    Assert(m_drawImpl.indexBuffer, "No index buffer set");
    Assert(m_drawImpl.indexBuffer->layout().fields().size() == 1u, "Invalid index buffer layout");

    return m_drawImpl.indexBuffer->layout().fields()[0].type();
}

void DrawExecution::applyDepthState()
{
    auto & state = m_drawImpl.state.depthState();

    Assert(!state.depthMask() || state.depthTest(), "Combination not implemented yet, see glDepthFunc");

    m_glStateManager.enableDepthTest(state.depthTest());
    m_glStateManager.setDepthMask(state.depthMask());
}

void DrawExecution::applyBlendState()
{
    auto & state = m_drawImpl.state.blendState();

    m_glStateManager.enableBlend(state.enabled());

    if (!state.enabled())
    {
        return;
    }

    m_glStateManager.setBlendEquation(state.equation());
    m_glStateManager.setBlendFunc(state.sfactor(), state.dfactor());
}

void DrawExecution::applyCullState()
{
    auto & state = m_drawImpl.state.cullState();

    m_glStateManager.enableCullFace(state.enabled());

    if (!state.enabled())
    {
        return;
    }

    m_glStateManager.setCullFace(state.mode());
}

void DrawExecution::applyRasterizerState()
{
    auto & state = m_drawImpl.state.rasterizerState();

    m_glStateManager.setPointSize(state.pointSize());
    m_glStateManager.setLineWidth(state.lineWidth());
}

void DrawExecution::applyStencilState()
{
    auto & state = m_drawImpl.state.stencilState();

    m_glStateManager.enableStencilTest(state.enabled());

    if (!state.enabled())
    {
        return;
    }

    auto & front = state.frontFace();
    auto & back = state.backFace();

    if (state.differentFaceFuncs())
    {
        m_glStateManager.setStencilFuncSeparate(gl::GL_FRONT, front.func, front.ref, front.readMask);
        m_glStateManager.setStencilFuncSeparate(gl::GL_BACK, back.func, back.ref, back.readMask);
    }
    else
    {
        m_glStateManager.setStencilFunc(state.func(), state.ref(), state.readMask());
    }

    if (state.differentFaceMasks())
    {
        m_glStateManager.setStencilMaskSeparate(gl::GL_FRONT, front.writeMask);
        m_glStateManager.setStencilMaskSeparate(gl::GL_BACK, back.writeMask);
    }
    else
    {
        m_glStateManager.setStencilMask(state.writeMask());
    }

    if (state.differentFaceOps())
    {
        m_glStateManager.setStencilOpSeparate(gl::GL_FRONT, front.sfail, front.dpfail, front.dppass);
        m_glStateManager.setStencilOpSeparate(gl::GL_BACK, back.sfail, back.dpfail, back.dppass);
    }
    else
    {
        m_glStateManager.setStencilOp(state.sfail(), state.dpfail(), state.dppass());
    }
}

}

}

