/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());


   string temp_str;
   int temp_value;
   is >> temp_str;  // ignore "{"
   while (is)
   {
	is >> temp_str;
	if (temp_str[0] == '"')
	    temp_str = temp_str.substr(1, temp_str.size()-2);
	else if (temp_str[0] == ',')
	{
	    is >> temp_str;
	    temp_str = temp_str.substr(1, temp_str.size()-2);
	}
	else
	    break;
	string temp_int;	
	is >> temp_int;  // ignore ":"
	is >> temp_int;

	if (temp_int[temp_int.size()-1] == ',')
	    temp_int = temp_int.substr(0, temp_int.size()-1);
	temp_value = atoi(temp_int.c_str());
	j._obj.push_back(DBJsonElem(temp_str, temp_value));
   }
   j.isread = true;   

   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{" << endl;
   if (j.size() > 0)
   {
	for (size_t i=0; i<j.size()-1; ++i)
	    os << "  \"" << j._obj.at(i).key() << "\" : " << j._obj.at(i).value() << "," << endl; 
	os << "  \"" << j._obj.at(j.size()-1).key() << "\" : " << j._obj.at(j.size()-1).value() << endl; 
   }
   os << "}" << endl;
   os << "Total JSON elements: " << j.size() << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   vector<DBJsonElem>{}.swap(_obj);
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for (size_t i=0; i<size(); ++i)
   {
	if (_obj.at(i).key() == elm.key())
	    return false;
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if (empty())	return NAN;
   return (static_cast<float>(sum())/_obj.size());
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if (empty()) idx = size();
   else
   {
	for (size_t i=0; i<size(); ++i)
	{
	    if (maxN < _obj.at(i).value())
	    {
		maxN = _obj.at(i).value();
		idx = i;
	    }
	}
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if (empty()) idx = size();
   else
   {
	for (size_t i=0; i<size(); ++i)
	{
	    if (minN > _obj.at(i).value())
	    {
		minN = _obj.at(i).value();
		idx = i;
	    }
	}
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   for (size_t i = 0; i < size(); ++i)
	s += _obj.at(i).value();

   return s;
}
