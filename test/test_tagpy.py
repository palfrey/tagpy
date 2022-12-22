import pathlib
import pytest
import tagpy


def test_non_existing_fileref():
    with pytest.raises(IOError) as e:
        tagpy.FileRef("does_not_exist.ogg")
    assert e.value.args == ("File does not exist",)


def test_no_such_type():
    with pytest.raises(ValueError) as e:
        tagpy.FileRef(str(pathlib.Path(__file__).parent.joinpath("foo.bar")))
    assert e.value.args == ("unable to find file type",)


def test_resolver():
    class DemoFile:
        pass

    class DemoResolver(tagpy.FileTypeResolver):
        def createFile(self, *args, **kwargs):
            return DemoFile()

    try:
        tagpy.FileRef.addFileTypeResolver(DemoResolver)
        f = tagpy.FileRef(str(pathlib.Path(__file__).parent.joinpath("la.ogg")))
        assert isinstance(f._file, DemoFile)
    finally:
        tagpy.FileRef.fileTypeResolvers = []


def test_wav():
    tagpy.FileRef(str(pathlib.Path(__file__).parent.joinpath("Caldhu.wav")))
