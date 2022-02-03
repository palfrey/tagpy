import tagpy
import tagpy.mpeg

f = tagpy.mpeg.File("la.mp3")
t = f.ID3v2Tag()

for frame_type in list(t.frameListMap().keys()):
    print(frame_type)
    frame_list = t.frameListMap()[frame_type]
    for frame in frame_list:
        print("  %s" % frame.toString())
