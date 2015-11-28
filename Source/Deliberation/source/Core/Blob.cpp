#include <Deliberation/Core/Blob.h>

namespace deliberation
{

Blob::Blob():
    m_impl(nullptr)
{
}

Blob::Blob(const Blob & blob):
    m_impl(blob.m_impl ? blob.m_impl->clone() : nullptr)
{
}

Blob::Blob(Blob && blob):
    m_impl(blob.m_impl ? blob.m_impl->clone() : nullptr)
{

}

std::size_t Blob::size() const
{
    return m_impl ? m_impl->size() : 0u;
}

const void * Blob::ptr() const
{
    return m_impl ? m_impl->ptr() : nullptr;
}

Blob & Blob::operator=(Blob && blob)
{
    m_impl = std::move(blob.m_impl);
    return *this;
}

}

