# Important

These pieces of information are mostly for my colleagues in MTA KOKI because most of the tools detailed here are infrastructure dependent. A working system is up and running on the machine right in front of the door when you enter the Microscopy Center office.

# Build Tools
This directory contains the batch scripts to make it easier to compile the project on Windows.

These have a bunch of prerequisites, hopefully, most of these will be eliminated in the future, but for now here is the list:

- google crashpad
    - [https://chromium.googlesource.com/crashpad/crashpad]()
    - to build follow the steps [https://chromium.googlesource.com/crashpad/crashpad/+/HEAD/doc/developing.md]()
- Qt and Qt Creator [https://www.qt.io/download]()
- WinPython [https://github.com/winpython/winpython/releases]()
    - this is the Python distribution that will be shipped along the application in the `setup.exe` file
    - so all the Python packages that are needed by the application should be preinstalled
    - we used a WinPython 64 Zero for the base and installed the packages listed in [module_reqs.txt](module_reqs.txt)
- git, obviously
- `vcredist_x64.exe`
    - it is shipped with Visual Studio
- test assets
    - data for the testing phase, can be found on the KOKI shared drive at `kkk/csoport_43/FodorBalint/test-assets`
- InnoSetup [http://www.jrsoftware.org/isinfo.php]()
    - the command line interface `iscc` should be in the `PATH` so it could be called systemwide

Look at or edit [setenv.bat](setenv.bat) to put the prerequisites to the right path.

## `setenv.bat`

- sets variables pointing to the path the prerequisites are sitting
- sets some additional variables describing the current version and mode we are building, the platform we are building on and the datetime
- this file is the first to be called in the other scripts (`build.bat`, `test.bat`, `deploy.bat`)

## `build.bat`

- builds the project and puts the output to the `build` directory

## `test.bat`

- calls the previously built `test.exe`
- uses the test assets

## `deploy.bat`

- collects the necessary dll files
- packs into a single `setup.exe` executable

# Continuous Integration

We use Jenkins for this task. The build steps are the simply executing `build.bat`, `test.bat` and `deploy.bat` sequentially. Slack Notification Plugin is used to send messages about the build. Publish Over CIFS Plugin is used to ship the baked `setup.exe` to the shared drive to directory `kkk/csoport_43/FodorBalint/a3dc-builds/`