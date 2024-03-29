import os

from conans import ConanFile

class CloveConan(ConanFile):
    settings = (
        "os",
        "os_build",
        "compiler",
        "build_type",
        "arch"
    )

    requires = (
        "assimp/5.0.1",
        "bullet3/3.07",
        "freetype/2.10.4",
        "glm/0.9.9.8",
        "gtest/1.10.0",
        "libsndfile/1.0.30",
        "openal/1.21.0",
        "shaderc/2021.1",
        "spdlog/1.8.2",
        "spirv-cross/cci.20211113",
        "yaml-cpp/0.6.3"
    )

    generators = (
        "cmake_find_package_multi"
    )

    default_options = {
        "assimp:shared": False,
        "bullet3:shared": False,
        "freetype:shared": False,
        "gtest:shared": False,
        "libsndfile:shared": False,
        "openal:shared": True,
        "shaderc:shared": True,
        "spdlog:shared": False,
        "spirv-cross:shared": False,
        "yaml-cpp:shared": False,
    }

    def imports(self):
        outputFolder = "./bin"
        if str(self.settings.os_build) != "Linux":
            dest = os.path.join(outputFolder, str(self.settings.build_type))

            self.copy("*.dll", src="bin", dst=dest)
            self.copy("*.dylib*", src="lib", dst=dest)
        else:
            self.copy("*.so*", src="lib", dst=outputFolder)
