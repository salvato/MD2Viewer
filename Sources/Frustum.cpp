#include "Frustum.h"


Frustum::Frustum()
{}


Frustum::~Frustum()
{}


void
Frustum::Build(float FarPlane, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    float zMinimum, r;

// Calculate the minimum Z distance in the frustum.
    zMinimum = -projectionMatrix[3][2] / projectionMatrix[2][2];
    r = FarPlane / (FarPlane - zMinimum);
    projectionMatrix[2][2] = r;
    projectionMatrix[3][2] = -r * zMinimum;
// Create the frustum matrix from the view matrix and updated projection matrix.
    glm::mat4 matrix = viewMatrix* projectionMatrix;
// Calculate near plane of frustum.
    m_planes[0].t.x = matrix[0][3] + matrix[0][2];
    m_planes[0].t.y = matrix[1][3] + matrix[1][2];
    m_planes[0].t.z = matrix[2][3] + matrix[2][2];
    m_planes[0].t.w = matrix[3][3] + matrix[3][2];
    m_planes[0].n = glm::normalize(m_planes[0].t);
// Calculate far plane of frustum.
    m_planes[1].t.x = matrix[0][3] - matrix[0][2];
    m_planes[1].t.y = matrix[1][3] - matrix[1][2];
    m_planes[1].t.z = matrix[2][3] - matrix[2][2];
    m_planes[1].t.w = matrix[3][3] - matrix[3][2];
    m_planes[1].n = glm::normalize(m_planes[1].t);
// Calculate left plane of frustum.
    m_planes[2].t.x = matrix[0][3] + matrix[0][0];
    m_planes[2].t.y = matrix[1][3] + matrix[1][0];
    m_planes[2].t.z = matrix[2][3] + matrix[2][0];
    m_planes[2].t.w = matrix[3][3] + matrix[3][0];
    m_planes[2].n = glm::normalize(m_planes[2].t);
// Calculate right plane of frustum.
    m_planes[3].t.x = matrix[0][3] - matrix[0][0];
    m_planes[3].t.y = matrix[1][3] - matrix[1][0];
    m_planes[3].t.z = matrix[2][3] - matrix[2][0];
    m_planes[3].t.w = matrix[3][3] - matrix[3][0];
    m_planes[3].n = glm::normalize(m_planes[3].t);
// Calculate top plane of frustum.
    m_planes[4].t.x = matrix[0][3] - matrix[0][1];
    m_planes[4].t.y = matrix[1][3] - matrix[1][1];
    m_planes[4].t.z = matrix[2][3] - matrix[2][1];
    m_planes[4].t.w = matrix[3][3] - matrix[3][1];
    m_planes[4].n = glm::normalize(m_planes[4].t);
// Calculate bottom plane of frustum.
    m_planes[5].t.x = matrix[0][3] + matrix[0][1];
    m_planes[5].t.y = matrix[1][3] + matrix[1][1];
    m_planes[5].t.z = matrix[2][3] + matrix[2][1];
    m_planes[5].t.w = matrix[3][3] +  matrix[3][1];
    m_planes[5].n = glm::normalize(m_planes[5].t);
    return;
}


bool
Frustum::TestSphere(glm::vec3 Pos, float radius) {
    int i;
    float distance;
// Check if the radius of the sphere is inside the view frustum,
// if any are out side return false and its unseen
    for(i=0; i<6; i++) {
        glm::vec3	negaplane = m_planes[i].n;
// these need to be vec3 so cast them here.
        glm::vec3 planeCentre= static_cast<glm::vec3>(m_planes[i].t);
        glm::vec3 planeNormal = static_cast<glm::vec3>(m_planes[i].n);
// position direction (negative the normal of the plane)
        glm::intersectRayPlane(Pos, -negaplane, planeCentre, planeNormal, distance);
        if(distance < -radius) {
            return false;
        }
    }
    return true;
}


/*
bool
RayToPlane(const glm::vec3& origin,
           const glm::vec3& dir,
           const glm::vec3& planeOrigin,
           const glm::vec3& planeNormal,
           glm::vec3& result)
{
    float denom = glm::dot(planeNormal, dir);
    if(abs(denom) > 0.0001f) {
        float t = glm::dot(planeNormal, planeOrigin - origin) / denom;
        if(t >= 0) {
            result = origin + dir * t;
            return true;
        }
    }
    return false;
}
*/
