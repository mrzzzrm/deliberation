#include "FramebufferImpl.h"

#include <glbinding/gl/enum.h>

#include <Deliberation/Core/Assert.h>

#include <Deliberation/Draw/GL/GLStateManager.h>

#include <Deliberation/Draw/Surface.h>

#include "Draw/GL/GLFramebuffer.h"

#include "Draw/Detail/TextureImpl.h"

namespace deliberation
{

namespace detail
{

std::shared_ptr<FramebufferImpl> FramebufferImpl::backbuffer(unsigned int width, unsigned int height)
{
    auto result = std::make_shared<FramebufferImpl>();
    result->m_isBackbuffer = true;
    result->m_resolutionDirty = false;
    result->m_width = width;
    result->m_height = height;

    return result;
}

FramebufferImpl::FramebufferImpl():
    m_isBackbuffer(true),
    m_glFramebufferDirty(false),
    m_resolutionDirty(true),
    m_width(0u),
    m_height(0u)
{

}

unsigned int FramebufferImpl::width() const
{
    if (m_resolutionDirty)
    {
        updateResolution();
    }
    return m_width;
}

unsigned int FramebufferImpl::height() const
{
    if (m_resolutionDirty)
    {
        updateResolution();
    }
    return m_height;
}

bool FramebufferImpl::isBackbuffer() const
{
    return m_isBackbuffer;
}

const Surface * FramebufferImpl::renderTarget(unsigned int index) const
{
    Assert(!m_isBackbuffer, "");

    if (index < m_renderTargets.size())
    {
        return m_renderTargets[index].engaged() ? &m_renderTargets[index].get() : nullptr;
    }
    else
    {
        return nullptr;
    }
}

const std::vector<Optional<Surface>> & FramebufferImpl::renderTargets() const
{
    Assert(!m_isBackbuffer, "");

    return m_renderTargets;
}

Surface * FramebufferImpl::depthTarget() const
{
    return m_depthTarget.ptr();
}

void FramebufferImpl::setRenderTarget(unsigned int index, Surface * surface)
{
    if (index >= m_renderTargets.size())
    {
        m_renderTargets.resize(index + 1);
    }

    if (m_renderTargets[index] != surface)
    {
        m_glFramebufferDirty = true;
    }

    if (surface)
    {
        m_renderTargets[index].reset(*surface);
    }
    else
    {
        m_renderTargets[index].disengage();
    }

    m_isBackbuffer = false;
    m_resolutionDirty = true;
}

void FramebufferImpl::setDepthTarget(Surface * surface)
{
    if (m_depthTarget == surface)
    {
        return;
    }

    if (surface)
    {
        m_depthTarget.reset(*surface);
    }
    else
    {
        m_depthTarget.disengage();
    }

    m_glFramebufferDirty = true;
    m_isBackbuffer = false;
    m_resolutionDirty = true;
}

void FramebufferImpl::bind(GLStateManager & glStateManager) const
{
    if (m_isBackbuffer)
    {
        glStateManager.bindFramebuffer(gl::GL_DRAW_FRAMEBUFFER, 0);
    }
    else
    {
        if (m_glFramebufferDirty)
        {
            updateFramebufferDesc();
            m_glFramebuffer = glStateManager.framebuffer(m_glFramebufferDesc.get());
            m_glFramebufferDirty = false;
        }

        m_glFramebuffer->bind();
    }
}

void FramebufferImpl::updateFramebufferDesc() const
{
    std::vector<GLFramebufferDesc::ColorAttachment> colorAttachments(m_renderTargets.size());

    Assert(m_renderTargets.size() > 0, "");

    for (auto rt = 0u; rt < m_renderTargets.size(); rt++)
    {
        if (m_renderTargets[rt].engaged())
        {
            auto glName = m_renderTargets[rt].get().m_texture->glName;

            if (glName == 0)
            {
                m_renderTargets[rt].get().m_texture->allocate();
                glName = m_renderTargets[rt].get().m_texture->glName;
            }

            colorAttachments[rt].glName = glName;

            Assert(m_renderTargets[rt].get().m_texture.get(), "");

            if (m_renderTargets[rt].get().m_texture->numFaces == 1)
            {
                colorAttachments[rt].target = gl::GL_TEXTURE_2D;
            }
            else
            {
                Assert(m_renderTargets[rt].get().m_texture->numFaces == 6, "");

                /*
                    TODO
                        This is duplicated in TextureUploadExecution!
                */
                static gl::GLenum targets[] =
                {
                    gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                    gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                    gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                    gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                    gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                    gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                };

                colorAttachments[rt].target = targets[m_renderTargets[rt].get().face()];
            }
        }
    }

    GLFramebufferDesc::DepthAttachment depthAttachment;
    gl::GLuint glName;

    if (m_depthTarget.engaged())
    {
        if (m_depthTarget.get().m_texture->glName == 0)
        {
            m_depthTarget.get().m_texture->allocate();
        }
        glName = m_depthTarget.get().m_texture->glName;
    }
    else
    {
        glName = 0;
    }

    depthAttachment.glName = glName;

    m_glFramebufferDesc.reset(GLFramebufferDesc(colorAttachments, depthAttachment));
}

void FramebufferImpl::updateResolution() const
{
    bool foundFirst = false;

    for (auto rt = 0u; rt < m_renderTargets.size(); rt++)
    {
        if (m_renderTargets[rt].engaged())
        {
            if (!foundFirst)
            {
                m_width = m_renderTargets[rt].get().width();
                m_height = m_renderTargets[rt].get().height();
                foundFirst = true;
            }

            Assert(m_width == m_renderTargets[rt].get().width(), "");
            Assert(m_height == m_renderTargets[rt].get().height(), "");
        }
    }

    Assert(foundFirst, "");

    m_resolutionDirty = false;
}

}

}

