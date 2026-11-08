#pragma once
#include <Matrix.h>
#include "CoreStructs.h"
#include "VulkanRenderer.h"
#include <Quaternion.h>

using namespace MATH;

class Actor
{
private:
	Vec3 pos, vel, scale;
	Quaternion orient;

	Sampler2D texture;
	IndexedVertexBuffer mesh;
	Matrix4 modelMatrix;

	DescriptorSetInfo descSetInfo;
	PipelineInfo pipelineInfo;

	bool highlighted = false;

public:

	Actor();

	void OnCreate(Vec3 pos_, Vec3 vel_, Quaternion orient_,
		DescriptorSetInfo descSetInfo_, PipelineInfo pipelineInfo_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f));

	void OnDestroy(VulkanRenderer* renderer);

	virtual void Update(const float dt);

	void Render(VulkanRenderer* renderer) const;

	bool createTexture(Sampler2D texture_);
	bool createMesh(IndexedVertexBuffer mesh_);

	// Setters
	void setModelMatrix(Matrix4 matrix_);
	void setHighlighState(const bool state);
	void rotate(const Quaternion& deltaOrient);
	void translate(const Vec3& delta);

	// Getters
	// Non modifiable (or at least shouldn't be)
	const Quaternion GetOriention() const;
	const PipelineInfo GetPipeline() const;
	const IndexedVertexBuffer& GetMesh() const;

	// Modifiable
	DescriptorSetInfo& GetDesc();
	Sampler2D& GetSampler();
};

