#pragma once

#include "VulkanRenderer.h"
#include <DualQuat.h>
#include <MMath.h>

using namespace MATH;
using namespace MATHEX;

class Camera {
private:
    VulkanRenderer* renderer;

    DualQuat orientationPosition;
    Vec4 perspective;

    std::vector<BufferMemory> cameraUBO;
    CameraData data;

    void RecalculateProjection();

public:
   Camera(VulkanRenderer* renderer_);

    bool OnCreate(float aspectRatio_, DualQuat& orientPos_);
    bool OnDestroy();

    // Getters
    Matrix4 getViewMatrix() { return MMath::toMatrix4(orientationPosition); }
    Matrix4 getProjectionMatrix() { return data.projectionMatrix; }
    CameraData* getData() { return &data; }
    std::vector<BufferMemory> getCameraUBO() { return cameraUBO; }

    // Setters
    bool setPerspective(Vec4 perspective_);


};