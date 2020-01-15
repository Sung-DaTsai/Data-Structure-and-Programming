/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   
   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0): _data(d), _left(l), _right(r) {}
   
   T _data;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
   
};


template <class T>
class BSTree
{
public:
   BSTree() {
      _root = 0; // = new BSTreeNode<T>(T());
   }
   ~BSTree() { clear();}
   // TODO: design your own class!!
   class iterator {
      friend class BSTree;

      typedef pair<BSTreeNode<T>* const, bool>  trace_pair;  // second = false means left_child, = true means right_child

   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}  // temporarily only for _root
      iterator(const iterator& i) : _node(i._node), _trace(i._trace) {}
	  iterator(bool begin_or_end, BSTreeNode<T>* n): _node(n) { if (begin_or_end) { while(to_leftchild()) {} } else { while(to_rightchild()) {} _trace.push(trace_pair(_node, true)); } }  // from _root to _begin or _end
      ~iterator() {}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { if (! next_node()) _trace.push(trace_pair(_node, true));  return *(this); }
      iterator operator ++ (int) { iterator temp = *this; if (! next_node()) _trace.push(trace_pair(_node, true)); return temp; }
      iterator& operator -- () { previous_node(); return *(this); }
      iterator operator -- (int) { iterator temp = *this; previous_node(); return temp; }

      iterator& operator = (const iterator& i) { _node = i._node; _trace = i._trace; return *(this); }

      bool operator != (const iterator& i) const { if ((i._node == _node) && (i._trace == _trace)) return false; else return true; }
      bool operator == (const iterator& i) const { if ((i._node == _node) && (i._trace == _trace)) return true; else return false; }

   private:
      BSTreeNode<T>* _node;
	  stack <trace_pair>  _trace;
	  bool to_parent()
	  {
		  if (_trace.size() == 0)
			  return false;
		  else
		  {
			  _node = _trace.top().first;
			  _trace.pop();
			  return true;
	      }
	  }
	  bool to_leftchild()
	  {
		  if (_node->_left == 0)
			  return false;
		  else
		  {
			  _trace.push(trace_pair(_node, false));
			  _node = _node->_left;
			  return true;
		  }
	  }
	  bool to_rightchild()
	  {
		  if (_node->_right == 0)
			  return false;
		  else
		  {
			  _trace.push(trace_pair(_node, true));
			  _node = _node->_right;
			  return true;
		  }
	  }
	  bool next_node()  // for ++, false when the last element reaches (need to add to dummy node, _end)
	  {
		  if ((_trace.size() != 0) && (_trace.top().first == _node)) // _end
			  return true;
		  else if (to_rightchild())
		  {
			  while (to_leftchild()) {}
		  }
		  else if (_trace.size() == 0)  // root but no right_child (--_end)
			  return false;
		  else if (! _trace.top().second)
			  to_parent();
		  else
		  {
			  while (to_parent())
			  {
				  if (_trace.size() == 0)  // original node is maximum (--_end)
				  {
					  while (to_rightchild()) {}
					  return false;
				  }
				  else if (! _trace.top().second)
				  {
					  to_parent();
					  return true;
				  }
			  }
		  }
		  return true;
	  }
	  void previous_node()  // for --
	  {
		  if ((_trace.size() != 0) && (_trace.top().first == _node)) // _end
			  _trace.pop();
		  else if (to_leftchild())
		  {
			  while (to_rightchild()) {}
		  }
		  else if (_trace.size() == 0)
			  return;
		  else if (_trace.top().second)
			  to_parent();
		  else
		  {
			  while (to_parent())
			  {
				  if (_trace.size() == 0)  // original node is minimum (_begin)
				  {
					  while (to_leftchild()) {}
					  return;
				  }
				  else if (_trace.top().second)
				  {
					  to_parent();
					  return;
				  }
			  }
		  }
	  }
   };
   

   iterator begin() const { if (!empty()) return iterator(true, _root); else return end(); }
   iterator end() const { if (!empty()) return iterator(false, _root); else return 0; }
   bool empty() const { if (_root == 0) return true; else return false; }
   size_t size() const { 
	   int count = 0;
	   for (iterator iter = begin(); iter != end(); iter++)
	       ++ count; 
	   return count; 
   }

   void insert(const T& x) 
   {
		if (empty())
			_root = new BSTreeNode<T>(x);
		else
			find_insert(x, true);

   }
   void pop_front() { if (!empty()) erase(begin()); }
   void pop_back() { if (!empty()) erase(--end()); }

   bool erase(iterator pos) 
   {
		if (empty())
			return false;
		else
		{
			iterator temp = pos;
			if ((pos._node->_left == 0) && (pos._node->_right == 0))
			{
				if (pos._trace.size() == 0)
				{
					delete temp._node;
					_root = 0;
				}
				else if ((pos._trace.size() != 0) && (pos._trace.top().second))
				{
					pos.to_parent();
					pos._node->_right = 0;
					delete temp._node;
				}
				else if ((pos._trace.size() != 0) && (! pos._trace.top().second))
				{
					pos.to_parent();
					pos._node->_left = 0;
					delete temp._node;
				}
			}
			else if ((pos._node->_left != 0) && (pos._node->_right != 0))
			{
				bool right_child = false;
				successor(pos, right_child);
				if (temp._trace.size() == 0)
				{
					if (!right_child)
					{
						_root = pos._node;
						pos._node->_left = temp._node->_left;
						delete temp._node;
					}
					else
					{
						temp._node->_data = pos._node->_data;
						if (pos._node->_right == 0)
							pos._trace.top().first->_left = 0;
						else
							pos._trace.top().first->_left = pos._node->_right;
						delete pos._node;
					}
				}
				else
				{
					if (!right_child)
					{
						pos._node->_left = temp._node->_left;
						if (! temp._trace.top().second)
							temp._trace.top().first->_left = pos._node;
						else
							temp._trace.top().first->_right = pos._node;
						delete temp._node;
					}
					else
					{
						temp._node->_data = pos._node->_data;
						if (pos._node->_right == 0)
							pos._trace.top().first->_left = 0;
						else
							pos._trace.top().first->_left = pos._node->_right;
						delete pos._node;
					}
				}
			}
			else
			{
				if (pos._trace.size() == 0)
				{
					if (pos.to_leftchild()) {} 
					else if (pos.to_rightchild()) {}
					_root = pos._node;
					delete temp._node;
				}
				else if ((pos._trace.size() != 0) && (pos._trace.top().second))
				{
					pos.to_parent();
					if (temp._node->_left != 0)
						pos._node->_right = temp._node->_left;
					else if (temp._node->_right != 0)
						pos._node->_right = temp._node->_right;
					delete temp._node;
				}
				else if ((pos._trace.size() != 0) && (! pos._trace.top().second))
				{
					pos.to_parent();
					if (temp._node->_left != 0)
						pos._node->_left = temp._node->_left;
					else if (temp._node->_right != 0)
						pos._node->_left = temp._node->_right;
					delete temp._node;
				}
			}
		}  
		return true; 
   }

   bool erase(const T& x) 
   {
		iterator iter = find(x);
		if (iter == end()) 
			return false;
		else
			return erase(iter);
   }

   iterator find(const T& x) 
   {
		if (empty())
			return end(); 
		else
			return find_insert(x);
   }

   void clear() { while(!empty()) pop_front(); }

   void sort() const { }
   void print() const { recursive_print(_root, 0); }

private:
   BSTreeNode<T>*  _root;     // = dummy node if bst is empty
   void successor(iterator &iter, bool& rightchild_or_rightleftchild)
   {
		if (iter._node->_right != 0)
		{
			if (iter.to_rightchild())
			{
				rightchild_or_rightleftchild = false;
				while (iter.to_leftchild()) { rightchild_or_rightleftchild = true; }
			}
		}
   }
   iterator find_insert(const T& x, bool inserting = false)
   {
		iterator iter(_root);
		while (true)
		{
			if (x == iter._node->_data)
			{
				if (inserting)
				{
					if (iter._node->_left != 0)
					{
						iter.to_leftchild();
						continue;
					}
					else
						iter._node->_left = new BSTreeNode<T>(x);
				}
				return iter;
			}
			else if (x > iter._node->_data)
			{
				if (! iter.to_rightchild())
				{
					if (inserting)
						iter._node->_right = new BSTreeNode<T>(x);
					return end();
				}
					
			}
			else
			{
				if (! iter.to_leftchild())
				{
					if (inserting)
						iter._node->_left = new BSTreeNode<T>(x);
					return end();
				}
			}
		}
   }
   void recursive_print(BSTreeNode<T>* ptr, int space) const
   {
		for (int i=0; i<space; ++i)
			cout << "  " ;
		if (ptr == 0)
			cout << "[0]" << endl;
		else
		{
			cout << ptr->_data << endl;
			recursive_print(ptr->_left, space+1);
			recursive_print(ptr->_right, space+1);
		}
   }
};

#endif // BST_H
