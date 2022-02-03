# Copyright (c) 2006-2008 Andreas Kloeckner
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


import _tagpy

FrameFactory = _tagpy.id3v2_FrameFactory
Frame = _tagpy.id3v2_Frame
FrameListMap = _tagpy.id3v2_FrameListMap
FrameList = _tagpy.id3v2_FrameList

Header = _tagpy.id3v2_Header
ExtendedHeader = _tagpy.id3v2_ExtendedHeader
Footer = _tagpy.id3v2_Footer

Tag = _tagpy.id3v2_Tag

AttachedPictureFrameType = _tagpy.id3v2_AttachedPictureFrame_Type
AttachedPictureFrame = _tagpy.id3v2_AttachedPictureFrame
CommentsFrame = _tagpy.id3v2_CommentsFrame
PeakVolume = _tagpy.id3v2_PeakVolume
ChannelType = _tagpy.id3v2_RelativeVolumeFrame_ChannelType
RelativeVolumeFrame = _tagpy.id3v2_RelativeVolumeFrame
TextIdentificationFrame = _tagpy.id3v2_TextIdentificationFrame
UserTextIdentificationFrame = _tagpy.id3v2_UserTextIdentificationFrame
UniqueFileIdentifierFrame = _tagpy.id3v2_UniqueFileIdentifierFrame
UnkownFrame = _tagpy.id3v2_UnknownFrame
UnknownFrame = _tagpy.id3v2_UnknownFrame
UnsynchronizedLyricsFrame = _tagpy.id3v2_UnsynchronizedLyricsFrame
