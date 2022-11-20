from conans import ConanFile, CMake, tools


class SPeMPECMakeGameConan(ConanFile):
    name = "spempe-cmake-game"
    version = "0.0.1"

    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of SpempeCmakeGame here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }
    generators = "cmake"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        return cmake

    def requirements(self):
        self.requires("hobgoblin/0.1.0@jbatnozic/stable")

    def build(self):
        cmake = _configure_cmake(self)
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def package(self):
        # Only needed if you're building a library and not a final product
        pass

    def package_info(self):
        # Only needed if you're building a library and not a final product
        pass
