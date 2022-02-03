import tagpy

f = tagpy.FileRef("la.ogg")
t = f.tag()

print(t.artist)
print(t.title)
print(t.album)
print(t.year)

t.artist = "Andreas"
t.title = "Laaa-ahh"
t.album = "Shake what'cha got"
t.year = 2006
f.save()
