/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
   string getkey() const;
   int getvalue() const;

   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
   // TODO: define constructor & member functions on your own
   Json() {}
   Json(const string& k) {read(k);}
   bool read(const string&);
   void print(const string&);
   void sum(const string&);
   void ave(const string&);
   void max(const string&);
   void min(const string&);
   void add(const string&);
   bool check_operation(const string &);
private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
};

#endif // P2_TABLE_H
