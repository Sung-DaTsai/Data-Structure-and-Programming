/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   string token;
   int numObjects;
   int arraySize;
   size_t begin = myStrGetTok(option, token);
   bool new_array;
   if (token.size() == 0)
	return CmdExec::errorOption(CMD_OPT_MISSING, token);
   else if (begin == string::npos)
   {
	if (myStr2Int(token, numObjects))
	{
	    if (numObjects > 0)  // do the function mtn numObjects
	    {
		new_array = false;
            }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	}
	else if (! myStrNCmp("-Array", token, 2))
	    return CmdExec::errorOption(CMD_OPT_MISSING, token);
        else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }
   else if (myStr2Int(token, numObjects))  // mtn numObjects -Array (size_t arraySize)
   {
	if (numObjects <= 0)
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	begin = myStrGetTok(option, token, begin);
        if (begin == string::npos)
	{
	    if (! myStrNCmp("-Array", token, 2))
		return CmdExec::errorOption(CMD_OPT_MISSING, token);
	    else
		return CmdExec::errorOption(CMD_OPT_EXTRA, token);
	}
	else if (! myStrNCmp("-Array", token, 2))
	{
	    begin = myStrGetTok(option, token, begin);
	    if (begin != string::npos)
	    {
		if (myStr2Int(token, arraySize))
		{
		    begin = myStrGetTok(option, token, begin);
		    return CmdExec::errorOption(CMD_OPT_EXTRA, token);
		}
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	    }
	    else if (myStr2Int(token, arraySize))
	    {
		if (arraySize > 0)  // do the function mtn numObjects -Array (size_t arraySize)
		{
		    new_array = true;
		}
	    	else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	}
	else
	    return CmdExec::errorOption(CMD_OPT_EXTRA, token);
   }
   else if (! myStrNCmp("-Array", token, 2))  // mtn -Array (size_t arraySize) numObjects
   {
	begin = myStrGetTok(option, token, begin);
        if (begin == string::npos)
	{
	    if (myStr2Int(token, arraySize) && (arraySize > 0))
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	}
	else if (myStr2Int(token, arraySize))
	{
	    if (arraySize <= 0)
	    	return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	    begin = myStrGetTok(option, token, begin);
	    if (begin != string::npos)
	    {
		if (myStr2Int(token, numObjects) && (numObjects > 0))
		{
		    begin = myStrGetTok(option, token, begin);
		    return CmdExec::errorOption(CMD_OPT_EXTRA, token);
		}
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	    }
	    else if (myStr2Int(token, numObjects))
	    {
		if (numObjects > 0)  // do the function mtn -Array (size_t arraySize) numObjects 
		{
		    new_array = true;
		}
	    	else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
	}
	else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
   }
   else // wrong thing happens
	return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);

   try
   {
	if (new_array)
	    mtest.newArrs(numObjects, arraySize);
	else
	    mtest.newObjs(numObjects);
   }
   catch(bad_alloc)
   {  
	return CMD_EXEC_ERROR;
   }
   catch(...)
   {
	return CMD_EXEC_ERROR;
   }
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   string token;
   int id;
   size_t begin = 0;
   bool isarray = false;
   bool israndom = false;
   while (begin != string::npos)
   {   
	begin = myStrGetTok(option, token, begin);
	if (token.size() == 0)
	    break;
	tokens.push_back(token);
	if (tokens.size() == 4)
	    break;
   }
   if (tokens.size() == 2)
   {
	if (! myStrNCmp("-Index", tokens[0], 2))
	{
	    if ((myStr2Int(tokens[1], id)) && (id >= 0))
	    {
		if (id < mtest.getObjListSize())  // MTDelete -Index (size_t objId) 
		    isarray = false;
		else
		{
		    cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << id << "!!" << endl;
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
		}
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	}
	else if (! myStrNCmp("-Random", tokens[0], 2))
	{
	    if ((myStr2Int(tokens[1], id)) && (id > 0))
	    {
		if (mtest.getObjListSize() > 0)  // MTDelete -Random (size_t numRandId)
		    israndom = true;
		else
		{
		    cerr << "Size of object list is 0!!" << endl;
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
		}
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);	
	}
	else if (! myStrNCmp("-Array", tokens[0], 2))
	{
	    if ( (! myStrNCmp("-Index", tokens[1], 2)) || (! myStrNCmp("-Random", tokens[1], 2)) )
		return CmdExec::errorOption(CMD_OPT_MISSING, tokens[1]);
	    else if ((! myStrNCmp("-Array", tokens[1], 2)))
		return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	}
	else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else if (tokens.size() == 3)
   {
	if (! myStrNCmp("-Index", tokens[0], 2))
	{
	    if ((myStr2Int(tokens[1], id)) && (id >= 0))
	    {
		if (! myStrNCmp("-Array", tokens[2], 2))  
		{
		    if (id < mtest.getArrListSize())   // MTDelete -Random (size_t numRandId) -Array
		    	isarray = true;
		    else
		    {
			cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << id << "!!" << endl;
		    	return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
		    }
		}
		else if ( (! myStrNCmp("-Index", tokens[2], 2)) || (! myStrNCmp("-Random", tokens[2], 2)) )
		    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	}
	else if (! myStrNCmp("-Random", tokens[0], 2))
	{
	    if ((myStr2Int(tokens[1], id)) && (id > 0))
	    {
		if (! myStrNCmp("-Array", tokens[2], 2))  
		{
		    if (mtest.getArrListSize() > 0)   // MTDelete -Index (size_t objId) -Array
		    {
			isarray = true;
			israndom = true;

		    }
		    else
		    {
			cerr << "Size of array list is 0!!" << endl;
			return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
		    }
		}
		else if ( (! myStrNCmp("-Index", tokens[2], 2)) || (! myStrNCmp("-Random", tokens[2], 2)) )
		    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);	
	}
	else if (! myStrNCmp("-Array", tokens[0], 2))
	{
	    if (! myStrNCmp("-Index", tokens[1], 2))
	    {
		if ((myStr2Int(tokens[2], id)) && (id >= 0))
		{
		    if (id < mtest.getArrListSize())  // MTDelete -Array -Index (size_t objId) 
			isarray = true;
		    else
		    {
			cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << id << "!!" << endl;
			return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
		    }
		}
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
	    }
	    else if (! myStrNCmp("-Random", tokens[1], 2))
	    {
		if ((myStr2Int(tokens[2], id)) && (id > 0))
	        {
		    if (mtest.getArrListSize() > 0)  // MTDelete -Array -Random (size_t numRandId)
		    {
			isarray = true;
			israndom = true;
		    }
		    else
		    {
		        cerr << "Size of array list is 0!!" << endl;
		        return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
		    }
	        }
	        else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	}
	else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else if (tokens.size() == 1)
   {
	if (! myStrNCmp("-Array", tokens[0], 2))
	    return CmdExec::errorOption(CMD_OPT_MISSING, "");
	else if ( (! myStrNCmp("-Index", tokens[0], 2)) || (! myStrNCmp("-Random", tokens[0], 2)) )
	    return CmdExec::errorOption(CMD_OPT_MISSING, tokens[0]);
	else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else if (tokens.size() == 4)
   {
	if (! myStrNCmp("-Index", tokens[0], 2))
	{
	    if (!((myStr2Int(tokens[1], id)) && (id >= 0)))
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	    else if (! myStrNCmp("-Array", tokens[2], 2))
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);
	    else if ( (! myStrNCmp("-Index", tokens[2], 2)) || (! myStrNCmp("-Random", tokens[2], 2)) )
		return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
	}
	else if (! myStrNCmp("-Random", tokens[0], 2))
	{
	    if (!((myStr2Int(tokens[1], id)) && (id > 0)))
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	    else if (! myStrNCmp("-Array", tokens[2], 2))
	    {
		if ((! myStrNCmp("-Index", tokens[3], 2)) || (! myStrNCmp("-Random", tokens[3], 2)) || (! myStrNCmp("-Array", tokens[3], 2)))
		    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);
	    }
	    else if ( (! myStrNCmp("-Index", tokens[2], 2)) || (! myStrNCmp("-Random", tokens[2], 2)) )
		return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
	}
	else if (! myStrNCmp("-Array", tokens[0], 2))
	{
	    if (! myStrNCmp("-Array", tokens[1], 2))
		return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
	    else if (! myStrNCmp("-Index", tokens[1], 2))
	    {
		if (!((myStr2Int(tokens[2], id)) && (id >= 0)))
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
		else if ((! myStrNCmp("-Index", tokens[3], 2)) || (! myStrNCmp("-Random", tokens[3], 2)) || (! myStrNCmp("-Array", tokens[3], 2)))
		    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);
	    }
	    else if (! myStrNCmp("-Random", tokens[1], 2))
	    {
		if (!((myStr2Int(tokens[2], id)) && (id > 0)))
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);	
		else if ((! myStrNCmp("-Index", tokens[3], 2)) || (! myStrNCmp("-Random", tokens[3], 2)) || (! myStrNCmp("-Array", tokens[3], 2)))
		    return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
		else
		    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[3]);	
	    }
	    else
		return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
	}
	else
	    return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   else
	return CmdExec::errorOption(CMD_OPT_MISSING, "");


   if (! isarray)
   {
	if (israndom)
	{
	    for (size_t i=0; i<id; ++i)
		mtest.deleteObj(rnGen(mtest.getObjListSize()));
	}
	else
	    mtest.deleteObj(id);
   }
   else
   {
	if (israndom)
	{
	    for (size_t i=0; i<id; ++i)
		mtest.deleteArr(rnGen(mtest.getArrListSize()));
	}
	else
	    mtest.deleteArr(id);
   }


   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


