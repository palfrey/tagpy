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
  MF_OL(removeField, 1, 2);
  MF_OL(render, 0, 1);

  // -------------------------------------------------------------
  // APE
  // -------------------------------------------------------------
  MF_OL(addValue, 2, 3);

  // -------------------------------------------------------------
  // MPC
  // -------------------------------------------------------------
  MF_OL(remove, 0, 1);
  //MF_OL(ID3v1Tag, 0, 1);
  MF_OL(APETag, 0, 1);

  // WAV
  MF_OL(strip, 0, 1);
}





void exposeRest()
{
  // -------------------------------------------------------------
  // Ogg
  // -------------------------------------------------------------
  exposeMap<String, StringList>("ogg_FieldListMap");

  {
    typedef Ogg::XiphComment cl;
    class_<cl, bases<Tag>, boost::noncopyable>
      ("ogg_XiphComment", init<optional<const ByteVector &> >())
      .DEF_SIMPLE_METHOD(fieldCount)
      .def("fieldListMap", &cl::fieldListMap,
           return_internal_reference<>())
      .DEF_SIMPLE_METHOD(vendorID)
      .DEF_OVERLOADED_METHOD(addField, void (cl::*)(const String &, const String &, bool))
      .DEF_OVERLOADED_METHOD(removeField, void (cl::*)(const String &, const String &))
      .DEF_OVERLOADED_METHOD(removeField, void (cl::*)(const String &, const String &))
      .DEF_OVERLOADED_METHOD(render, ByteVector (cl::*)(bool) const)
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
      ("ogg_flac_File", init<const char *, optional<bool, AudioProperties::ReadStyle> >())
      ;
  }

  {
    typedef Ogg::Vorbis::File cl;
    class_<cl, bases<Ogg::File>, boost::noncopyable>
      ("ogg_vorbis_File", init<const char *, optional<bool, AudioProperties::ReadStyle> >())
      ;
  }

  // -------------------------------------------------------------
  // APE
  // -------------------------------------------------------------
  {
    typedef APE::Footer cl;
    class_<cl, boost::noncopyable>(
      "ape_Footer", init<optional<const ByteVector &> >())
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
      .DEF_SIMPLE_METHOD(value)
      .DEF_SIMPLE_METHOD(size)
      .DEF_SIMPLE_METHOD(toString)
      .DEF_SIMPLE_METHOD(toStringList)
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
  {
    typedef FLAC::File cl;
    class_<cl, boost::noncopyable, bases<File> >("flac_File",
                                   init<const char *, optional<bool, AudioProperties::ReadStyle> >())
      .def(init<const char *, ID3v2::FrameFactory *, optional<bool, AudioProperties::ReadStyle> >())
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
      .DEF_SIMPLE_METHOD(setID3v2FrameFactory)
      .DEF_SIMPLE_METHOD(streamInfoData)
      .DEF_SIMPLE_METHOD(streamLength)
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
      ("mpc_File", init<const char *, optional<bool, AudioProperties::ReadStyle> >())
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
           &cl::remove,
           remove_overloads())
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
      ("wav_File", init<const char *, optional<bool, AudioProperties::ReadStyle> >())
      .def("ID3v2Tag",
           (ID3v2::Tag *(TagLib::RIFF::WAV::File::*)())
           &cl::ID3v2Tag,
           return_internal_reference<>())
      .def("InfoTag",
           (TagLib::RIFF::Info::Tag *(TagLib::RIFF::WAV::File::*)())
           &cl::InfoTag,
           return_internal_reference<>())
      .DEF_OVERLOADED_METHOD(strip, void (cl::*)(TagLib::RIFF::WAV::File::TagTypes) const)
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
