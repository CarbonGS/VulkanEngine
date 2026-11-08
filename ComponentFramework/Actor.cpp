#include "Actor.h"

Actor::Actor()
{}

void Actor::OnCreate(Vec3 pos_, Vec3 vel_, Quaternion orient_, DescriptorSetInfo descSetInfo_, PipelineInfo pipelineInfo_, Vec3 scale_)
{
	// Initialize physics base values
	pos = pos_;
	vel = vel_;
	orient = orient_;
	scale = scale_;

	// Set model data
	modelMatrix = MMath::translate(pos_) * MMath::toMatrix4(orient_) * MMath::scale(scale_.x, scale_.y, scale_.z);

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
	modelMatrix = MMath::translate(pos) * MMath::toMatrix4(orient) * MMath::scale(scale.x, scale.y, scale.z);
}

void Actor::Render(VulkanRenderer* renderer) const
{

	renderer->BindMesh(mesh);
	renderer->BindDescriptorSet(pipelineInfo.pipelineLayout, descSetInfo.descriptorSet);
	renderer->BindPipeline(pipelineInfo.pipeline);
	renderer->SetPushConstant(pipelineInfo, modelMatrix, highlighted);
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

void Actor::setHighlighState(const bool state)
{
	highlighted = state;
}

void Actor::rotate(const Quaternion& deltaOrient)
{
	orient = QMath::normalize(deltaOrient * orient);
}

void Actor::translate(const Vec3& delta)
{
	pos += delta;
}

const Quaternion Actor::GetOriention() const
{
	return orient;
}

const PipelineInfo Actor::GetPipeline() const
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

const IndexedVertexBuffer& Actor::GetMesh() const
{
	return mesh;
}
