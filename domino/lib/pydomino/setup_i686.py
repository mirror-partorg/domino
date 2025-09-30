from setuptools import setup, Extension

import os

files = open("var/files.lst").read().split("\n")
#files = ["var/"+f for f in  os.listdir("var") if f[-2:] == ".c"]
#files = files + ["src/"+f for f in  os.listdir("src") if f[-2:] == ".c"]
#files = files + [f for f in  os.listdir(".") if f[-2:] == ".c"]
print(files)

setup(
    version='1.1.0',
    name='pydomino',
    ext_modules=[
        Extension(
            'pydomino',
            files,
            include_dirs = ["../../include","../../../../var/MSYS_NT-10.0-19045/i686"],
	    libraries=['dolib', 'domino'],
	    extra_compile_args=["-m32"],
            library_dirs=['../../../../var/MSYS_NT-10.0-19045/i686/.libs','.'],
            define_macros=[("BTI_WIN_32",None),("DOMINO78",None),("_WIN32",None)]
        )
    ],
)
