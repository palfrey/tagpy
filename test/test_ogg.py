from pathlib import Path
import shutil
from tempfile import TemporaryDirectory
import tagpy
import tagpy.id3v2
from tagpy.ogg import flac
from packaging.version import Version


def get_cover(f) -> tagpy.ogg.flac.Picture:
    tag = None
    if isinstance(f, tagpy.FileRef):
        tag = f.tag()
        f = f.file()
    covers = []
    if tag is not None and hasattr(tag, "covers"):
        covers = tag.covers
    elif hasattr(f, "ID3v2Tag"):
        covers = [
            a
            for a in f.ID3v2Tag().frameList()
            if isinstance(a, tagpy.id3v2.AttachedPictureFrame)
        ]
    if covers == []:
        raise Exception("No covers found")
    cover = covers[0]
    if isinstance(cover, flac.Picture):
        return cover
    else:
        mime = cover.mimeType().lower().strip()
        picture = flac.Picture(cover.picture())
        picture.setMimeType(mime)
        return picture


def test_cover_and_tags():
    with TemporaryDirectory() as tempdir:
        current_folder = Path(__file__).parent
        tempfile = Path(tempdir).joinpath("la.ogg")
        shutil.copy(current_folder.joinpath("la.ogg"), tempfile)
        f1 = tagpy.FileRef(
            current_folder.joinpath("Caldhu-with-cover-art.mp3").as_posix()
        )
        f2 = tagpy.FileRef(tempfile.as_posix())
        t1 = f1.tag()
        t2 = f2.tag()
        t2.title = t1.title
        t2.artist = t1.artist
        t2.album = t1.album
        t2.comment = t1.comment
        t2.genre = t1.genre
        t2.year = t1.year
        t2.track = t1.track
        if Version(tagpy.version) >= Version("1.11"):
            cover = get_cover(f1)
            t2.addPicture(cover)
        f2.save()
