#include "Camera.h"

Camera::Camera(VulkanRenderer* renderer_) : renderer(renderer_) {
	if (renderer) {
		cameraUBO = renderer->CreateUniformBuffers<CameraData>();
	}
};

bool Camera::OnCreate(float aspectRatio_, DualQuat& orientPos_) {

	if (aspectRatio_ <= 0.0f) {
		return false;
	}

	orientationPosition = orientPos_;
	data.viewMatrix = getViewMatrix();

	perspective = Vec4(45.0f, aspectRatio_, 0.5f, 100.0f);
	RecalculateProjection();

	return true;
}

bool Camera::OnDestroy()
{
	if (renderer) {
		renderer->DestroyUBO(cameraUBO);
	}
	return true;
}

bool Camera::setPerspective(Vec4 perspective_)
{
	if (perspective_[0] == 0.0f)
	{
		return false;
	}

	perspective = perspective_;
	RecalculateProjection();
	return true;
}

void Camera::RecalculateProjection() {
	data.projectionMatrix = MMath::perspective(perspective[0], perspective[1], perspective[2], perspective[3]);
	data.projectionMatrix[5] *= -1.0f;

	if (renderer) {
		renderer->UpdateUniformBuffer<CameraData>(data, cameraUBO);
	}
}
