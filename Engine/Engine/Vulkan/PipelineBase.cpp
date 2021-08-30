#include "PipelineBase.h"

namespace Vulkan
{
	PipelineBase::PipelineBase(const std::shared_ptr<PipelineLayout>& pipeline_layout)
		: m_pipeline_layout(pipeline_layout)
	{

	}

	PipelineBase::~PipelineBase() {
		if (m_pipeline)
			vkDestroyPipeline(m_pipeline_layout->GetDevicePtr()->GetHandle(), m_pipeline, nullptr);
	}
}