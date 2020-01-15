/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   cout << "Enter command: ";
   cin.get();  // avoid enter
   while (true) {
	string operation;
	getline(cin, operation);
	if (operation.length() == 0)
	{
	    cout << "Enter command: ";
	    continue;
	}
	size_t idx = 0;
	bool leave = false;
	while (isspace (operation.at(idx)))
	{
	    idx ++ ;
	    if (operation.length() <= idx)
	    {
	        cout << "Enter command: ";
		leave = true;
	        break;
	    }
	}
	if (leave)
	    continue;
	operation = operation.substr(idx);
	if (operation.length() < 3)
	{
	    cout << "Error: unknown command: \"" << operation << "\"" << endl;
	    cout << "Enter command: ";
	    continue;
	}
	if (operation.substr(0, 3) == "SUM")
	    json.sum(operation);
	else if (operation.substr(0, 3) == "AVE")
	    json.ave(operation);
	else if (operation.substr(0, 3) == "MAX")
	    json.max(operation);
	else if (operation.substr(0, 3) == "MIN")
	    json.min(operation);
	else if (operation.substr(0, 3) == "ADD")
	    json.add(operation);
	else if (operation.substr(0, 4) == "EXIT")
	{
	    for (size_t k=4; k<operation.length();k++)
	    {
		if(!isspace(operation.at(k)))
		{
	    	    cout << "Error: unknown command: \"" << operation << "\"" << endl;
		    cout << "Enter command: ";
		    leave = true;
		    break;
		}
	    }
	    if (leave)
		continue;
	    break;
	}
	else if (operation.substr(0, 5) == "PRINT")
	    json.print(operation);
	else
	    cout << "Error: unknown command: \"" << operation << "\"" << endl;
	cout << "Enter command: ";
	
   }
   return 0;
}
