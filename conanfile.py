from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from pathlib import Path


class BitcrackleRecipe(ConanFile):
    name = "bitcrackle"
    version = "0.1"
    package_type = "application"

    license = "MIT"
    author = "Maciek Jabłoński maciekjablonsky@gmail.com"

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "src/*", "cmake/*"
    generators = ("VirtualBuildEnv", "VirtualRunEnv")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires('fmt/11.1.4')
        self.requires('boost/1.87.0')
        self.requires('catch2/3.8.1')
        self.requires('mp-units/2.4.0')
        self.requires('gcem/1.18.0')
        # self.requires('qt/6.8.3')

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
        tc.generate()

        full_env = {}
        full_env.update(self.buildenv.vars(self))
        full_env.update(self.runenv.vars(self))

        env_path = Path(self.build_folder) / "build_and_run.ps1"
        self.output.info(f"Producing build_and_run.ps1 file at {env_path}.")
        with open(env_path, "w") as f:
            for k, v in full_env.items():
                # Quote values only when needed — simple and safe
                if " " in v:
                    v = f'"{v}"'
                f.write(f"{k}={v}\n")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
