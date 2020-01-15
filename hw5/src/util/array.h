/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(false) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node = _node + 1; return (*this); }
      iterator operator ++ (int) { iterator temp = *this; _node = _node + 1; return (temp); }
      iterator& operator -- () { _node = _node - 1; return (*this); }
      iterator operator -- (int) { iterator temp = *this; _node = _node - 1; return (temp); }

      iterator operator + (int i) const { iterator temp = *this; temp += i; return (temp); }
      iterator& operator += (int i) { for (int j=0; j<i; ++j) ++ _node; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { if (i._node != this->_node) return true; else return false;}
      bool operator == (const iterator& i) const { if (i._node != this->_node) return false; else return true;}

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { if (empty()) return end(); else return _data; }
   iterator end() const { if (_capacity == 0) return 0; else return _data + _size;}
   bool empty() const {if (_size == 0) return true; else return false; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { if (_size == _capacity) expand(); _data[_size ++] = x;}
   void pop_front() { erase(begin()); }
   void pop_back() { if (empty()) return; erase(--end()); }

   bool erase(iterator pos) { if (empty()) return false; *(pos._node) = _data[_size-1]; --_size; return true; }
   bool erase(const T& x) { iterator iter = find(x); if (iter == end()) return false; else return erase(iter); }

   iterator find(const T& x) { 
	for (iterator iter = begin(); iter != end(); iter++)
	{
	    if (*(iter._node) == x)
		return iter;
	}
	return end(); 
   }

   void clear() { _size = 0;}

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); _isSorted = true; }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void expand() 
   {
	if (_capacity > 0)
	{
	    T* new_data = new T[2*_capacity];
	    for (size_t j=0; j<_size; ++j)
		new_data[j] = _data[j];
	    delete []_data;
	    _data = new_data;
	    _capacity = 2*_capacity;
	}
	else
	{
	    _data = new T[1];
	    _capacity = 1;
	}
   }
};

#endif // ARRAY_H
