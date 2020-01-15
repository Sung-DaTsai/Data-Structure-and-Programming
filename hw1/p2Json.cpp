/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   ifstream json_input(jsonFile);
   if (json_input.fail())  // file doesn't exist
	return false;

   string each_line;
   string each_key;
   int each_value;
   int start_idx;
   int end_idx;
   bool new_element = true;
   bool start_value = false;
   bool comma_pass = false;
   vector <string> final_line;  // find out all the commas and separate at those positions
   while (getline(json_input, each_line))
   {
	for (size_t i=0; i<each_line.length(); i++)
	{
	    if(each_line[i] == ':')
		comma_pass = true;
	    if((!comma_pass) && new_element && (each_line[i] == '"') )
	    {
		new_element = false;
		start_idx = i;
	    }
	    else if((!comma_pass) && (!new_element) && (each_line[i] == '"') )
	    {
		end_idx = i;
		each_key = each_line.substr(start_idx + 1, end_idx - start_idx - 1);
		new_element = true;
	    }
	    else if((comma_pass) && (!start_value) && (isdigit(each_line[i])) && (i == each_line.length()-1)) // only a digit appears at the end of line
	    {
		start_idx = i;
		each_value = atoi((each_line.substr(start_idx)).c_str());
		JsonElem elements(each_key, each_value);
		(this -> _obj).push_back(elements);
		comma_pass = false;
	    }
	    else if((comma_pass) && (!start_value) && ((isdigit(each_line[i])) || each_line[i] == '-')) // a number with more than one digit
	    {
		start_idx = i;
		start_value = true;
	    }
	    else if((comma_pass) && (start_value) && (i == each_line.length()-1) )  // a number ends at the end of the line
	    {
		end_idx = i;
		each_value = atoi((each_line.substr(start_idx)).c_str());
		start_value = false;
		JsonElem elements(each_key, each_value);
		(this -> _obj).push_back(elements);
		comma_pass = false;
	    }
	    else if((comma_pass) && (start_value) && (!isdigit(each_line[i])) )  // a number ends
	    {
		end_idx = i;
		each_value = atoi((each_line.substr(start_idx, end_idx - start_idx)).c_str());
		start_value = false;
		JsonElem elements(each_key, each_value);
		(this -> _obj).push_back(elements);
		comma_pass = false;
	    }

	}
  
   }
   return true; // TODO
}

string JsonElem::getkey() const
{
    return _key;
}

int JsonElem::getvalue() const
{
    return _value;
}

void Json::print(const string& check)
{
    if (check.length() == 5) {}
    else if (! check_operation(check.substr(5)) ) 
	return;
    cout << "{" << endl;
    for (size_t i=0; i<_obj.size(); i++)
    {
	if(i != _obj.size()-1)
	    cout << "  " << _obj.at(i) << "," << endl;
	else
	    cout << "  " << _obj.at(_obj.size()-1) << endl;
    }
    cout << "}" << endl ;
}

void Json::sum(const string& check)
{
    if (check.length() == 3) {}
    else if (! check_operation(check.substr(3)) ) 
	return;
    if(_obj.size() == 0)
    {
	cout << "Error: No element found!!" << endl ;
	return;
    }
    int summation = 0;
    for(size_t i=0; i<_obj.size(); i++)
	summation += _obj.at(i).getvalue(); 
    cout << "The summation of the values is: " << summation << "." << endl ;
}

void Json::ave(const string& check)
{
    if (check.length() == 3) {}
    else if (! check_operation(check.substr(3)) ) 
	return;
    if(_obj.size() == 0)
    {
	cout << "Error: No element found!!" << endl ;
	return;
    }
    int average = 0;
    for(size_t i=0; i<_obj.size(); i++)
	average += _obj.at(i).getvalue(); 
    cout << "The average of the values is: " << setprecision(1) << fixed << static_cast <double>(average)/_obj.size() << "." << endl ;
}

void Json::max(const string& check)
{
    if (check.length() == 3) {}
    else if (! check_operation(check.substr(3)) ) 
	return;
    if(_obj.size() == 0)
    {
	cout << "Error: No element found!!" << endl ;
	return;
    }
    int max = _obj.at(0).getvalue();
    int idx = 0;
    vector <int> same;
    for(size_t i=1; i<_obj.size(); i++)
    {
	if(max < _obj.at(i).getvalue())
	{
	    idx = i;
	    max = _obj.at(i).getvalue();
	    vector <int> {}.swap(same);
	}
	else if(max == _obj.at(i).getvalue())
	{
	    if(same.size() == 0)
		same.push_back(idx);
	    same.push_back(i);
	}
    }
    if(same.size() == 0)
	cout << "The maximum element is: { " << _obj.at(idx) << " }." << endl ;
    else
    {
	cout << "The maximum element is: { ";
	for(size_t i=0; i<same.size(); i++)
	    cout << _obj.at(same.at(i)) << " ";
	cout << " }." << endl ;
    }
}

void Json::min(const string& check)
{
    if (check.length() == 3) {}
    else if (! check_operation(check.substr(3)) ) 
	return;
    if(_obj.size() == 0)
    {
	cout << "Error: No element found!!" << endl ;
	return;
    }
    int min = _obj.at(0).getvalue();
    int idx = 0;
    vector <int> same;
    for(size_t i=1; i<_obj.size(); i++)
    {
	if(min > _obj.at(i).getvalue())
	{
	    idx = i;
	    min = _obj.at(i).getvalue();
	    vector <int> {}.swap(same);
	}
	else if(min == _obj.at(i).getvalue())
	{
	    if(same.size() == 0)
		same.push_back(idx);
	    same.push_back(i);
	}
    }
    if(same.size() == 0)
	cout << "The minimum element is: { " << _obj.at(idx) << " }." << endl ;
    else
    {
	cout << "The minimum element is: { ";
	for(size_t i=0; i<same.size(); i++)
	    cout << _obj.at(same.at(i)) << " ";
	cout << " }." << endl ;
    }
}

void Json::add(const string& new_element)
{
    bool alpha = false;
    bool digit = false;
    size_t start;
    size_t end;
    size_t finals;
    string key;
    int value;
    if (new_element.length() <= 3)
    {
	cout << "Error: Missing argument!!" << endl;
	return;
    }
    else if(!isspace(new_element[3]))
    {
	cout << "Error: Illegal argument \"" << new_element.substr(3) << "\"!!" << endl;
	return;
    }
    for(size_t i=3; i<new_element.length(); i++)
    {
	if ((!alpha) && (!isspace(new_element[i])) )
	{
	    alpha = true;
	    start = i;
	}
	else if ((alpha) && (isspace(new_element[i])) )
	{
	    end = i;
            key = new_element.substr(start, end - start);
	    break;
	}
    }
    if (key == "")
    {
	cout << "Error: Missing argument!!" << endl;
	return;
    }
    for(size_t i=0; i<_obj.size(); i++)
    {
	if (key == _obj.at(i).getkey())
	{
	    cout << "Error: Key \"" << key << "\" is repeated!!" << endl;
	    return;
	}
    }

    for(size_t i=end; i<new_element.size(); i++)
    {
	if ((digit && (new_element[i] == '-')) || ((!digit) && (new_element[i] == '-') && (i == new_element.size()-1) ) )
	{
	    cout << "Error: Illegal argument \"" << new_element.substr(start) << "\"!!" << endl;
	    return;
	}
	else if( (!isdigit(new_element[i])) && (!isspace(new_element[i])) && (new_element[i] != '-') )
	{
	    cout << "Error: Illegal argument \"" << new_element.substr(i) << "\"!!" << endl;
	    return;
	}
	else if((!digit) && (isdigit(new_element[i])) && (i == new_element.size()-1) ) 
	{
	    start = i;
	    finals = i;
	    value = atoi((new_element.substr(start)).c_str());
	    digit = true;
	    break;
	}
	else if((!digit) && ((isdigit(new_element[i])) || (new_element[i] == '-')) ) 
	{
	    start = i;
	    digit = true;
	}
	else if(digit && (isdigit(new_element[i])) && (i == new_element.size()-1) )
	{
	    finals = i;
	    value = atoi((new_element.substr(start)).c_str());
	    break;
	}
	else if(digit && (isspace(new_element[i])) )
	{
	    finals = i;
	    value = atoi((new_element.substr(start, finals - start)).c_str());
    	    if (! check_operation(new_element.substr(finals)) ) 
		return;
	    break;
	}
    }
    if (!digit)
    {
	cout << "Error: Missing argument after \"" << key << "\"!!" << endl;
	return;
    }

    JsonElem new_elem(key, value);
    (this -> _obj).push_back(new_elem);
}

bool Json::check_operation (const string& operation)
{
    for(size_t i=0; i<operation.length(); i++)
    {
	if(!isspace(operation.at(i)))
	{
	    cout << "Error: Extra argument \"" << operation.substr(i) << "\"!!" << endl;
	    return false;
	}
    }
    return true;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

