/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
	friend class HashSet<Data>;
   public:
	iterator(size_t empty) { if (empty == 0) { _bucketaddr = 0; _nowbucket = 0; _nownode = 0; isend = true;} }
	iterator(vector<Data>* addr, size_t bucketnum, size_t nodenum, size_t numbucket, bool isEnd): _bucketaddr(addr), 
	_nowbucket(bucketnum), _nownode(nodenum), _totalBuckets(numbucket), isend(isEnd) { _update(); } 
	~iterator() {}
	const Data& operator * () const { return _bucketaddr[_nowbucket].at(_nownode); }
	iterator& operator ++ () { getnext(); return (*this); }
	iterator operator ++ (int) { iterator temp = *this; getnext(); return temp; }
	iterator& operator -- () { getprevious(); return *(this); }
	iterator operator -- (int) { iterator temp = *this; getprevious(); return temp; }
	bool operator != (const iterator& i) const 
	{ 
	    if ((_nowbucket != i._nowbucket)||(_nownode != i._nownode)||(_bucketaddr != i._bucketaddr)||(isend != i.isend)) return true; 
	    else return false;
	}
	bool operator == (const iterator& i) const 
	{ 
	    if ((_nowbucket != i._nowbucket)||(_nownode != i._nownode)||(_bucketaddr != i._bucketaddr)||(isend != i.isend)) return false;
	    else return true;
	}

	iterator& operator = (const iterator& i) { _bucketaddr = i._bucketaddr; _nowbucket = i._nowbucket; _nownode = i._nownode; _update(); _totalBuckets = i._totalBuckets; return *(this); }

   private:
	vector<Data>* _bucketaddr;
	size_t _nowbucket;
	size_t _nownode;
	size_t _nowbucketsize;
	size_t _totalBuckets;
	bool isend;
	void _update() { _nowbucketsize = _bucketaddr[_nowbucket].size(); }
	void getnext() 
	{
	    size_t tempold = _nowbucket;
	    if (_nownode != _nowbucketsize - 1)
	    {
		_nownode += 1;
		return;
	    }
	    else
	    {
		while (_nowbucket < _totalBuckets - 1)
		{
		    if (_bucketaddr[++_nowbucket].size() != 0)
		    {
			_update();
			_nownode = 0;
			return;
		    }
		}
	    }
	    isend = true;
	    _nowbucket = tempold;
	}
	void getprevious() 
	{
	    if (isend)
		isend = false;
	    size_t tempold = _nowbucket;
	    if (_nownode != 0)
	    {
		_nownode -= 1;
		return;
	    }
	    else
	    {
		while (_nowbucket > 0)
		{
		    if (_bucketaddr[--_nowbucket].size() != 0)
		    {
			_update();
			_nownode = _nowbucketsize - 1;
			return;
		    }
		}
	    }
	    _nowbucket = tempold;
	}
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const 
   {
	if (empty())  return end();
	size_t bucketbegin = 0;
	while (_buckets[bucketbegin].empty())
	    ++bucketbegin;
	return iterator(_buckets, bucketbegin, 0, _numBuckets, false); 
   }
   // Pass the end
   iterator end() const 
   {
	if (empty())  return 0;
	size_t bucketbegin = _numBuckets-1;
	while (_buckets[bucketbegin].empty())
	    --bucketbegin;
	return iterator(_buckets, bucketbegin, _buckets[bucketbegin].size()-1, _numBuckets, true); 
   }
   // return true if no valid data
   bool empty() const 
   { 
	size_t bucketbegin = 0;
	while (_buckets[bucketbegin].empty())
	{
	    if (bucketbegin == _numBuckets - 1)
		return true;
	    ++bucketbegin;
	}
	return false;
   }
   // number of valid data
   size_t size() const 
   { 
	size_t s = 0;
	for (size_t i=0; i<_numBuckets; ++i)
	    s += _buckets[i].size();
	return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
   {
	size_t bucketnum = bucketNum(d);
	for (unsigned k=0; k<_buckets[bucketnum].size(); ++k)
	{
	    if (_buckets[bucketnum].at(k) == d)
		return true;
	}
	return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const
   {
	size_t bucketnum = bucketNum(d);
	for (unsigned k=0; k<_buckets[bucketnum].size(); ++k)
	{
	    if (_buckets[bucketnum].at(k) == d)
	    {
		d = _buckets[bucketnum].at(k);
		return true;
	    }
	}
	return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) 
   {
	size_t bucketnum = bucketNum(d);
	for (unsigned k=0; k<_buckets[bucketnum].size(); ++k)
	{
	    if (_buckets[bucketnum].at(k) == d)
	    {
		_buckets[bucketnum].at(k) = d;
		return true;
	    }
	}
	insert(d);
	return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) 
   {
	if (!check(d))
	{
	    size_t bucketnum = bucketNum(d);
	    _buckets[bucketnum].push_back(d);
	    return true;
	}
	return false; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
   {
	size_t bucketnum = bucketNum(d);
	for (unsigned k=0; k<_buckets[bucketnum].size(); ++k)
	{
	    if (_buckets[bucketnum].at(k) == d)
	    {
		if (k != _buckets[bucketnum].size()-1)
		    _buckets[bucketnum].at(k) = _buckets[bucketnum].at(_buckets[bucketnum].size()-1);
		_buckets[bucketnum].pop_back();
		return true;
	    }
	}
	return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
