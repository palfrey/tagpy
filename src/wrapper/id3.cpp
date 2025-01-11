// Copyright (c) 2006-2008 Andreas Kloeckner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.




#include <taglib/id3v1genres.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2extendedheader.h>
#include <taglib/id3v2footer.h>
#include <taglib/mpegfile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/commentsframe.h>
#include <taglib/relativevolumeframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/uniquefileidentifierframe.h>
#include <taglib/unknownframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/apetag.h>

#include "common.hpp"




namespace
{
  // -------------------------------------------------------------
  // ID3v2
  // -------------------------------------------------------------
  struct id3v2_FrameWrap : ID3v2::Frame, wrapper<ID3v2::Frame>
  {
      String toString() const { return this->get_override("toString")(); }
    protected: // maintain constructability
      id3v2_FrameWrap(const ByteVector &data) : ID3v2::Frame(data) { }
      // In docs, but not in code:
      /* id3v2_FrameWrap(ID3v2::Header *h) : ID3v2::Frame(h) { } */
  };

  void id3v2_Tag_addFrame(ID3v2::Tag &t, ID3v2::Frame *f)
  {
 #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
    ID3v2::Frame *f_clone = ID3v2::FrameFactory::instance()->createFrame(f->render());
 #else
    ID3v2::Frame *f_clone = ID3v2::FrameFactory::instance()->createFrame(f->render(), t.header());
 #endif
    t.addFrame(f_clone);
  }

  object id3v2_rvf_channels(const ID3v2::RelativeVolumeFrame &rvf)
  {
    List<ID3v2::RelativeVolumeFrame::ChannelType> l = rvf.channels();
    return make_list(l.begin(), l.end());
  }

  #define MF_OL(MF, MIN, MAX) \
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MF##_overloads, MF, MIN, MAX);

 #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
  MF_OL(createFrame, 1, 2);
 #else
  MF_OL(createFrame, 2, 2);
 #endif
  MF_OL(volumeAdjustmentIndex, 0, 1);
  MF_OL(volumeAdjustment, 0, 1);
  MF_OL(peakVolume, 0, 1);
  MF_OL(setVolumeAdjustmentIndex, 1, 2);
  MF_OL(setVolumeAdjustment, 1, 2);
  MF_OL(setPeakVolume, 1, 2);

  #if (TAGPY_TAGLIB_HEX_VERSION >= 0x10800)
    MF_OL(render, 0, 1)
  #endif

  // -------------------------------------------------------------
  // MPEG
  // -------------------------------------------------------------
  #if (TAGPY_TAGLIB_HEX_VERSION >= 0x10800)
    MF_OL(save, 0, 3)
  #else
    MF_OL(save, 0, 2)
  #endif
  MF_OL(ID3v1Tag, 0, 1)
  MF_OL(ID3v2Tag, 0, 1)
  MF_OL(APETag, 0, 1)
  MF_OL(strip, 0, 1)
}




void exposeID3()
{
  // -------------------------------------------------------------
  // ID3v1
  // -------------------------------------------------------------
  def("id3v1_genre", ID3v1::genre);

  class_<ID3v1::Tag, boost::noncopyable, bases<Tag> >
    ("id3v1_Tag")
    .def("render", &ID3v1::Tag::render)
    ;

  // -------------------------------------------------------------
  // ID3v2
  // -------------------------------------------------------------
  exposeMap<ByteVector, ID3v2::FrameList>("id3v2_FrameListMap");
  exposePointerList<ID3v2::Frame>("id3v2_FrameList");

  {
    typedef ID3v2::FrameFactory cl;

 #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
    ID3v2::Frame *(ID3v2::FrameFactory::*cf1)(const ByteVector &, bool) const
      = &cl::createFrame;
    ID3v2::Frame *(ID3v2::FrameFactory::*cf2)(const ByteVector &, TagLib::uint) const
 #else
    ID3v2::Frame *(ID3v2::FrameFactory::*cf)(const ByteVector &, const ID3v2::Header *) const
 #endif
      = &cl::createFrame;

    class_<ID3v2::FrameFactory, boost::noncopyable>
      ("id3v2_FrameFactory", no_init)
 #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
      .def("createFrame", cf1, return_value_policy<manage_new_object>())
      .def("createFrame", cf2, createFrame_overloads()[return_value_policy<manage_new_object>()])
 #else
      .def("createFrame", cf, createFrame_overloads()[return_value_policy<manage_new_object>()])
 #endif
      .def("instance", &cl::instance,
          return_value_policy<reference_existing_object>())
      .staticmethod("instance")

      .DEF_SIMPLE_METHOD(defaultTextEncoding)
      .DEF_SIMPLE_METHOD(setDefaultTextEncoding)
      ;
  }

  {
    typedef ID3v2::Frame cl;
    class_<id3v2_FrameWrap, boost::noncopyable>("id3v2_Frame", no_init)
      .DEF_SIMPLE_METHOD(frameID)
      .DEF_SIMPLE_METHOD(size)
      .DEF_SIMPLE_METHOD(setData)
      .DEF_SIMPLE_METHOD(setText)
      .def("toString", pure_virtual(&ID3v2::Frame::toString))
      .DEF_SIMPLE_METHOD(render)

      .def("headerSize",
           #if (TAGLIB_MAJOR_VERSION == 1)
           (TagLib::uint (*)())
           #else
           (unsigned int (TagLib::ID3v2::Frame::*)() const)
           #endif
           &ID3v2::Frame::headerSize)
      .def("headerSize",
           #if (TAGLIB_MAJOR_VERSION == 1)
           (TagLib::uint (*)())
           #else
           (unsigned int (TagLib::ID3v2::Frame::*)() const)
           #endif
           &ID3v2::Frame::headerSize)
      // MISSING: textDelimiter
      ;
  }

  {
    typedef ID3v2::Header cl;
    class_<cl, boost::noncopyable>
      ("id3v2_Header")
    // MISSING: second constructor
      .DEF_SIMPLE_METHOD(majorVersion)
      .DEF_SIMPLE_METHOD(revisionNumber)
      .DEF_SIMPLE_METHOD(extendedHeader)
      .DEF_SIMPLE_METHOD(experimentalIndicator)
      .DEF_SIMPLE_METHOD(footerPresent)
      .DEF_SIMPLE_METHOD(tagSize)
      .DEF_SIMPLE_METHOD(completeTagSize)
      .DEF_SIMPLE_METHOD(setTagSize)
      .DEF_SIMPLE_METHOD(setData)
      .DEF_SIMPLE_METHOD(render)
      .DEF_SIMPLE_METHOD(size)
      .staticmethod("size")
      .DEF_SIMPLE_METHOD(fileIdentifier)
      .staticmethod("fileIdentifier")
      ;
  }

  {
    typedef ID3v2::ExtendedHeader cl;
    class_<cl, boost::noncopyable>
      ("id3v2_ExtendedHeader", no_init)
      .DEF_SIMPLE_METHOD(size)
      .DEF_SIMPLE_METHOD(setData)
      ;
  }

  {
    typedef ID3v2::Footer cl;
    class_<cl, boost::noncopyable>
      ("id3v2_Footer", no_init)
      .DEF_SIMPLE_METHOD(render)
      .DEF_SIMPLE_METHOD(size)
      .staticmethod("size")
      ;
  }

  {
    typedef ID3v2::Tag cl;
    const ID3v2::FrameList &(cl::*fl1)(const ByteVector &) const =
      &cl::frameList;
    const ID3v2::FrameList &(cl::*fl2)() const =
      &cl::frameList;

    class_<cl, boost::noncopyable, bases<Tag> >("id3v2_Tag")
      .def("header", &ID3v2::Tag::header, return_internal_reference<>())
      .def("extendedHeader", &ID3v2::Tag::extendedHeader, return_internal_reference<>())
      #if (TAGLIB_MAJOR_VERSION == 1)
      .def("footer", &ID3v2::Tag::footer, return_internal_reference<>())
      #endif

      .def("frameListMap", &ID3v2::Tag::frameListMap, return_internal_reference<>())
      .def("frameList", fl1, return_internal_reference<>())
      .def("frameList", fl2, return_internal_reference<>())

      .def("addFrame", id3v2_Tag_addFrame)
      .DEF_SIMPLE_METHOD(removeFrame)
      .DEF_SIMPLE_METHOD(removeFrames)

      #if (TAGPY_TAGLIB_HEX_VERSION >= 0x10800)
        // Commented out following comment at:
        // https://github.com/inducer/tagpy/commit/fb6d9a95f8ed1b0f347a82569a13e60a75c7e6d6
        // .DEF_OVERLOADED_METHOD(render, ByteVector (cl::*)() const)
 #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
        .DEF_OVERLOADED_METHOD(render, ByteVector (cl::*)(int) const)
 #else
        .DEF_OVERLOADED_METHOD(render, ByteVector (cl::*)(ID3v2::Version) const)
 #endif
      #else
        .def("render", (ByteVector (cl::*)() const) &cl::render)
      #endif
      ;
  }


  // -------------------------------------------------------------
  // ID3v2 frame types
  // -------------------------------------------------------------
  {
    typedef TagLib::ID3v2::AttachedPictureFrame scope;
    enum_<ID3v2::AttachedPictureFrame::Type>("id3v2_AttachedPictureFrame_Type")
      .ENUM_VALUE(Other)
      .ENUM_VALUE(FileIcon)
      .ENUM_VALUE(OtherFileIcon)
      .ENUM_VALUE(FrontCover)
      .ENUM_VALUE(BackCover)
      .ENUM_VALUE(LeafletPage)
      .ENUM_VALUE(Media)
      .ENUM_VALUE(LeadArtist)
      .ENUM_VALUE(Artist)
      .ENUM_VALUE(Conductor)
      .ENUM_VALUE(Band)
      .ENUM_VALUE(Composer)
      .ENUM_VALUE(Lyricist)
      .ENUM_VALUE(RecordingLocation)
      .ENUM_VALUE(DuringRecording)
      .ENUM_VALUE(DuringPerformance)
      .ENUM_VALUE(MovieScreenCapture)
      .ENUM_VALUE(ColouredFish)
      .ENUM_VALUE(Illustration)
      .ENUM_VALUE(BandLogo)
      .ENUM_VALUE(PublisherLogo)
      ;
  }

  {
    typedef ID3v2::AttachedPictureFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_AttachedPictureFrame", init<boost::python::optional<const ByteVector &> >())
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      .DEF_SIMPLE_METHOD(mimeType)
      .DEF_SIMPLE_METHOD(setMimeType)
      .DEF_SIMPLE_METHOD(type)
      .DEF_SIMPLE_METHOD(setType)
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(picture)
      .DEF_SIMPLE_METHOD(setPicture)
      ;
  }

  {
    typedef ID3v2::CommentsFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_CommentsFrame", init<boost::python::optional<const ByteVector &> >())
      .def(init<String::Type>())
      .DEF_SIMPLE_METHOD(language)
      .DEF_SIMPLE_METHOD(setLanguage)
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      ;
  }

  {
    typedef ID3v2::RelativeVolumeFrame::PeakVolume cl;
    class_<cl>
      ("id3v2_PeakVolume")
      .def_readwrite("bitsRepresentingPeak", &cl::bitsRepresentingPeak)
      .def_readwrite("peakVolume", &cl::peakVolume)
      ;
  }

  {
    typedef TagLib::ID3v2::RelativeVolumeFrame scope;
    enum_<ID3v2::RelativeVolumeFrame::ChannelType>("id3v2_RelativeVolumeFrame_ChannelType")
      .ENUM_VALUE(Other)
      .ENUM_VALUE(MasterVolume)
      .ENUM_VALUE(FrontRight)
      .ENUM_VALUE(FrontLeft)
      .ENUM_VALUE(BackRight)
      .ENUM_VALUE(BackLeft)
      .ENUM_VALUE(FrontCentre)
      .ENUM_VALUE(BackCentre)
      .ENUM_VALUE(Subwoofer)
      ;
  }

  {
    typedef ID3v2::RelativeVolumeFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_RelativeVolumeFrame", init<const ByteVector &>())
      // MISSING: Empty constructor, gives symbol errors
      .def("channels", id3v2_rvf_channels)
      #if (TAGLIB_MAJOR_VERSION == 1)
      .DEF_SIMPLE_METHOD(setChannelType)
      #endif
      .DEF_OVERLOADED_METHOD(volumeAdjustmentIndex, short (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setVolumeAdjustmentIndex, void (cl::*)(short, cl::ChannelType))
      .DEF_OVERLOADED_METHOD(volumeAdjustment, float (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setVolumeAdjustment, void (cl::*)(float, cl::ChannelType))
      .DEF_OVERLOADED_METHOD(peakVolume, cl::PeakVolume (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setPeakVolume, void (cl::*)(const cl::PeakVolume &, cl::ChannelType))
      ;
  }

  {
    typedef ID3v2::TextIdentificationFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_TextIdentificationFrame", init<const ByteVector &, boost::python::optional<String::Type> >())
      .def("setText", (void (cl::*)(const String &)) &cl::setText)
      .def("setText", (void (cl::*)(const StringList &)) &cl::setText)
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      .DEF_SIMPLE_METHOD(fieldList)
      ;
  }

  {
    typedef ID3v2::UnsynchronizedLyricsFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_UnsynchronizedLyricsFrame", init<boost::python::optional<const ByteVector &> >())
      .def(init<String::Type>())
      .DEF_SIMPLE_METHOD(language)
      .DEF_SIMPLE_METHOD(setLanguage)
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      ;
  }

  {
    typedef ID3v2::UserTextIdentificationFrame cl;
    class_<cl, bases<ID3v2::TextIdentificationFrame>, boost::noncopyable>
      ("id3v2_UserTextIdentificationFrame", init<const ByteVector &>())
      .def(init<boost::python::optional<String::Type> >())
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(fieldList)
      ;
  }

  {
    typedef ID3v2::UniqueFileIdentifierFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_UniqueFileIdentifierFrame", init<const ByteVector &>())
      .def(init<const String &, const ByteVector &>())
      .DEF_SIMPLE_METHOD(owner)
      .DEF_SIMPLE_METHOD(setOwner)
      .DEF_SIMPLE_METHOD(identifier)
      .DEF_SIMPLE_METHOD(setIdentifier)
      ;
  }

  {
    typedef ID3v2::UnknownFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_UnknownFrame", init<const ByteVector &>())
      .DEF_SIMPLE_METHOD(data)
      ;
  }

  // -------------------------------------------------------------
  // MPEG
  // -------------------------------------------------------------
  enum_<MPEG::File::TagTypes>("mpeg_TagTypes")
    .value("NoTags", MPEG::File::NoTags)
    .value("ID3v1", MPEG::File::ID3v1)
    .value("ID3v2", MPEG::File::ID3v2)
    .value("APE", MPEG::File::APE)
    .value("AllTags", MPEG::File::AllTags)
    ;

  {
    typedef MPEG::Properties cl;
    class_<cl, bases<AudioProperties>, boost::noncopyable>
      ("mpeg_Properties",
       //init<File *, AudioProperties::ReadStyle>()
       no_init
       )
      .ADD_RO_PROPERTY(layer)
      // .ADD_RO_PROPERTY(protectionEnabled) (not implemented in TagLib 1.4)
      // .ADD_RO_PROPERTY(channelMode) (depends on ChannelMode type)
      .ADD_RO_PROPERTY(isCopyrighted)
      .ADD_RO_PROPERTY(isOriginal)
      ;
  }

  {
    typedef MPEG::File cl;
    class_<MPEG::File, bases<File>, boost::noncopyable>
      ("mpeg_File",
       init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def(init<const char *, ID3v2::FrameFactory *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def("save",
	   #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,12,0)
             (bool (MPEG::File::*)(int, bool, int))
	   #else
             (bool (MPEG::File::*)(int, TagLib::File::StripTags, TagLib::ID3v2::Version, TagLib::File::DuplicateTags))
	   #endif
           &cl::save,
           save_overloads())
      .def("ID3v1Tag",
           (ID3v1::Tag *(MPEG::File::*)(bool))
           &cl::ID3v1Tag,
           ID3v1Tag_overloads()[return_internal_reference<>()])
      .def("ID3v2Tag",
           (ID3v2::Tag *(MPEG::File::*)(bool))
           &cl::ID3v2Tag,
           ID3v2Tag_overloads()[return_internal_reference<>()])
      .def("APETag",
           (APE::Tag *(cl::*)(bool)) &cl::APETag,
           APETag_overloads()[return_internal_reference<>()])
      .def("strip",
           (bool (cl::*)(int)) &cl::strip,
           strip_overloads())
      #if (TAGLIB_MAJOR_VERSION == 1)
      .DEF_SIMPLE_METHOD(setID3v2FrameFactory)
      #endif
      .DEF_SIMPLE_METHOD(firstFrameOffset)
      .DEF_SIMPLE_METHOD(nextFrameOffset)
      .DEF_SIMPLE_METHOD(previousFrameOffset)
      .DEF_SIMPLE_METHOD(lastFrameOffset)
      ;
  }

  // MISSING: Header, XingHeader
}




// EMACS-FORMAT-TAG
//
// Local Variables:
// mode: C++
// eval: (c-set-style "stroustrup")
// eval: (c-set-offset 'access-label -2)
// eval: (c-set-offset 'inclass '++)
// c-basic-offset: 2
// tab-width: 8
// End:
