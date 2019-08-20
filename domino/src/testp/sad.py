from distutils.core import setup, Extension

setup(name="first", version="1.0",
      ext_modules=[Extension("testp", ["pytestp.c"])])
      