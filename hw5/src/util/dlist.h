/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator temp = *this; _node = _node->_next; return temp; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator temp = *this; _node = _node->_prev; return temp; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { if (i._node != _node) return true; else return false; }
      bool operator == (const iterator& i) const { if (i._node != _node) return false; else return true; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { if (empty()) return end(); else return _head; }
   iterator end() const { if (empty()) return _head; else return --begin(); }
   bool empty() const { if (_head->_next == _head) return true; else return false; }
   size_t size() const { 
	if (empty()) return 0; 
	iterator temp = begin(); 
	size_t count = 0;
	while (temp != end()) 
	{
	    ++ count;
	    ++ temp;
	}
	return count;
   }

   void push_back(const T& x)
   {	
	bool check_first = empty();
	DListNode<T>* new_node = new DListNode<T>(x, (--end())._node, end()._node);
	(--end())._node->_next = new_node;
	end()._node->_prev = new_node;
	if (check_first)
	    _head = new_node;
   }
   void pop_front() { erase(begin()); }

   void pop_back() { erase(--end()); }

   // return false if nothing to erase
   bool erase(iterator pos) 
   {
	if (!empty())
	{
	    pos._node->_next->_prev = pos._node->_prev;
	    pos._node->_prev->_next = pos._node->_next;
	    if (pos._node == _head)
		_head = _head->_next;
	    delete pos._node;
	    return true;
	}
	return false;
   }
   bool erase(const T& x) { iterator iter = find(x); if (iter!=end()) return erase(iter); return false; }

   iterator find(const T& x) { 
	for (iterator iter=begin(); iter!=end(); iter++)  
	{
	    if (iter._node->_data == x)
		return iter;
	}
	return end(); 
   }

   void clear() { while (!empty()) pop_back(); }  // delete all nodes except for the dummy node

   void sort() const 
   {
	if (empty())
	    return;
	for (iterator i=begin(); i!=end(); i++)
	{
	    iterator j = i;
	    ++j;
	    for (; j!=end(); j++)
	    {
		if (i._node->_data > j._node->_data)
		{
		    T temp = i._node->_data;
		    i._node->_data = j._node->_data;
		    j._node->_data = temp;
		}
	    }
	} 
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
