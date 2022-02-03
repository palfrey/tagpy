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




#include <taglib/tstring.h>
#include <taglib/tag.h>
#include <taglib/tag.h>
#include <taglib/tfile.h>
#include <taglib/audioproperties.h>
#include <taglib/tstringlist.h>
#include <taglib/fileref.h>
#include <taglib/tmap.h>
#include <taglib/tlist.h>

#include "common.hpp"




namespace
{
  struct tlstring_to_unicode
  {
      static PyObject *convert(String const& s)
        {
          const char *cstr = s.toCString(true);
          return PyUnicode_DecodeUTF8(cstr, strlen(cstr), "ignore");
        }
  };

  struct tlbytevector_to_string
  {
      static PyObject *convert(ByteVector const& s)
        {
          return PyUnicode_FromStringAndSize(s.data(), s.size());
        }
  };

  struct ByteVectorIntermediate
  {
      string str;
      ByteVectorIntermediate(const string &_str)
        : str(_str)
        { }
      operator ByteVector() const
        {
          return ByteVector(str.data(), str.size());
        }
  };




  // -------------------------------------------------------------
  // Basics
  // -------------------------------------------------------------
  struct TagWrap : Tag, wrapper<Tag>
  {
      String title() const { return this->get_override("title")(); }
      String artist() const { return this->get_override("artist")(); }
      String album() const { return this->get_override("album")(); }
      String comment() const { return this->get_override("comment")(); }
      String genre() const { return this->get_override("genre")(); }
      TagLib::uint year() const { return this->get_override("year")(); }
      TagLib::uint track() const { return this->get_override("track")(); }
      void setTitle(const String &v) const { this->get_override("setTitle")(v); }
      void setArtist(const String &v) const { this->get_override("setArtist")(v); }
      void setAlbum(const String &v) const { this->get_override("setAlbum")(v); }
      void setComment(const String &v) const { this->get_override("setComment")(v); }
      void setGenre(const String &v) const { this->get_override("setGenre")(v); }
      void setYear(TagLib::uint i) const { this->get_override("setYear")(i); }
      void setTrack(TagLib::uint i) const { this->get_override("setTrack")(i); }
  };




  struct AudioPropertiesWrap : AudioProperties, wrapper<AudioProperties>
  {
      int length() const { return this->get_override("length")(); }
      int bitrate() const { return this->get_override("bitrate")(); }
      int sampleRate() const { return this->get_override("sampleRate")(); }
      int channels() const { return this->get_override("channels")(); }
    protected:
      AudioPropertiesWrap(ReadStyle style) : AudioProperties(style) { }
  };




  struct FileWrap : File, wrapper<File>
  {
    public:
      Tag *tag() const { return this->get_override("tag")(); }
      AudioProperties *audioProperties() const { return this->get_override("audioProperties")(); }
      bool save() { return this->get_override("save")(); }
    protected:
      FileWrap(const char *file) : File(file) { }
  };
}




void exposeID3();
void exposeRest();




BOOST_PYTHON_MODULE(_tagpy)
{
  // -------------------------------------------------------------
  // Infrastructure
  // -------------------------------------------------------------
  to_python_converter<String, tlstring_to_unicode>();
  to_python_converter<ByteVector, tlbytevector_to_string>();
  implicitly_convertible<std::wstring, String>();
  implicitly_convertible<std::string, ByteVectorIntermediate>();
  implicitly_convertible<ByteVectorIntermediate, ByteVector>();

  // -------------------------------------------------------------
  // Basics
  // -------------------------------------------------------------
  exposeList<String>("StringListBase");

  {
    typedef StringList cl;
    class_<cl, bases<List<String> > >("StringList")
      ;
  }

  {
    typedef Tag cl;
    class_<TagWrap, boost::noncopyable>("Tag", no_init)
      .add_property("title", &cl::title, &cl::setTitle)
      .add_property("artist", &cl::artist, &cl::setArtist)
      .add_property("album", &cl::album, &cl::setAlbum)
      .add_property("comment", &cl::comment, &cl::setComment)
      .add_property("genre", &cl::genre, &cl::setGenre)
      .add_property("year", &cl::year, &cl::setYear)
      .add_property("track", &cl::track, &cl::setTrack)

      .DEF_VIRTUAL_METHOD(isEmpty)
      .DEF_SIMPLE_METHOD(duplicate)
      .staticmethod("duplicate")
      ;
  }

  {
    typedef AudioProperties cl;
    class_<AudioPropertiesWrap, boost::noncopyable>("AudioProperties", no_init)
      .add_property("length", &cl::length)
      .add_property("bitrate", &cl::bitrate)
      .add_property("sampleRate", &cl::sampleRate)
      .add_property("channels", &cl::channels)
      ;
  }

  enum_<AudioProperties::ReadStyle>("ReadStyle")
    .value("Fast", AudioProperties::Fast)
    .value("Average", AudioProperties::Average)
    .value("Accurate", AudioProperties::Accurate)
    ;

  {
    typedef File cl;

    class_<FileWrap, boost::noncopyable>("File", no_init)
      .def("name", &File::name)
      .def("audioProperties", pure_virtual(&File::audioProperties),
           return_internal_reference<>())
      .def("tag", pure_virtual(&File::tag),
           return_internal_reference<>())
      .DEF_VIRTUAL_METHOD(save)
      .DEF_SIMPLE_METHOD(readOnly)
      .DEF_SIMPLE_METHOD(isOpen)
      .DEF_SIMPLE_METHOD(isValid)
      .DEF_SIMPLE_METHOD(clear)
      .DEF_SIMPLE_METHOD(length)
      ;
  }

  enum_<String::Type>("StringType")
    .value("Latin1", String::Latin1)
    .value("UTF16", String::UTF16)
    .value("UTF16BE", String::UTF16BE)
    .value("UTF8", String::UTF8)
    .value("UTF16LE", String::UTF16LE)
    ;

  exposeID3();
  exposeRest();
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
