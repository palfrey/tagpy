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




#include <boost/python.hpp>
#include <boost/scoped_array.hpp>
#include <stdexcept>




using namespace boost::python;
using namespace TagLib;
using namespace std;



#define MF_OL(MF, MIN, MAX) \
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MF##_overloads, MF, MIN, MAX);
#define DEF_SIMPLE_METHOD(NAME) \
  def(#NAME, &cl::NAME)
#define DEF_VIRTUAL_METHOD(NAME) \
  def(#NAME, pure_virtual(&cl::NAME))
#define DEF_OVERLOADED_METHOD(NAME, CAST) \
  def(#NAME, (CAST) &cl::NAME, NAME##_overloads())
#define ENUM_VALUE(NAME) \
  value(#NAME, scope::NAME)
#define ADD_RO_PROPERTY(NAME) \
  add_property(#NAME, &cl::NAME)

#define CHECK_VERSION(MAJOR, MINOR, PATCH) \
  (MAJOR << 16) + \
  (MINOR << 8) + \
  (PATCH << 0)

#define TAGLIB_HEX_VERSION CHECK_VERSION(TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION, TAGLIB_PATCH_VERSION)

#if CHECK_VERSION(1,9,0) < TAGLIB_HEX_VERSION
#warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#warning TagPy is meant to wrap TagLib 1.9 and above.
#warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif




namespace {
  template<typename Iterator>
  inline object make_list(Iterator first, Iterator last)
  {
    boost::python::list result;
    while (first != last)
      result.append(*first++);
    return result;
  }




  // -------------------------------------------------------------
  // Map
  // -------------------------------------------------------------
  template<typename Key, typename Value>
  Value &Map_getitem(Map<Key, Value> &m, const Key &k)
  {
    if (!m.contains(k))
    {
      PyErr_SetString( PyExc_KeyError, "key not in map");
      throw error_already_set();
    }
    return m[k];
  }

  template<typename Key, typename Value>
  void Map_setitem(Map<Key, Value> &m, const Key &k, const Value &v)
  {
    m[k] = v;
  }

  template<typename Key, typename Value>
  object Map_keys(Map<Key, Value> &m)
  {
    boost::python::list keys;

    typedef Map<Key, Value> map;
    typename map::Iterator first = m.begin(), last = m.end();
    while (first != last)
      keys.append((first++)->first);
    return keys;
  }

  template<typename Key, typename Value>
  void exposeMap(const char *name)
  {
    typedef Map<Key, Value> map;
    class_<map>(name)
      .def("__len__", &map::size)
      .def("size", &map::size)
      .def("clear", &map::clear, return_self<>())
      .def("isEmpty", &map::isEmpty)
      .def("__getitem__", Map_getitem<Key, Value>, return_internal_reference<>())
      .def("__setitem__", Map_setitem<Key, Value>)
      .def("__contains__", &map::contains)
      .def("keys", Map_keys<Key, Value>)
      ;
  }

  // -------------------------------------------------------------
  // List
  // -------------------------------------------------------------
  template<typename Value>
  Value &List_getitem(List<Value> &l, uint i)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    return l[i];
  }

  template<typename Value>
  void List_setitem(List<Value> &l, uint i, Value v)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    l[i] = v;
  }

  template<typename Value>
  void List_append(List<Value> &l, Value v)
  {
    l.append(v);
  }

  template<typename Value>
  void exposeList(const char *name)
  {
    typedef List<Value> list;
    class_<list>(name)
      .def("__len__", &list::size)
      .def("size", &list::size)
      .def("clear", &list::clear, return_self<>())
      .def("isEmpty", &list::isEmpty)
      .def("__getitem__", List_getitem<Value>, return_value_policy<return_by_value>())
      .def("__setitem__", List_setitem<Value>)
      .def("append", List_append<Value>)
      // MISSING: iterators, insert, find, contains, erase,
      // assignment, comparison
      ;
  }

  // -------------------------------------------------------------
  // PointerList
  // -------------------------------------------------------------
  template<typename Value>
  Value *&PointerList_getitem(List<Value *> &l, uint i)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    return l[i];
  }

  template<typename Value>
  void PointerList_setitem(List<Value *> &l, uint i, auto_ptr<Value> v)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    l[i] = v.release();
  }

  template<typename Value>
  void PointerList_append(List<Value *> &l, auto_ptr<Value> v)
  {
    l.append(v.release());
  }

  template<typename Value>
  void exposePointerList(const char *name)
  {
    typedef List<Value *> list;
    class_<list>(name)
      .def("__len__", &list::size)
      .def("size", &list::size)
      .def("clear", &list::clear, return_self<>())
      .def("isEmpty", &list::isEmpty)
      .def("__getitem__", PointerList_getitem<Value>, return_internal_reference<>())
      .def("__setitem__", PointerList_setitem<Value>)
      .def("append", PointerList_append<Value>)
      // MISSING: iterators, insert, find, contains, erase,
      // assignment, comparison
      ;
  }
}

template <class T>
struct TagWrap : T, wrapper<T>
{
    String title() const { return this->get_override("title")(); }
    String artist() const { return this->get_override("artist")(); }
    String album() const { return this->get_override("album")(); }
    String comment() const { return this->get_override("comment")(); }
    String genre() const { return this->get_override("genre")(); }
    unsigned int year() const { return this->get_override("year")(); }
    unsigned int track() const { return this->get_override("track")(); }
    void setTitle(const String &v) const { this->get_override("setTitle")(v); }
    void setArtist(const String &v) const { this->get_override("setArtist")(v); }
    void setAlbum(const String &v) const { this->get_override("setAlbum")(v); }
    void setComment(const String &v) const { this->get_override("setComment")(v); }
    void setGenre(const String &v) const { this->get_override("setGenre")(v); }
    void setYear(unsigned int i) const { this->get_override("setYear")(i); }
    void setTrack(unsigned int i) const { this->get_override("setTrack")(i); }
};
