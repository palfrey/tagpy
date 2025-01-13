TagPy
=====
[![PyPI](https://img.shields.io/pypi/v/tagpy)](https://pypi.org/project/tagpy/)
[![Coverage Status](https://coveralls.io/repos/github/palfrey/tagpy/badge.svg)](https://coveralls.io/github/palfrey/tagpy)

TagPy is a a set of Python bindings for [Scott Wheeler's TagLib](https://taglib.org/). It builds upon [Boost.Python](http://www.boost.org/libs/python/doc/), a wrapper generation library which
is part of the [Boost set of C++ libraries](http://www.boost.org).

Just like TagLib, TagPy can:

- read and write ID3 tags of version 1 and 2, with many supported frame types
  for version 2 (in MPEG Layer 2 and MPEG Layer 3, FLAC and MPC),
- access Xiph Comments in Ogg Vorbis Files and Ogg Flac Files,
- access APE tags in Musepack and MP3 files.
- access ID3 version 2 tags in WAV files

All these have their own specific interfaces, but TagLib's generic tag
reading and writing mechanism is also supported.

You can find examples in the test/ directory.

Installing TagPy
================

If you're lucky (Python 3.9-3.13 on x86 Linux currently), you can probably just run `pip install tagpy` which will use the precompiled wheels. If this fails due to compilation
issues, you'll need to install some things first.

* Debian: `apt-get install libboost-python-dev libtag1-dev`
* Fedora: `dnf install boost-python3-devel taglib-devel`
* Alpine 3.17: `apk add taglib-dev boost1.80-python3` (or another `boost*-python3` for other alpine versions)
Other setups are not currently supported, but patches with CI checking for others are welcomed.

TagPy works with

- TagLib >=1.9 (all versions up to 2.0.2 currently tested)
- Boost.Python 1.74
- gcc 10.2.1

Slightly older versions of gcc and Boost.Python should be fine, but the 1.9 requirement for TagLib is firm. Anything newer is probably ok, and please file bugs for anything that fails.

Using TagPy
===========

Using TagPy is as simple as this:

    >>> import tagpy
    >>> f = tagpy.FileRef("la.mp3")
    >>> f.tag().artist
    u'Andreas'

The `test/` directory contains a few more examples.

In general, TagPy duplicates the TagLib API, with a few notable
exceptions:

- Namespaces (i.e. Python modules) are spelled in lower case.
  For example, `TagLib::Ogg::Vorbis` is now `taglib.ogg.vorbis`.

- Enumerations form their own scope and are not part of any
  enclosing class scope, if any.

  For example, the value `TagLib::String::UTF16BE` from the
  enum `TagLib::String::Type` is now `tagpy.StringType.UTF16BE`.

- `TagLib::String` objects are mapped to and expected as Python
  unicode objects.

- `TagLib::ByteVector` objects are mapped to regular Python
  string objects.
