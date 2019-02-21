#pragma once

// Include GLM
#undef countof
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/intersect.hpp"
#undef countof

typedef struct PLANE {
    glm::vec4 t; // extracted from the matrix
    glm::vec4 n; // normalised
} PLANE, *P_PLANE;


class Frustum
{
public:
    Frustum() ;
    ~Frustum();

public:
    void Build(float Farplane, glm::mat4 Projection, glm::mat4 view);
    bool TestSphere(glm::vec3, float);

private:
    PLANE m_planes[6];
};
