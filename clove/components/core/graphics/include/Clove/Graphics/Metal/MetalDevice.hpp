#pragma once

#include "Clove/Graphics/GhaDevice.hpp"

#include <any>
#include <memory>
#ifdef __OBJC__
#include <MetalKit/MetalKit.h>
#endif

namespace garlic::clove {
	class MetalFactory;
}

namespace garlic::clove {
	class MetalDevice : public GhaDevice {
		//VARIABLES
	private:
#ifdef __OBJC__
		id<MTLDevice> device{ nullptr };
		MTKView *view{ nullptr };
#endif
		
		std::shared_ptr<MetalFactory> factory;
		
		//FUNCTIONS
	public:
		MetalDevice() = delete;
		MetalDevice(std::any nativeWindow);
		
		MetalDevice(MetalDevice const &other) = delete;
		MetalDevice(MetalDevice &&other) noexcept;

		MetalDevice &operator=(MetalDevice const &other) = delete;
		MetalDevice &operator=(MetalDevice &&other) noexcept;

		~MetalDevice();
		
		std::shared_ptr<GhaFactory> getGraphicsFactory() const override;

		void waitForIdleDevice() override;

		Limits getLimits() const override;
	};
}
