#include "Actor.h"

Actor::Actor()
{}

void Actor::OnCreate(Vec3 pos_, Vec3 vel_, Quaternion orient_, DescriptorSetInfo descSetInfo_, PipelineInfo pipelineInfo_)
{
	// Initialize physics base values
	pos = pos_;
	vel = vel_;
	orient = orient_;

	// Set model data
	modelMatrix = MMath::translate(pos_) * MMath::toMatrix4(orient_);

	// Assign descriptor set
	descSetInfo = descSetInfo_;

	pipelineInfo = pipelineInfo_;
}

void Actor::OnDestroy(VulkanRenderer* renderer)
{
	renderer->DestroyPipeline(pipelineInfo);
	renderer->DestroyDescriptorSet(descSetInfo);
	renderer->DestroySampler2D(texture);
	renderer->DestroyIndexedMesh(mesh);
}

void Actor::Update(const float dt)
{
	modelMatrix = MMath::translate(pos) * MMath::toMatrix4(orient) * MMath::scale(1.0f, 1.0f, 1.0f);
}

void Actor::Render(VulkanRenderer* renderer) const
{
	renderer->BindMesh(mesh);
	renderer->BindDescriptorSet(pipelineInfo.pipelineLayout, descSetInfo.descriptorSet);
	renderer->BindPipeline(pipelineInfo.pipeline);
	renderer->SetPushConstant(pipelineInfo, modelMatrix);
	renderer->DrawIndexed(mesh);
}

bool Actor::createTexture(Sampler2D texture_)
{
	if (!&texture_)
	{
		return false;
	}

	texture = texture_;
	return true;
}

bool Actor::createMesh(IndexedVertexBuffer mesh_)
{
	if (!&mesh)
	{
		return false;
	}

	mesh = mesh_;
	return true;
}

void Actor::setModelMatrix(Matrix4 matrix_)
{
	modelMatrix = matrix_;
}

Quaternion Actor::GetOriention() const
{
	return orient;
}

PipelineInfo Actor::GetPipeline()
{
	return pipelineInfo;
}

DescriptorSetInfo& Actor::GetDesc()
{
	return descSetInfo;
}

Sampler2D& Actor::GetSampler()
{
	return texture;
}

IndexedVertexBuffer& Actor::GetMesh()
{
	return mesh;
}
