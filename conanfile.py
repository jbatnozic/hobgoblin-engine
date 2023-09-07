from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import copy
from os.path import join, sep


class HobgoblinConan(ConanFile):
    name = "hobgoblin"
    version = "0.1.0"
    package_type = "library"

    # Optional metadata
    license = "<License TBD>"
    author = "Jovan Batnožić (jovanbatnozic@hotmail.rs)"
    url = "https://github.com/jbatnozic/Hobgoblin"
    description = "Simple game engine"
    topics = ("game", "engine", "multiplayer")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False], 
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False, 
        "fPIC": True
    }

    _modules = [
        "ChipmunkPhysics",
        "ColDetect",
        "Common",
        "Config",
        "Format",
        "Graphics",
        "Logging",
        "Math",
        "Preprocessor",
        "Private",
        "QAO",
        "RigelNet",
        "RmlUI",
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
            self.options.rm_safe("fPIC")

    def requirements(self):
        # Public
        self.requires("fmt/10.0.0", transitive_headers=True)
        self.requires("sfml/2.6.0", transitive_headers=True)
        self.requires("rmlui/4.4",  transitive_headers=True)

        # Private
        self.requires("glew/2.2.0")
        self.requires("gtest/1.10.0")
        self.requires("ztcpp/3.0.1@jbatnozic/stable")
        
        # Overrides (transitive dependencies)
        self.requires("freetype/2.11.1", override=True)

    def configure(self):
        # Check that we have at least C++17
        std = str(self.settings.compiler.cppstd)
        if std in ["98", "gnu98" "11", "gnu11", "14", "gnu14"]:
            raise Exception("Hobgoblin requires C++17 or newer standard.")
        
        # Configure options
        if self.options.shared:
            self.options.rm_safe("fPIC")

        self.options["sfml"].shared   = False
        self.options["sfml"].audio    = True
        self.options["sfml"].graphics = True
        self.options["sfml"].network  = True
        self.options["sfml"].window   = True

        self.options["ztcpp"].shared  = True

    def generate(self):
        # Import dynamic libraries
        hobgoblin_bindir = join(self.build_folder, "bin", str(self.settings.build_type))
        for dep in self.dependencies.values():
            bindirs = dep.cpp_info.bindirs
            # about the 'if' condition:
            # - check if bindirs is not undefined;
            # - some packages wrongly define it as a relative path, so to avoid recursively
            #   copying our own bindir, we check that it contains the OS's separator (if it
            #   has at least 1, then we know it's an absolute path).
            if (len(bindirs) > 0 and (sep in bindirs[0])):
                copy(self, "*.dylib", bindirs[0], hobgoblin_bindir)
                copy(self, "*.dll",   bindirs[0], hobgoblin_bindir)

        # Generate build system
        cmake_deps = CMakeDeps(self)
        cmake_deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # === PACKAGE CORE MODULE HEADERS ===
        for module in self._modules:
            copy(self, "*.h",   join(self.source_folder, "EngineCore/Modules/{}/Include".format(module)), join(self.package_folder, "include"))
            copy(self, "*.hpp", join(self.source_folder, "EngineCore/Modules/{}/Include".format(module)), join(self.package_folder, "include"))

        # === PACKAGE OVERLAY HEADERS ===
        for overlay in self._overlays:
            copy(self, "*.h",   join(self.source_folder, "Overlays/{}/Include".format(overlay)), join(self.package_folder, "include"))
            copy(self, "*.hpp", join(self.source_folder, "Overlays/{}/Include".format(overlay)), join(self.package_folder, "include"))

        # === PACKAGE CORE MODULE LIBRARIES ===
        copy(self, pattern="*Hobgoblin*.dll",   src=join(self.build_folder, "bin"), dst=join(self.package_folder, "bin"), keep_path=False)
        copy(self, pattern="*Hobgoblin*.lib",   src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)   
        copy(self, pattern="*Hobgoblin*.so",    src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)
        copy(self, pattern="*Hobgoblin*.dylib", src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)
        copy(self, pattern="*Hobgoblin*.a",     src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)

        # # === PACKAGE OVERLAY LIBRARIES ===
        for overlay in self._overlays:
            copy(self, "*{}*.dll".format(overlay),   src=join(self.build_folder, "bin"), dst=join(self.package_folder, "bin"), keep_path=False)
            copy(self, "*{}*.lib".format(overlay),   src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*{}*.so".format(overlay),    src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*{}*.dylib".format(overlay), src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*{}*.a".format(overlay),     src=join(self.build_folder, "lib"), dst=join(self.package_folder, "lib"), keep_path=False)

    def package_info(self):
        self.cpp_info.libdirs = ['lib']
        self.cpp_info.bindirs = ['bin']

        # Specifying libraries in reverse order (most dependent ones
        # first, most basicones last) prevents link errors on Linux.
        self.cpp_info.libs = [
            # Overlays
            "SPeMPE",

            # Core modules
            "Hobgoblin.ChipmunkPhysics", # TODO Should be optional
            "Hobgoblin.ColDetect",       # TODO Should be optional          
            "Hobgoblin.Graphics",        # TODO Should be optional         
            "Hobgoblin.QAO",             # TODO Should be optional
            "Hobgoblin.RigelNet",        # TODO Should be optional
            "Hobgoblin.RmlUi",           # TODO Should be optional
            "Hobgoblin.Config",          # TODO Should be optional

            "Hobgoblin.Utility",
            "Hobgoblin.Logging",
            "Hobgoblin.Common",
        ]

        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                "DbgHelp"
            ]
