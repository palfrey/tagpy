import pathlib
import tagpy
import tagpy.mpeg


def test_framedumper():
    f = tagpy.FileRef(str(pathlib.Path(__file__).parent.joinpath("la.mp3")))
    t = f._file.ID3v2Tag()

    for frame_type in list(t.frameListMap().keys()):
        print(frame_type)
        frame_list = t.frameListMap()[frame_type]
        for frame in frame_list:
            print("  %s" % frame.toString())
