# former crash bug by Andreas Hemel <debian-bugs@daishan.de>

import tagpy
import tagpy.id3v2
import pathlib


def test_deb_bug_438556():
    fileref = tagpy.FileRef(str(pathlib.Path(__file__).parent.joinpath("la.mp3")))
    file = fileref.file()
    tag = file.ID3v2Tag(True)
    frame = tagpy.id3v2.UniqueFileIdentifierFrame("blah", "blah")
    tag.addFrame(frame)
    file.save()
