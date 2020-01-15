/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   {
	_data.push_back(d);
	size_t n = size()-1;
	size_t p = (n-1) / 2;
	while (n != 0)
	{
	    if (d < _data[p])
	    {
		_data[n] = _data[p];
		n = p;
		p = (p-1)/2;
	    }
	    else
		break;
	}
	_data[n] = d;
   }
   void delMin() { delData(0); }
   void delData(size_t i) 
   {
	size_t temp = _data[i].getLoad();
	_data[i] = _data[size()-1];
	if (_data[i].getLoad() < temp)
	{
	    size_t n = i;
	    size_t p = (n-1) / 2;
	    while (n != 0)
	    {
	        if (_data[size()-1] < _data[p])
	        {
		    _data[n] = _data[p];
		    n = p;
		    p = (p-1)/2;
	        }
	        else
		    break;
	    }
	    _data[n] = _data[size()-1];
	    _data.pop_back();
	    return;
	}
	size_t parent = i;
	size_t child = 2*parent + 2;
	while (child < size()-1)
	{
	    if ((_data[size()-1].getLoad() <= _data[child-1].getLoad()) && (_data[size()-1].getLoad() <= _data[child].getLoad()))
		break;
	    else if (_data[child] < _data[child-1])
	    {
		_data[parent] = _data[child];
		parent = child;
		child = parent*2 + 2;
	    }
	    else
	    {
		_data[parent] = _data[child-1];
		parent = child - 1;
		child = parent*2 + 2;
	    }
	}
	if ((child-1 < size()-1) && (_data[size()-1].getLoad() > _data[child-1].getLoad()))
	{
	    _data[parent] = _data[child-1];
	    parent = child - 1;
	    child = parent*2 + 2;
	}
	_data[parent] = _data[size()-1];
	_data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
