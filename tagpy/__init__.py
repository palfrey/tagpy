# Copyright (c) 2006-2008 Andreas Kloeckner, Christoph Burgmer
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


from typing import cast
from _tagpy import File, ReadStyle, version as _version

version = cast(str, _version)


class FileTypeResolver(object):
    def createFile(
        self, fileName, readAudioProperties=True, audioPropertiesStyle=ReadStyle.Average
    ):
        raise NotImplementedError


class FileRef(object):
    fileTypeResolvers = []

    def __init__(
        self, f, readAudioProperties=True, audioPropertiesStyle=ReadStyle.Average
    ):
        if isinstance(f, FileRef):
            self._file = f._file
        elif isinstance(f, File):
            self._file = f
        else:
            self._file = FileRef.create(f, readAudioProperties, audioPropertiesStyle)

    def tag(self):
        return self._file.tag()

    def audioProperties(self):
        return self._file.audioProperties()

    def file(self):
        return self._file

    def save(self):
        return self._file.save()

    @classmethod
    def defaultFileExtensions(cls):
        return cls._getExtToModule.keys()

    def isNull(self):
        return not self._file or not self._file.isValid()

    @classmethod
    def addFileTypeResolver(cls, resolver):
        cls.fileTypeResolvers.insert(0, resolver)
        return resolver

    @staticmethod
    def _getExtToModule():
        import tagpy.ogg.vorbis
        import tagpy.ogg.flac
        import tagpy.ogg.opus
        import tagpy.mpeg
        import tagpy.mp4
        import tagpy.flac
        import tagpy.mpc
        import tagpy.wav

        # import tagpy.wavpack, tagpy.ogg.speex, tagpy.trueaudio

        return {
            "ogg": tagpy.ogg.vorbis,
            "mp3": tagpy.mpeg,
            "oga": tagpy.ogg.flac,
            "flac": tagpy.flac,
            "mpc": tagpy.mpc,
            "wav": tagpy.wav,
            "mp4": tagpy.mp4,
            "m4a": tagpy.mp4,
            "opus": tagpy.ogg.opus,
            # ".wv": tagpy.wavpack,
            # ".spx": tagpy.ogg.speex,
            # ".tta": tagpy.trueaudio,
        }

    @classmethod
    def create(
        cls, fileName, readAudioProperties=True, audioPropertiesStyle=ReadStyle.Average
    ):
        for resolver in cls.fileTypeResolvers:
            file = resolver.createFile(
                fileName, readAudioProperties, audioPropertiesStyle
            )
            if file:
                return file

        from os.path import exists

        if not exists(fileName):
            raise IOError("File does not exist")

        from os.path import splitext

        ext = splitext(fileName)[1][1:].lower()

        try:
            module = cls._getExtToModule()[ext]
        except KeyError:
            raise ValueError("unable to find file type")

        return module.File(fileName, readAudioProperties, audioPropertiesStyle)
