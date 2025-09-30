from distutils.core import setup, Extension

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
            include_dirs = ["../../include","../../../../var/MINGW64_NT-10.0-19045/x86_64"],
	    libraries=['dolib', 'domino'],
            library_dirs=['../../../../var/MINGW64_NT-10.0-19045/x86_64/.libs ','.'],
            define_macros=[("BTI_WIN_64",None),("DOMINO78",None)]
        )
    ],
)
