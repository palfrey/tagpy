from pathlib import Path
import shutil
from tempfile import TemporaryDirectory
import tagpy
import tagpy.id3v2
from tagpy.ogg import flac


def test_flac_picture():
    fp = flac.Picture()

    fp.setType(flac.PictureType.FrontCover)
    assert fp.type() == flac.PictureType.FrontCover

    fp.setData(bytes.fromhex("deadbeef"))
    assert fp.data() == bytes.fromhex("deadbeef")

    fp.setMimeType("image/jpeg")
    assert fp.mimeType() == "image/jpeg"

    fp.setDescription("test")
    assert fp.description() == "test"

    fp.setWidth(100)
    assert fp.width() == 100

    fp.setHeight(100)
    assert fp.height() == 100

    fp.setColorDepth(24)
    assert fp.colorDepth() == 24

    fp.setNumColors(1234)
    assert fp.numColors() == 1234


def get_cover(file_ref: tagpy.FileRef) -> tagpy.ogg.flac.Picture:
    tag = file_ref.tag()
    file = file_ref.file()

    assert hasattr(file, "ID3v2Tag")

    covers = [
        a
        for a in file.ID3v2Tag().frameList()
        if isinstance(a, tagpy.id3v2.AttachedPictureFrame)
    ]

    assert len(covers) > 0

    cover = covers[0]

    picture = flac.Picture()
    picture.setData(cover.picture())
    assert len(picture.data()) > 0
    picture.setMimeType(cover.mimeType())
    return picture


def test_cover_and_tags():
    test_dir_path = Path(__file__).parent
    src_flac_file_path = test_dir_path / "la.flac"
    mp3_file_path = test_dir_path / "Caldhu-with-cover-art.mp3"

    with TemporaryDirectory() as temp_dir:

        dst_flac_file_path = Path(temp_dir) / "la.flac"

        shutil.copy(src_flac_file_path, dst_flac_file_path)

        mp3_file_ref = tagpy.FileRef(mp3_file_path.as_posix())
        flac_file_ref = tagpy.FileRef(dst_flac_file_path.as_posix())

        mp3_file_tag = mp3_file_ref.tag()
        flac_file_tag = flac_file_ref.tag()

        for t in [
            "title",
            "artist",
            "album",
            "comment",
            "genre",
            "year",
            "track",
        ]:
            setattr(flac_file_tag, t, getattr(mp3_file_tag, t))

        # Test cover
        cover = get_cover(mp3_file_ref)
        flac_file = flac_file_ref.file()

        # Test adding auto_ptr<FLAC::Picture>
        flac_file.addPicture(cover)

        flac_file_ref.save()


def test_cover_copy():
    test_dir_path = Path(__file__).parent
    src_flac_file_path = test_dir_path / "la.flac"
    src_cover_flac_file_path = test_dir_path / "la-with-cover-art.flac"

    with TemporaryDirectory() as temp_dir:
        dst_flac_file_path = Path(temp_dir) / "la.flac"

        shutil.copy(src_flac_file_path, dst_flac_file_path)

        src_file_ref = tagpy.FileRef(src_cover_flac_file_path.as_posix())
        dst_file_ref = tagpy.FileRef(dst_flac_file_path.as_posix())

        sf = src_file_ref.file()
        pl = sf.pictureList()
        assert pl.size() == 1

        df = dst_file_ref.file()

        # Test adding FLAC::Picture*
        df.addPicture(pl[0])

        # Test saving picture
        dst_file_ref.save()

        # Test removePictures
        df.removePictures()

        # Test removePicture
        df.addPicture(pl[0])
        df.removePicture(df.pictureList()[0], True)
