from setuptools import setup
from Cython.Build import cythonize
from setuptools.command.build_ext import build_ext
import os

class BuildExt(build_ext):
    def run(self):
        super().run()

        # duyệt các file .pyd vừa build
        for root, dirs, files in os.walk("."):
            for f in files:
                if f.endswith(".pyd") and ".cp" in f:
                    short = f.split(".cp")[0] + ".pyd"
                    old_path = os.path.join(root, f)
                    new_path = os.path.join(root, short)

                    if not os.path.exists(new_path):
                        os.rename(old_path, new_path)

setup(
    ext_modules=cythonize("*.pyx", compiler_directives={"language_level": 3}),
    cmdclass={"build_ext": BuildExt},
)
#python setup.py build_ext --inplace