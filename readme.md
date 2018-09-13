# A3-DC
TODO: description

# Requirements

* OpenGL >=3.2
* TODO

# Getting Started
TODO: how to install the software

TODO: how to use it with a toy example

# Troubleshooting

	Fatal Python error: Py_Initialize: unable to load the file system codec
	ModuleNotFoundError: No module named 'encodings'
	
`PYTHONHOME` is not a path to a Python base directory. Use `--python-path` program argument to set to a correct one.

	LNK1181 linker error while compiling on Windows
	
Check your path to the project directory (e.g. `"c:\my projects\A3DC"`), if it contains spaces then probably that is the cause. Remove any spaces from the path. It seems to be an issue with jom.exe, we can not fix it right now.

# Development Infos

## Requirements
TODO
## Build
TODO
## Contribute
TODO

# License

See [LICENSE.txt](LICENSE.txt)

## Dependencies

* Qt
	* https://www.qt.io
	* http://doc.qt.io/qt-5/opensourcelicense.html
* pybind11
	* https://github.com/pybind/pybind11
	* https://github.com/pybind/pybind11/blob/stable/LICENSE
* libics
	* https://svi-opensource.github.io/libics/
	* https://svi-opensource.github.io/libics/#license
* catch2
	* https://github.com/catchorg/Catch2
	* https://github.com/catchorg/Catch2/blob/master/LICENSE.txt
* benchpress
	* https://github.com/cjgdev/benchpress
	* https://github.com/cjgdev/benchpress/blob/master/LICENSE
