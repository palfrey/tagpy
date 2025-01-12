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




#include <taglib/apetag.h>
#include <taglib/mpcfile.h>
#include <taglib/flacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/oggflacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/apefooter.h>
#include <taglib/apeitem.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/wavfile.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4coverart.h>
#include <taglib/mp4item.h>

#include "common.hpp"




namespace
{
  // -------------------------------------------------------------
  // FLAC
  // -------------------------------------------------------------
  MF_OL(ID3v1Tag, 0, 1);
  MF_OL(ID3v2Tag, 0, 1);
  MF_OL(xiphComment, 0, 1);

  // -------------------------------------------------------------
  // Ogg
  // -------------------------------------------------------------
  MF_OL(addField, 2, 3);
  #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,11,0)
  MF_OL(removeField, 1, 2);
  #else
  MF_OL(removeFields, 1, 2);
  #endif
  MF_OL(render, 0, 1);

  // -------------------------------------------------------------
  // APE
  // -------------------------------------------------------------
  MF_OL(addValue, 2, 3);

  // -------------------------------------------------------------
  // MPC
  // -------------------------------------------------------------
  #if (TAGLIB_MAJOR_VERSION == 1)
  MF_OL(remove, 0, 1);
  #endif
  //MF_OL(ID3v1Tag, 0, 1);
  MF_OL(APETag, 0, 1);

  // WAV
  MF_OL(strip, 0, 1);

  #if TAGLIB_HEX_VERSION >= CHECK_VERSION(1,10,0)
  // MP4
  TagLib::MP4::CoverArtList mp4_Tag_GetCovers(TagLib::MP4::Tag &t) {
    if (!t.contains("covr")) {
      return {};
    }
    return t.item("covr").toCoverArtList();
  }
  void mp4_Tag_SetCovers(TagLib::MP4::Tag &t, TagLib::MP4::CoverArtList& l) {
    return t.setItem("covr", l);
  }
  #endif
}

#if TAGLIB_HEX_VERSION >= CHECK_VERSION(1,11,0)
void addPictureWithOwnership(Ogg::XiphComment &cl, std::auto_ptr<TagLib::FLAC::Picture> picture) {
  cl.addPicture(picture.get());
  picture.release();
}
#endif

void exposeRest()
{
  // -------------------------------------------------------------
  // Ogg
  // -------------------------------------------------------------
  exposeMap<String, StringList>("ogg_FieldListMap");

  {
    typedef Ogg::XiphComment cl;
    class_<cl, bases<Tag>, boost::noncopyable>
      ("ogg_XiphComment", init<boost::python::optional<const ByteVector &> >())
      .DEF_SIMPLE_METHOD(fieldCount)
      .def("fieldListMap", &cl::fieldListMap,
           return_internal_reference<>())
      .DEF_SIMPLE_METHOD(vendorID)
      .DEF_OVERLOADED_METHOD(addField, void (cl::*)(const String &, const String &, bool))
      #if TAGLIB_HEX_VERSION < CHECK_VERSION(1,11,0)
      .DEF_OVERLOADED_METHOD(removeField, void (cl::*)(const String &, const String &))
      .DEF_OVERLOADED_METHOD(removeField, void (cl::*)(const String &, const String &))
      #else
      .DEF_OVERLOADED_METHOD(removeFields, void (cl::*)(const String &, const String &))
      .DEF_OVERLOADED_METHOD(removeFields, void (cl::*)(const String &, const String &))
      #endif
      .DEF_OVERLOADED_METHOD(render, ByteVector (cl::*)(bool) const)
      #if TAGLIB_HEX_VERSION >= CHECK_VERSION(1,11,0)
      .DEF_SIMPLE_METHOD(pictureList)
      .DEF_SIMPLE_METHOD(removeAllPictures)
      .def("removePicture", &cl::removePicture)
      .def("addPicture", addPictureWithOwnership);
      #endif
      ;
  }

  {
    typedef Ogg::File cl;
    class_<cl, bases<File>, boost::noncopyable>
      ("ogg_File", no_init)
      .DEF_SIMPLE_METHOD(packet)
      .DEF_SIMPLE_METHOD(setPacket)
      // MISSING: page headers
      ;
  }

  {
    typedef Ogg::FLAC::File cl;
    class_<cl, bases<Ogg::File>, boost::noncopyable>
      ("ogg_flac_File", init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      ;
  }

  {
    typedef Ogg::Vorbis::File cl;
    class_<cl, bases<Ogg::File>, boost::noncopyable>
      ("ogg_vorbis_File", init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      ;
  }

  // -------------------------------------------------------------
  // APE
  // -------------------------------------------------------------
  {
    typedef APE::Footer cl;
    class_<cl, boost::noncopyable>(
      "ape_Footer", init<boost::python::optional<const ByteVector &> >())
      .DEF_SIMPLE_METHOD(version)
      .DEF_SIMPLE_METHOD(headerPresent)
      .DEF_SIMPLE_METHOD(footerPresent)
      .DEF_SIMPLE_METHOD(isHeader)
      .DEF_SIMPLE_METHOD(setHeaderPresent)
      .DEF_SIMPLE_METHOD(itemCount)
      .DEF_SIMPLE_METHOD(setItemCount)
      .DEF_SIMPLE_METHOD(tagSize)
      .DEF_SIMPLE_METHOD(completeTagSize)
      .DEF_SIMPLE_METHOD(setTagSize)
      .DEF_SIMPLE_METHOD(setData)
      .DEF_SIMPLE_METHOD(renderFooter)
      .DEF_SIMPLE_METHOD(renderHeader)
      ;
  }

  {
    typedef APE::Item scope;
    enum_<scope::ItemTypes>("ape_ItemTypes")
      .ENUM_VALUE(Text)
      .ENUM_VALUE(Binary)
      .ENUM_VALUE(Locator)
      ;
  }

  {
    typedef APE::Item cl;
    class_<cl>("ape_Item")
      .def(init<const String &, const String &>())
      .def(init<const String &, const StringList &>())
      .def(init<const cl &>())
      .DEF_SIMPLE_METHOD(key)
      .DEF_SIMPLE_METHOD(binaryData)
      .DEF_SIMPLE_METHOD(size)
      .DEF_SIMPLE_METHOD(toString)
      .DEF_SIMPLE_METHOD(values)
      .DEF_SIMPLE_METHOD(render)
      .DEF_SIMPLE_METHOD(parse)
      .DEF_SIMPLE_METHOD(setReadOnly)
      .DEF_SIMPLE_METHOD(isReadOnly)
      .DEF_SIMPLE_METHOD(setType)
      .DEF_SIMPLE_METHOD(type)
      .DEF_SIMPLE_METHOD(isEmpty)
      ;
  }

  exposeMap<const String, APE::Item>("ape_ItemListMap");

  {
    typedef APE::Tag cl;
    class_<cl, bases<Tag>, boost::noncopyable>("ape_Tag")
      .def(init<File *, long>())
      .def("footer", &cl::footer, return_internal_reference<>())
      .def("itemListMap", &cl::itemListMap, return_internal_reference<>())
      .DEF_SIMPLE_METHOD(removeItem)
      .DEF_OVERLOADED_METHOD(addValue, void (cl::*)(const String &, const String &,bool))
      .DEF_SIMPLE_METHOD(setItem)
      ;
  }

  // -------------------------------------------------------------
  // FLAC
  // -------------------------------------------------------------
  enum_<TagLib::FLAC::Picture::Type>("flac_PictureType")
    .value("Other", TagLib::FLAC::Picture::Type::Other)
    .value("FileIcon", TagLib::FLAC::Picture::Type::FileIcon)
    .value("OtherFileIcon", TagLib::FLAC::Picture::Type::OtherFileIcon)
    .value("FrontCover", TagLib::FLAC::Picture::Type::FrontCover)
    .value("BackCover", TagLib::FLAC::Picture::Type::BackCover)
    .value("LeafletPage", TagLib::FLAC::Picture::Type::LeafletPage)
    .value("Media", TagLib::FLAC::Picture::Type::Media)
    .value("LeadArtist", TagLib::FLAC::Picture::Type::LeadArtist)
    .value("Artist", TagLib::FLAC::Picture::Type::Artist)
    .value("Conductor", TagLib::FLAC::Picture::Type::Conductor)
    .value("Band", TagLib::FLAC::Picture::Type::Band)
    .value("Composer", TagLib::FLAC::Picture::Type::Composer)
    .value("Lyricist", TagLib::FLAC::Picture::Type::Lyricist)
    .value("RecordingLocation", TagLib::FLAC::Picture::Type::RecordingLocation)
    .value("DuringRecording", TagLib::FLAC::Picture::Type::DuringRecording)
    .value("DuringPerformance", TagLib::FLAC::Picture::Type::DuringPerformance)
    .value("MovieScreenCapture", TagLib::FLAC::Picture::Type::MovieScreenCapture)
    .value("ColouredFish", TagLib::FLAC::Picture::Type::ColouredFish)
    .value("Illustration", TagLib::FLAC::Picture::Type::Illustration)
    .value("BandLogo", TagLib::FLAC::Picture::Type::BandLogo)
    .value("PublisherLogo", TagLib::FLAC::Picture::Type::PublisherLogo)
    ;

  {
    typedef TagLib::FLAC::Picture cl;
    class_<cl, std::auto_ptr<cl>, boost::noncopyable>
      ("flac_Picture", init<const ByteVector &>())
      .DEF_SIMPLE_METHOD(type)
      .DEF_SIMPLE_METHOD(data)
      .DEF_SIMPLE_METHOD(mimeType)
      .def("setType", &cl::setType)
      .def("setMimeType", &cl::setMimeType);
      ;

  }

  exposePointerList<TagLib::FLAC::Picture>("flac_PictureList");

  {
    typedef FLAC::File cl;
    class_<cl, boost::noncopyable, bases<File> >("flac_File",
                                   init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def(init<const char *, ID3v2::FrameFactory *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def("ID3v1Tag",
           (ID3v1::Tag *(FLAC::File::*)(bool))
           &FLAC::File::ID3v1Tag,
           ID3v1Tag_overloads()[return_internal_reference<>()])
      .def("ID3v2Tag",
           (ID3v2::Tag *(FLAC::File::*)(bool))
           &FLAC::File::ID3v2Tag,
           ID3v2Tag_overloads()[return_internal_reference<>()])
      .def("xiphComment",
           (Ogg::XiphComment *(FLAC::File::*)(bool))
           &FLAC::File::xiphComment,
           xiphComment_overloads()[return_internal_reference<>()])
      ;
  }

  // -------------------------------------------------------------
  // MPC
  // -------------------------------------------------------------
  enum_<MPC::File::TagTypes>("mpc_TagTypes")
    .value("NoTags", MPC::File::NoTags)
    .value("ID3v1", MPC::File::ID3v1)
    .value("ID3v2", MPC::File::ID3v2)
    .value("APE", MPC::File::APE)
    .value("AllTags", MPC::File::AllTags)
    ;

  {
    typedef MPC::File cl;
    class_<MPC::File, bases<File>, boost::noncopyable>
      ("mpc_File", init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def("ID3v1Tag",
           (ID3v1::Tag *(cl::*)(bool))
           &cl::ID3v1Tag,
           ID3v1Tag_overloads()[return_internal_reference<>()])
      .def("APETag",
           (APE::Tag *(cl::*)(bool))
           &cl::APETag,
           APETag_overloads()[return_internal_reference<>()])
      .def("remove",
           (void (cl::*)(int))
           &cl::strip,
           strip_overloads())
      ;
  }

  /// WAV

  enum_<TagLib::RIFF::WAV::File::TagTypes>("wav_TagTypes")
    .value("NoTags", TagLib::RIFF::WAV::File::NoTags)
    .value("ID3v2", TagLib::RIFF::WAV::File::ID3v2)
    .value("Info", TagLib::RIFF::WAV::File::Info)
    .value("AllTags", TagLib::RIFF::WAV::File::AllTags)
    ;

  {
    typedef TagLib::RIFF::WAV::File cl;
    class_<cl, bases<File>, boost::noncopyable>
      ("wav_File", init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def("ID3v2Tag",
           (ID3v2::Tag *(TagLib::RIFF::WAV::File::*)())
           &cl::ID3v2Tag,
           return_internal_reference<>())
      .def("InfoTag",
           (TagLib::RIFF::Info::Tag *(TagLib::RIFF::WAV::File::*)())
           &cl::InfoTag,
           return_internal_reference<>())
      #if (TAGPY_TAGLIB_HEX_VERSION >= 0x11100)
      .DEF_OVERLOADED_METHOD(strip, void (cl::*)(TagLib::RIFF::WAV::File::TagTypes) const)
      #endif
      ;
  }

    /// MP4
  #if TAGLIB_HEX_VERSION >= CHECK_VERSION(1,13,0)
  enum_<TagLib::MP4::File::TagTypes>("mp4_TagTypes")
    .value("NoTags", TagLib::MP4::File::NoTags)
    .value("MP4", TagLib::MP4::File::MP4)
    .value("AllTags", TagLib::MP4::File::AllTags)
    ;
  #endif
  {
    typedef TagLib::MP4::File cl;
    class_<cl, bases<File>, boost::noncopyable>
      ("mp4_File", init<const char *, boost::python::optional<bool, AudioProperties::ReadStyle> >())
      .def("tag",
           (MP4::Tag *(cl::*)() const)
           &cl::tag,
           return_internal_reference<>())
      ;
  }
  enum_<TagLib::MP4::CoverArt::Format>("mp4_CoverArtFormats")
    .value("JPEG", TagLib::MP4::CoverArt::JPEG)
    .value("PNG", TagLib::MP4::CoverArt::PNG)
    .value("BMP", TagLib::MP4::CoverArt::BMP)
    .value("GIF", TagLib::MP4::CoverArt::GIF)
    .value("Unknown", TagLib::MP4::CoverArt::Unknown)
    ;
  {
    typedef TagLib::MP4::CoverArt cl;
    class_<cl>
      ("mp4_CoverArt", init<TagLib::MP4::CoverArt::Format, const ByteVector &>())
      .DEF_SIMPLE_METHOD(format)
      .DEF_SIMPLE_METHOD(data)
      ;
  }
  exposeList<TagLib::MP4::CoverArt>("mp4_CoverArtList");
  {
    typedef TagLib::MP4::Tag cl;
    class_<TagWrap<cl>, boost::noncopyable>("Tag", no_init)
      .add_property("title", &cl::title, &cl::setTitle)
      .add_property("artist", &cl::artist, &cl::setArtist)
      .add_property("album", &cl::album, &cl::setAlbum)
      .add_property("comment", &cl::comment, &cl::setComment)
      .add_property("genre", &cl::genre, &cl::setGenre)
      .add_property("year", &cl::year, &cl::setYear)
      .add_property("track", &cl::track, &cl::setTrack)
      #if TAGLIB_HEX_VERSION >= CHECK_VERSION(1,10,0)
      .add_property("covers", &mp4_Tag_GetCovers, &mp4_Tag_SetCovers)
      #endif
      .DEF_VIRTUAL_METHOD(isEmpty)
      .DEF_SIMPLE_METHOD(duplicate)
      .staticmethod("duplicate")
      ;
  }

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
