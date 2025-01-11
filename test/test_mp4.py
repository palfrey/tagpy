from pathlib import Path
import tagpy
import tagpy.id3v2


def get_cover(f):
    tag = None
    if isinstance(f, tagpy.FileRef):
        tag = f.tag()
        f = f.file()
    covers = []
    if hasattr(tag, "covers"):
        covers = tag.covers
    elif hasattr(f, "ID3v2Tag"):
        covers = [
            a
            for a in f.ID3v2Tag().frameList()
            if isinstance(a, tagpy.id3v2.AttachedPictureFrame)
        ]
    if covers:
        cover = covers[0]
        fmt = tagpy.mp4.CoverArtFormats.Unknown
        if isinstance(cover, tagpy.mp4.CoverArt):
            return cover
        else:
            mime = cover.mimeType().lower().strip()
            if mime == "image/jpeg":
                fmt = tagpy.mp4.CoverArtFormats.JPEG
            elif mime == "image/png":
                fmt = tagpy.mp4.CoverArtFormats.PNG
            elif mime == "image/bmp":
                fmt = tagpy.mp4.CoverArtFormats.BMP
            elif mime == "image/gif":
                fmt = tagpy.mp4.CoverArtFormats.GIF
            return tagpy.mp4.CoverArt(fmt, cover.picture())


def test_mp4():
    current_folder = Path(__file__).parent
    f1 = tagpy.FileRef(current_folder.joinpath("Caldhu-with-cover-art.mp3").as_posix())
    f2 = tagpy.FileRef(current_folder.joinpath("Caldhu.mp4").as_posix())
    t1 = f1.tag()
    t2 = f2.tag()
    t2.title = t1.title
    t2.artist = t1.artist
    t2.album = t1.album
    t2.comment = t1.comment
    t2.genre = t1.genre
    t2.year = t1.year
    t2.track = t1.track
    c = tagpy.mp4.CoverArtList()
    c.append(get_cover(f1))
    t2.covers = c
    f2.save()
