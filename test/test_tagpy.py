import pytest
import tagpy

try:
    import faulthandler
except ImportError:
    pass
else:
    faulthandler.enable()


def test_non_existing_fileref():
    with pytest.raises(IOError):
        tagpy.FileRef("does_not_exist.ogg")


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        exec(sys.argv[1])
    else:
        from py.test.cmdline import main

        main([__file__])
