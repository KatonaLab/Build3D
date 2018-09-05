TEMPLATE = subdirs
SUBDIRS = app core test benchmark libics

core.subdir = src/core
test.subdir = src/test
benchmark.subdir = src/benchmark
libics.subdir = src/lib/libics
app.subdir = src/app

test.depends = core libics
benchmark.depends = core libics
app.depends = core libics