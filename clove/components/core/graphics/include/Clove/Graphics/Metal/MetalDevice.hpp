#pragma once

#include "Clove/Graphics/GhaDevice.hpp"

#include <any>
#include <memory>

namespace clove {
	class MetalFactory;
	struct DeviceWrapper;
}

namespace clove {
	class MetalDevice : public GhaDevice {
		//VARIABLES
	private:
		std::unique_ptr<DeviceWrapper> wrapper{ nullptr };
        std::unique_ptr<MetalFactory> factory{ nullptr };

        //FUNCTIONS
	public:
		MetalDevice() = delete;
        MetalDevice(std::unique_ptr<DeviceWrapper> deviceWrapper, std::unique_ptr<MetalFactory> factory);

        MetalDevice(MetalDevice const &other) = delete;
		MetalDevice(MetalDevice &&other) noexcept;

		MetalDevice &operator=(MetalDevice const &other) = delete;
		MetalDevice &operator=(MetalDevice &&other) noexcept;

		~MetalDevice();
		
		GhaFactory *getGraphicsFactory() const override;

		void waitForIdleDevice() override;

        Info getInfo() const override;
		Limits getLimits() const override;
	};
}
