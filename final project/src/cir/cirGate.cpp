/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/

unsigned CirGate::_globalref = 1;

void
CirGate::reportGate() const
{
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}


void 
CirGate::printGate(int level, int space, bool inverse, bool isfanin) const
{
}

void
CirGate::getFaninindex(ostream& outfile) const
{
   if (isInverse(fanin.at(0)))
	outfile << realgate(fanin.at(0))->getvar()*2 + 1 << endl;
   else
	outfile << realgate(fanin.at(0))->getvar()*2 << endl;
}

void 
CirGate::changefaningate(const bool& replace_fanout)  // only erase here, not replacing
{
   realgate(fanin.at(0))->erasefanout(this);
   realgate(fanin.at(1))->erasefanout(this);
   /*if (replace_fanout) // add fanouts of mine into fanin gates
   {
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
   	{
	    if (isInverse(fanin.at(0)) == isInverse(*iter))
		realgate(fanin.at(0))->setfanout(size_t(realgate(*iter)));
	    else
		realgate(fanin.at(0))->setfanout(size_t(realgate(*iter))+1);
	    if (isInverse(fanin.at(1)) == isInverse(*iter))
		realgate(fanin.at(1))->setfanout(size_t(realgate(*iter)));
	    else
		realgate(fanin.at(1))->setfanout(size_t(realgate(*iter))+1);
	}
   }*/
}

void
CirGate::getfanin(size_t& a, size_t& b)
{
   a = fanin.at(0);
   b = fanin.at(1);
}

void 
CirGate::changefanoutgate(size_t replacing)  // replace fanouts with new fanin and link new fanin to fanouts
{
   for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	realgate(*iter)->replacefanin(this, replacing);
}

void 
CirGate::replacefanin(CirGate* oldgate, size_t newgate)
{
   for (unsigned i=0; i<fanin.size(); ++i)
   {
	if (realgate(fanin.at(i)) == oldgate)
	{
	    if (isInverse(fanin.at(i)) == isInverse(newgate))
	    {
		fanin.at(i) = size_t(realgate(newgate));
		realgate(newgate)->setfanout(size_t(this));
	    }
	    else
	    {
		fanin.at(i) = size_t(realgate(newgate))+1;
		realgate(newgate)->setfanout(size_t(this)+1);
	    }
	}
   }
}

void 
CirGate::erasefanout(CirGate* oldgate)
{
   for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end();)
   {
	if (realgate(*iter) == oldgate)
	    iter = fanout.erase(iter);
	else
	    ++ iter;
   }
}

void
CirGate::reportFECs() const
{
   cout << "= FECs:";
   if (FEC_gates == 0)
   {
	cout << endl;
	return;
   }
   vector<size_t> FECs;
   bool opposite = false;
   for (vector<size_t>::const_iterator iter = FEC_gates->begin(); iter != FEC_gates->end(); iter++)
   {
	if (realgate(*iter) == this)
	{
	    if (isInverse(*iter))
		opposite = true;
	    continue;
	}
	else if (!isInverse(*iter))
	    FECs.push_back(realgate(*iter)->getvar()*2);
	else
	    FECs.push_back(realgate(*iter)->getvar()*2 + 1);
   }
   sort(FECs.begin(), FECs.end());
   for (vector<size_t>::const_iterator iter = FECs.begin(); iter != FECs.end(); iter++)
   {
	cout << " ";
	if (opposite)
	{
	    if ((*iter) % 2 == 0)
	    	cout << "!";
	    cout << (*iter)/2;
	}
	else
	{
	    if ((*iter) % 2 == 1)
	    	cout << "!";
	    cout << (*iter)/2;
	}
   }
   cout << endl;
}
/**************************************/
/*   class ANDgate member functions   */
/**************************************/
void
ANDgate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= AIG(" << to_string(getvar()) << "), line " << to_string(getLineNo()) << endl;
   reportFECs();
   cout << "= Value: ";
   for (int i=63; i>=0; --i)
   {
	cout << sim_value[i];
	if ((i%8 == 0) && (i != 0))
	    cout << "_";
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
ANDgate::reportFanin(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, true);
   setglobalref();
}

void
ANDgate::reportFanout(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, false);
   setglobalref();
}

void 
ANDgate::printGate(int level, int space, bool inverse, bool isfanin) const
{
   for (int i=0; i<space; ++i)
	cout << " ";
   if (inverse)  cout << "!";
   cout << "AIG " << getvar();

   if ((_ref == _globalref) && (level != 0))
   {
	cout << " (*)" << endl;
	return;
   }
   else if (level == 0)
   {
	cout << endl;
	return;
   }
   else
	cout << endl;
   if (isfanin)
   {
	realgate(fanin.at(0))->printGate(level-1, space+2, isInverse(fanin.at(0)), true);
	realgate(fanin.at(1))->printGate(level-1, space+2, isInverse(fanin.at(1)), true);
	resetref();
   }
   else
   {
	for (unsigned i=0; i<fanout.size(); ++i)
	{
	    realgate(fanout.at(i))->printGate(level-1, space+2, isInverse(fanout.at(i)), false);
	    resetref();
	}
   }
}


/**************************************/
/*   class PIgate member functions    */
/**************************************/
void
PIgate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= PI(" << to_string(getvar()) << ")";
   if (getsymbol().size() != 0)
	cout << "\"" << getsymbol() << "\"";
   cout << ", line " << to_string(getLineNo()) << endl;
   cout << "= FECs:" << endl;
   cout << "= Value: ";
   for (int i=63; i>=0; --i)
   {
	cout << sim_value[i];
	if ((i%8 == 0) && (i != 0))
	    cout << "_";
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
PIgate::reportFanin(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, true);
   setglobalref();
}

void
PIgate::reportFanout(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, false);
   setglobalref();
}

void 
PIgate::printGate(int level, int space, bool inverse, bool isfanin) const
{
   for (int i=0; i<space; ++i)
	cout << " ";
   if (inverse)  cout << "!";
   cout << "PI " << getvar() << endl;
   if ((level == 0) || (isfanin))  return;
   resetref();
   if (! isfanin)
   {
	for (unsigned i=0; i<fanout.size(); ++i)
	    realgate(fanout.at(i))->printGate(level-1, space+2, isInverse(fanout.at(i)), false);
   }

}
/**************************************/
/*   class POgate member functions    */
/**************************************/
void
POgate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= PO(" << to_string(getvar()) << ")";
   if (getsymbol().size() != 0)
	cout << "\"" << getsymbol() << "\"";
   cout << ", line " << to_string(getLineNo()) << endl;
   cout << "= FECs:" << endl;
   cout << "= Value: ";
   for (int i=63; i>=0; --i)
   {
	cout << sim_value[i];
	if ((i%8 == 0) && (i != 0))
	    cout << "_";
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
POgate::reportFanin(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, true);
   setglobalref();
}

void
POgate::reportFanout(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, false);
   setglobalref();
}

void 
POgate::printGate(int level, int space, bool inverse, bool isfanin) const
{
   for (int i=0; i<space; ++i)
	cout << " ";
   if (inverse)  cout << "!";
   cout << "PO " << getvar() << endl;
   if ((level == 0) || (!isfanin))  return;
   resetref();

   realgate(fanin.at(0))->printGate(level-1, space+2, isInverse(fanin.at(0)), true);
}


/**************************************/
/*   class CONSTgate member functions */
/**************************************/
void
CONSTgate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= CONST(0), line 0" << endl;
   reportFECs();
   cout << "= Value: ";
   for (int i=63; i>=0; --i)
   {
	cout << sim_value[i];
	if ((i%8 == 0) && (i != 0))
	    cout << "_";
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
CONSTgate::reportFanin(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, true);
   setglobalref();
}

void
CONSTgate::reportFanout(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, false);
   setglobalref();
}

void 
CONSTgate::printGate(int level, int space, bool inverse, bool isfanin) const
{
   for (int i=0; i<space; ++i)
	cout << " ";
   if (inverse)  cout << "!";
   cout << "CONST 0" << endl;
   if ((level != 0) && (!isfanin))
   {
	for (unsigned i=0; i<fanout.size(); ++i)
	    realgate(fanout.at(i))->printGate(level-1, space+2, isInverse(fanout.at(i)), false);
   }
}


/**************************************/
/*   class UNDEFgate member functions */
/**************************************/
void
UNDEFgate::reportGate() const
{
   cout << "================================================================================" << endl;
   cout << "= UNDEF(" << to_string(getvar()) << "), line 0" << endl;
   cout << "= FECs:" << endl;
   cout << "= Value: ";
   for (int i=63; i>=0; --i)
   {
	cout << sim_value[i];
	if ((i%8 == 0) && (i != 0))
	    cout << "_";
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
UNDEFgate::reportFanin(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, true);
   setglobalref();
}

void
UNDEFgate::reportFanout(int level) const
{
   assert (level >= 0);
   printGate(level, 0, false, false);
   setglobalref();
}

void 
UNDEFgate::printGate(int level, int space, bool inverse, bool isfanin) const
{
   for (int i=0; i<space; ++i)
	cout << " ";
   if (inverse)  cout << "!";
   cout << "UNDEF " << getvar() << endl;
   if ((level == 0) || (isfanin))  return;
   resetref();
   if (! isfanin)
   {
	for (unsigned i=0; i<fanout.size(); ++i)
	    realgate(fanout.at(i))->printGate(level-1, space+2, isInverse(fanout.at(i)), false);
   }

}
