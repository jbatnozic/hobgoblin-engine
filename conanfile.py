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

    # Engine Core modules

    _modules_foundation = [
        "S00_PDef",
        "S01_Common",
        "S01_Format",
        "S01_GSL",
        "S01_Preprocessor",
        "S02_HGExcept",
        "S03_Logging",
    ]

    _modules_utilities = [
        "S00_Math",
        "S01_Utility",
    ]

    _modules_principals = [
        "S00_ChipmunkPhysics",
        "S00_HGConfig",
        "S00_Input",
        "S00_QAO",
        "S00_RigelNet",
        "S00_Window",
        "S01_ColDetect",
        "S01_Graphics",
        "S02_RmlUi",
    ]

    # Overlays

    _overlays = [
        "SPeMPE"
    ]

    # Sources

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
        self.requires("fmt/10.0.0",   transitive_headers=True)
        self.requires("ms-gsl/4.0.0", transitive_headers=True)
        self.requires("sfml/2.6.0"  , transitive_headers=True)
        self.requires("rmlui/4.4",    transitive_headers=True)
        self.requires("icu/74.1",     transitive_headers=True)

        # Private
        self.requires("glew/2.2.0")
        self.requires("gtest/1.10.0")
        self.requires("ztcpp/3.0.2@jbatnozic/stable")
        
        # Overrides (transitive dependencies)
        self.requires("freetype/2.11.1", override=True)

    def configure(self):
        # Check that we have at least C++20
        std = str(self.settings.compiler.cppstd)
        if std in ["98", "gnu98" "11", "gnu11", "14", "gnu14", "17", "gnu17"]:
            raise Exception("Hobgoblin requires C++20 or newer standard.")
        
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
        INCLUDE_FILE_TYPES      = ["*.h", "*.hpp", "*.inl"]
        INCLUDE_DESTINATION_DIR = join(self.package_folder, "include")

        # === PACKAGE CORE MODULE HEADERS ===
        for file_type in INCLUDE_FILE_TYPES:
            for module in self._modules_foundation:
                source_dir = join(self.source_folder, "EngineCore/Modules_L00_Foundation/{}/Include".format(module))
                copy(self, file_type, source_dir, INCLUDE_DESTINATION_DIR)
            
            for module in self._modules_utilities:
                source_dir = join(self.source_folder, "EngineCore/Modules_L01_Utilities/{}/Include".format(module))
                copy(self, file_type, source_dir, INCLUDE_DESTINATION_DIR)

            for module in self._modules_principals:
                source_dir = join(self.source_folder, "EngineCore/Modules_L02_Principals/{}/Include".format(module))
                copy(self, file_type, source_dir, INCLUDE_DESTINATION_DIR)

        # === PACKAGE OVERLAY HEADERS ===
        for file_type in INCLUDE_FILE_TYPES:
            for overlay in self._overlays:
                source_dir = join(self.source_folder, "Overlays/{}/Include".format(overlay))
                copy(self, file_type, source_dir, INCLUDE_DESTINATION_DIR)

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
        # first, most basic ones last) prevents link errors on Linux.
        self.cpp_info.libs = [
            # Overlays
            "SPeMPE",

            # Principals
            "Hobgoblin.RmlUi",
            "Hobgoblin.Graphics",
            "Hobgoblin.ColDetect",
            "Hobgoblin.Window",
            "Hobgoblin.RigelNet",
            "Hobgoblin.QAO",
            "Hobgoblin.Input",
            "Hobgoblin.HGConfig",
            "Hobgoblin.ChipmunkPhysics",
            
            # Utilities
            "Hobgoblin.Utility",
            # "Hobgoblin.Math",         -- Header-only
            # "Hobgoblin.Common",       -- Header-only
            
            # Foundation
            "Hobgoblin.Logging",
            "Hobgoblin.HGExcept",
            # "Hobgoblin.Preprocessor", -- Header-only
            # "Hobgoblin.GSL",          -- Header-only
            # "Hobgoblin.Format",       -- Header-only
            # "Hobgoblin.PDef",         -- Header-only
        ]

        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                "DbgHelp"
            ]
