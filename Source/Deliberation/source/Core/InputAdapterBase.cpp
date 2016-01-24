#include <Deliberation/Core/InputAdapterBase.h>

#include <iostream>

namespace deliberation
{

InputAdapterBase::InputAdapterBase() = default;

InputAdapterBase::~InputAdapterBase() = default;

bool InputAdapterBase::mouseButtonDown(unsigned int button) const
{
    if (button >= m_mouseButtons.size())
    {
        return false;
    }
    return m_mouseButtons[button];
}

bool InputAdapterBase::keyDown(unsigned int key) const
{
    if (key >= m_keys.size())
    {
        return false;
    }
    return m_keys[key];
}

const glm::vec2 & InputAdapterBase::mousePosition() const
{
    return m_mousePosition;
}

void InputAdapterBase::setMouseButtonDown(unsigned int button, bool down)
{
    static bool haveWarned = false;
    if (button > 4096 && !haveWarned)
    {
        std::cout << "Consider switching container, button index is " << button << std::endl;
        haveWarned = true;
    }

    if (button >= m_mouseButtons.size())
    {
        m_mouseButtons.resize(button + 1, false);
    }

    m_mouseButtons[button] = down;
}

void InputAdapterBase::setKeyDown(unsigned int key, bool down)
{
    static bool haveWarned = false;
    if (key > 4096 && !haveWarned)
    {
        std::cout << "Consider switching container, key index is " << key << std::endl;
        haveWarned = true;
    }

    if (key >= m_keys.size())
    {
        m_keys.resize(key + 1, false);
    }

    m_keys[key] = down;
}

void InputAdapterBase::setMousePosition(const glm::vec2 & position)
{
    m_mousePosition = position;
}

}

