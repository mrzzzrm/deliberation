#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <Deliberation/Deliberation_API.h>

namespace deliberation
{

template<typename T>
class DELIBERATION_API VoxelCluster final
{
public:
    static const T EMPTY_VOXEL;

public:
    VoxelCluster(const glm::uvec3 & size);

    const glm::vec3 & size() const;

    void set(const glm::uvec3 & voxel, const T & value);

    T & get(const glm::uvec3 & voxel);
    const T & get(const glm::uvec3 & voxel) const;

    bool test(const glm::uvec3 & voxel) const;

private:
    size_t voxelToIndex(const glm::uvec3 & voxel) const;

private:
    glm::vec3      m_size;
    std::vector<T> m_voxels;
};

}

#include <Deliberation/Voxel/VoxelCluster.inl>