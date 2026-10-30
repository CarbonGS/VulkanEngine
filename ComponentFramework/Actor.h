#pragma once
#include <Matrix.h>
#include "CoreStructs.h"
#include "VulkanRenderer.h"
#include <Quaternion.h>

using namespace MATH;

class Actor
{
private:
	Vec3 pos, vel;
	Quaternion orient;

	Sampler2D texture;
	IndexedVertexBuffer mesh;
	Matrix4 modelMatrix;

	DescriptorSetInfo descSetInfo;
	PipelineInfo pipelineInfo;

public:

	Actor();

	void OnCreate(Vec3 pos_, Vec3 vel_, Quaternion orient_,
		DescriptorSetInfo descSetInfo_, PipelineInfo pipelineInfo_);

	void OnDestroy(VulkanRenderer* renderer);

	virtual void Update(const float dt);

	void Render(VulkanRenderer* renderer) const;

	bool createTexture(Sampler2D texture_);
	bool createMesh(IndexedVertexBuffer mesh_);

	// Setters
	void setModelMatrix(Matrix4 matrix_);

	// Getters
	Quaternion GetOriention() const;
	PipelineInfo GetPipeline();
	DescriptorSetInfo& GetDesc();
	Sampler2D& GetSampler();
	IndexedVertexBuffer& GetMesh();
};

