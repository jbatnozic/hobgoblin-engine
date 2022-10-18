from conans import ConanFile, CMake, tools


class HobgoblinConan(ConanFile):
    name = "hobgoblin"
    version = "0.1.0"

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

    license = "<License TBD>"
    author = "Jovan Batnožić (jovanbatnozic@hotmail.rs)"
    url = "https://github.com/jbatnozic/Hobgoblin"
    description = "Simple game engine"
    topics = ("game", "engine", "multiplayer")

    _modules = [
        "ChipmunkPhysics",
        "ColDetect",
        "Common",
        "Format",
        "Graphics",
        "Logging",
        "Math",
        "Preprocessor",
        "Private",
        "QAO",
        "RigelNet",
        "Utility"
    ]

    _overlays = [
        "SPeMPE"
    ]

    exports_sources = [
        "CMakeLists.txt",
        "CMakeSettings.json",
        "CODEANALYSIS.ruleset",
        "EngineCore/*",
        "Overlays/*"
    ]

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def requirements(self):
        self.requires("fmt/8.1.1")
        self.requires("gtest/1.10.0")
        self.requires("sfml/2.5.1")
        self.requires("ztcpp/2.1.0@jbatnozic/stable")

    def configure(self):
        self.options["sfml"].shared   = False
        self.options["sfml"].audio    = True
        self.options["sfml"].graphics = True
        self.options["sfml"].network  = True
        self.options["sfml"].window   = True

        self.options["ztcpp"].shared  = True

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def package(self):
        # === PACKAGE CORE MODULE HEADERS ===
        for module in self._modules:
            self.copy("*.h",   dst="include", src="EngineCore/Modules/{}/Include".format(module))
            self.copy("*.hpp", dst="include", src="EngineCore/Modules/{}/Include".format(module))

        # === PACKAGE OVERLAY HEADERS ===
        for overlay in self._overlays:
            self.copy("*.h",   dst="include", src="Overlays/{}/Include".format(overlay))
            self.copy("*.hpp", dst="include", src="Overlays/{}/Include".format(overlay))

        # === PACKAGE CORE MODULE LIBRARIES ===
        self.copy("*Hobgoblin*.lib", dst="lib", keep_path=False)
        self.copy("*Hobgoblin*.dll", dst="bin", keep_path=False)
        self.copy("*Hobgoblin*.so", dst="lib", keep_path=False)
        self.copy("*Hobgoblin*.dylib", dst="lib", keep_path=False)
        self.copy("*Hobgoblin*.a", dst="lib", keep_path=False)

        # === PACKAGE OVERLAY LIBRARIES ===
        for overlay in self._overlays:
            self.copy("*{}*.lib".format(overlay), dst="lib", keep_path=False)
            self.copy("*{}*.dll".format(overlay), dst="bin", keep_path=False)
            self.copy("*{}*.so".format(overlay), dst="lib", keep_path=False)
            self.copy("*{}*.dylib".format(overlay), dst="lib", keep_path=False)
            self.copy("*{}*.a".format(overlay), dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = [
            # Core modules
            "Hobgoblin.ChipmunkPhysics", # TODO Should be optional
            "Hobgoblin.ColDetect",       # TODO Should be optional
            "Hobgoblin.Common",
            "Hobgoblin.Graphics",        # TODO Should be optional
            "Hobgoblin.Logging",
            "Hobgoblin.QAO",             # TODO Should be optional
            "Hobgoblin.RigelNet",        # TODO Should be optional
            "Hobgoblin.Utility",
            # Overlays
            "SPeMPE"
        ]
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                "DbgHelp"
            ]