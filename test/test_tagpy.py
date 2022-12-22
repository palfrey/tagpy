import pytest
import tagpy


def test_non_existing_fileref():
    with pytest.raises(IOError):
        tagpy.FileRef("does_not_exist.ogg")
