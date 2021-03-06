#pragma once
#include "vulkan/vulkan.h"
#include "memory/blockallocator.h"

namespace oi {

	namespace gc {

		struct GraphicsExt;

		struct GPUMemoryBlockExt {

			GraphicsExt *g;
			u32 memoryId;
			VkMemoryAllocateFlagBits memoryBits;
			VirtualBlockAllocator allocator;
			VkDeviceMemory memory;
			Buffer mappedMemory;
			bool isDedicated = false;

			bool compatible(const std::tuple<VkMemoryPropertyFlagBits, VkMemoryRequirements, VkMemoryDedicatedRequirementsKHR> &requirements) const;

			void free();
			bool free(BlockAllocation range);

		};

		struct GPUAllocationExt {

			GPUMemoryBlockExt *block;
			u32 offset;
			BlockAllocation allocation;
			Buffer mappedMemory;

			bool operator==(const GPUAllocationExt &other) const;

		};

	}

}