from setuptools import setup, Extension

import os

files = ["var/"+f for f in  os.listdir("var") if f[-2:] == ".c"]
files = files + ["src/"+f for f in  os.listdir("src") if f[-2:] == ".c"]
#files = files + [f for f in  os.listdir(".") if f[-2:] == ".c"]
print(files)

setup(
    version='1.1.0',
    name='pydomino',
    ext_modules=[
        Extension(
            'pydomino',
            files,
            include_dirs = ["../../include","../../../../var/MINGW64_NT-10.0-19045/x86_64","Z:/dev/msys64/usr/include/python3.12","Z:/dev/msys64/usr/include","Z:/dev/msys64/usr/lib/gcc/x86_64-pc-cygwin/15.1.0/include","Z:/dev/msys64/usr/include/w32api"],
	    libraries=['dolib', 'domino'],
            library_dirs=['../../../../var/MINGW64_NT-10.0-19045/x86_64/.libs ','.'],
            define_macros=[("BTI_WIN_64",None),("DOMINO78",None),("_WIN32",None)]
        )
    ],
)
