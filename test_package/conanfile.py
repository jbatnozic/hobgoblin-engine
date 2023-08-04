import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run
from conan.tools.files import copy
from os.path import join, sep


class HobgoblinTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def generate(self):
        for dep in self.dependencies.values():
            libdirs = dep.cpp_info.libdirs
            if (len(libdirs) > 0 and (sep in libdirs[0])):
                copy(self, "*.dll",   dep.cpp_info.libdirs[0], self.build_folder)
                copy(self, "*.dylib", dep.cpp_info.libdirs[0], self.build_folder)
                copy(self, "*.so",    dep.cpp_info.libdirs[0], self.build_folder)
            bindirs = dep.cpp_info.bindirs
            if (len(bindirs) > 0 and (sep in bindirs[0])):
                copy(self, "*.dll",   dep.cpp_info.bindirs[0], self.build_folder)
                copy(self, "*.dylib", dep.cpp_info.bindirs[0], self.build_folder)
                copy(self, "*.so",    dep.cpp_info.bindirs[0], self.build_folder)
            
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = join(self.cpp.build.bindir, "Hobgoblin.Test.Package")
            self.run(cmd, env="conanrun")
