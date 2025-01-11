# former crash bug by Andreas Hemel <debian-bugs@daishan.de>

import shutil
import tagpy
import tagpy.id3v2
import pathlib

from tempfile import TemporaryDirectory


def test_deb_bug_438556():
    with TemporaryDirectory() as tempdir:
        tempfile = pathlib.Path(tempdir).joinpath("la.mp3")
        shutil.copy(pathlib.Path(__file__).parent.joinpath("la.mp3"), tempfile)
        fileref = tagpy.FileRef(tempfile.as_posix())
        file = fileref.file()
        tag = file.ID3v2Tag(True)
        frame = tagpy.id3v2.UniqueFileIdentifierFrame("blah", "blah")
        tag.addFrame(frame)
        file.save()
