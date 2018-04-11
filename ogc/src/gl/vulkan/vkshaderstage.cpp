#include "graphics/shaderstage.h"
#include "graphics/gl/vulkan.h"
#include "graphics/graphics.h"
using namespace oi::gc;
using namespace oi;

ShaderStage::~ShaderStage() {
	VkShaderStage &shader = *(VkShaderStage*) platformData;
	VkGraphics &graphics = *(VkGraphics*) g->getPlatformData();

	if (shader.shader != nullptr)
		vkDestroyShaderModule(graphics.device, shader.shader, allocator);
}

bool ShaderStage::init(Graphics *g) {

	VkShaderStage &shader = *(VkShaderStage*)platformData;
	VkGraphics &graphics = *(VkGraphics*)g->getPlatformData();

	String fpath = path + "." + g->getShaderExtension();
	Buffer fbuf = Buffer::readFile(fpath);

	if (fbuf.size() == 0)
		return Log::throwError<ShaderStage, 0x0>(String("Couldn't find shader stage at path \"") + fpath + "\"");

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fbuf.size();
	createInfo.pCode = (uint32_t*) fbuf.addr();

	vkCheck<0x1, ShaderStage>(vkCreateShaderModule(graphics.device, &createInfo, allocator, &shader.shader), "Shader stage creation failed");

	VkPipelineShaderStageCreateInfo &stageInfo = shader.pipeline;
	memset(&stageInfo, 0, sizeof(stageInfo));
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = VkShaderStageType(type.getName()).getValue();
	stageInfo.module = shader.shader;
	stageInfo.pName = "main";

	fbuf.deconstruct();
	return true;
}