from pathlib import Path
import shutil
from tempfile import TemporaryDirectory
import tagpy
import tagpy.id3v2


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

        flac_file_ref.save()
