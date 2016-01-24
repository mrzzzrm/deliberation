#include <Deliberation/Scene/DebugGrid3DRenderer.h>

#include <glbinding/gl/enum.h>

#include <Deliberation/Deliberation.h>

#include <Deliberation/Draw/Context.h>
#include <Deliberation/Draw/StencilState.h>

#include <Deliberation/Scene/Camera3D.h>

namespace deliberation
{

DebugGrid3DRenderer::DebugGrid3DRenderer(Context & context,
                                         float scale,
                                         const Camera3D & camera):
    m_context(context),
    m_camera(camera)
{
    init(scale);
}

void DebugGrid3DRenderer::draw()
{
    m_normalLines.uniform("viewProjection").set(m_camera.viewProjection());
    m_normalLines.schedule();

    m_fatLines.uniform("viewProjection").set(m_camera.viewProjection());
    m_fatLines.schedule();
}

void DebugGrid3DRenderer::init(float scale)
{
    auto extend = 100.0f;
    auto halfExtend = extend / 2.0f;

    std::vector<glm::vec3> vertices;

    for (auto x = scale; x < halfExtend; x += scale)
    {
        vertices.push_back({x, 0.0f,  halfExtend});
        vertices.push_back({x, 0.0f, -halfExtend});
        vertices.push_back({-x, 0.0f,  halfExtend});
        vertices.push_back({-x, 0.0f, -halfExtend});
    }

    for (auto z = scale; z < halfExtend; z += scale)
    {
        vertices.push_back({-halfExtend, 0.0f, z});
        vertices.push_back({ halfExtend, 0.0f, z});
        vertices.push_back({-halfExtend, 0.0f, -z});
        vertices.push_back({ halfExtend, 0.0f, -z});
    }

    vertices.push_back({-halfExtend, 0.0f, 0.0f});
    vertices.push_back({ halfExtend, 0.0f, 0.0f});
    vertices.push_back({0.0f, 0.0f,  halfExtend});
    vertices.push_back({0.0f, 0.0f, -halfExtend});

    auto layout = m_context.createPackedBufferLayout<glm::vec3>({"Position"});

    m_vertexBuffer = m_context.createBuffer(layout);
    m_vertexBuffer.createUpload(vertices).schedule();

    m_program = m_context.createProgram({deliberation::dataPath("Data/Shaders/GridRenderer.vert"),
                                         deliberation::dataPath("Data/Shaders/GridRenderer.frag")});

    m_normalLines = m_context.createDraw(m_program, gl::GL_LINES, "GridRenderer - normal lines");
    m_normalLines.addVertexBufferRange(m_vertexBuffer, 0u, vertices.size() - 4);
    m_normalLines.uniform("Color").set(glm::vec3(0.3f, 0.3f, 0.3f));
    //m_normalLines.state().setStencilState(StencilState::clearsBit(0));

    m_fatLines = m_context.createDraw(m_program, gl::GL_LINES, "GridRenderer - origin lines");
    m_fatLines.addVertexBufferRange(m_vertexBuffer, vertices.size() - 4, 4);
    m_fatLines.state().rasterizerState().setLineWidth(5.0f);
    m_fatLines.uniform("Color").set(glm::vec3(0.7f, 0.3f, 0.3f));
    //m_fatLines.state().setStencilState(StencilState::clearsBit(0));
}

}

