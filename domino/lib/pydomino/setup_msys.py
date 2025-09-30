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
            include_dirs = ["../../include","C:/var/work/var/MSYS_NT-10.0-17134/x86_64"],
	    libraries=['dolib', 'domino'],
            library_dirs=['C:/var/work/var/MSYS_NT-10.0-17134/x86_64/.libs','.'],
            define_macros=[("BTI_WIN_64",None),("DOMINO78",None),("_WIN32",None)]
        )
    ],
)
