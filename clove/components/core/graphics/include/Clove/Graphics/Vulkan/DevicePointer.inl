namespace clove {
    VkDevice DevicePointer::get() const {
        return logicalDevice;
    }

    VkPhysicalDevice DevicePointer::getPhysical() const {
        return physicalDevice;
    }

    VkSurfaceKHR DevicePointer::getSurface() const {
        return surface;
    }
}