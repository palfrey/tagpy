# former crash bug by Andreas Hemel <debian-bugs@daishan.de>

import tagpy
import tagpy.id3v2

fileref = tagpy.FileRef("la.mp3")
file = fileref.file()
tag = file.ID3v2Tag(True)
frame = tagpy.id3v2.UniqueFileIdentifierFrame("blah", "blah")
tag.addFrame(frame)
file.save()
