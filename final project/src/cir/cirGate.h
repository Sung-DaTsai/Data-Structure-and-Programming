/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
# define NEG 0x1
class CirGate
{
public:
   CirGate(unsigned linenumber, unsigned var): _LineNo(linenumber), _var(var), _ref(0), _symbol(""), sim_value(0), FEC_gates(0) {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return _LineNo; }
   string getsymbol() const { return _symbol; }
   unsigned getvar() const { return _var; }
   virtual void getGateInfo(size_t &) const {}
   virtual void writeoutInfo(ostream &, size_t &, bool &) const {}
   virtual void UpdateDFS(IdList& DFSList) const { resetref(); }
   virtual bool isfanindef() const { return false; }
   virtual bool hasfanout() const { return false; }
   virtual bool InDFS() const { if (_ref == _globalref) return true; else return false; }
   virtual bool isUNDEF() const { return false; }
   void getfanin(size_t&, size_t&);

   virtual bool isAig() const { return false; }

   // bit slicing methods
   bool isInverse(size_t address) const { return address & NEG; }
   CirGate* realgate(size_t address) const { if(isInverse(address)) return (CirGate*)((void*)(address - 1)); else return (CirGate*)((void*)(address)); }
   // set method
   bool setsymbol(string symbol) { if (_symbol != "") return false; _symbol = symbol; return true; }
   void setfanin(size_t fanins) { if (fanin.at(0) == 0) { fanin.at(0) = fanins; } 
                                  else if (fanin.size() == 2) { fanin.at(1) = fanins;} }
   //void setfaninsecond(size_t fanins) { fanin.at(1) = fanins; secondfanin = true; }
   void setfanout(size_t fanouts) { fanout.push_back(fanouts); }
   //void setfaninfailed(size_t fanins) { if (fanin.at(0) == 0) fanin.at(0) = fanins; 
	//				else if (fanin.size() == 2) fanin.at(1) = fanins; }
   static void setglobalref() { ++_globalref; }
   void resetref() const { _ref = _globalref; }
   // Printing functions
   virtual void printGate(int, int, bool, bool) const;
   virtual void reportGate() const;
   virtual void reportFanin(int level) const;
   virtual void reportFanout(int level) const;
   void getFaninindex(ostream&) const;  // print fanout's fanin index
   
   // Erase related methods
   void changefaningate(const bool&);
   void erasefanout(CirGate*);
   void changefanoutgate(size_t);
   void replacefanin(CirGate*, size_t);
   bool matching_fanin(size_t gate) { for (unsigned i=0; i<fanin.size(); ++i) { if (fanin.at(i) == gate) return true; } 				      return false; }

   // Simulation related methods
   virtual void setSimValue(const Parallel_sim&) {}
   Parallel_sim getSimValue() const { return sim_value; }
   Parallel_sim getSim(size_t gate) const { if (isInverse(gate)) return ~realgate(gate)->getSimValue(); 
					    else return realgate(gate)->getSimValue(); }
   void setFECpartner(vector<size_t>* fecs) { FEC_gates = fecs; }
   void resetFECpartner() { FEC_gates = 0; }
   void reportFECs() const;
   //bool sortingbyvar(size_t input1, size_t input2) 
   //{ return (realgate(input1)->getvar() < realgate(input2)->getvar()); } 


   // Fraig related functions
   void setSatVar(const Var& v) { satvar = v; }
   Var getSatVar() const { return satvar; }
   void addAIGCNF(SatSolver& s) const 
   {    
	s.addAigCNF(satvar, realgate(fanin.at(0))->getSatVar(), isInverse(fanin.at(0)), 
		    realgate(fanin.at(1))->getSatVar(), isInverse(fanin.at(1))); 
   }
   bool hasFECpair() const { if (FEC_gates != 0) return true; else return false; }

   vector<size_t> * getFECpairs() const { return FEC_gates; }

   virtual void UpdateBFS(IdList& BFSList) const { resetref(); }
private:

protected:
   vector<size_t> fanin;  // use bit slicing
   vector<size_t> fanout;
   static unsigned _globalref;
   unsigned _LineNo;
   unsigned _var;
   mutable unsigned _ref;
   string _symbol;
   Parallel_sim sim_value;
   vector<size_t> * FEC_gates;
   Var satvar;
};

class ANDgate
: public CirGate
{
public:
   ANDgate(unsigned linenumber, unsigned var): CirGate(linenumber, var) { fanin.push_back(0); fanin.push_back(0); }
   ~ANDgate() {}
   bool isfanindef() const { if ((realgate(fanin.at(0))->isUNDEF()) || (realgate(fanin.at(1))->isUNDEF())) return false; 
			   else return true; }
   bool hasfanout() const { if (fanout.size() != 0) return true; else return false; }
   void printGate(int, int, bool, bool) const;
   void reportGate() const;
   void reportFanin(int level) const;//{ cout << "self" << endl; cout << "next one" << endl; fanin->printGate();}
   void reportFanout(int level) const;
   bool isAig() const { return true; }
   void getGateInfo(size_t &index) const 
   {
	if (_ref == _globalref) return;

	realgate(fanin.at(0))->getGateInfo(index);
	realgate(fanin.at(1))->getGateInfo(index); 

	cout << "[" << index << "] AIG " << _var << " ";

	if (realgate(fanin.at(0))->isUNDEF())  cout << "*";
	if (isInverse(fanin.at(0)))  cout << "!";
	cout << realgate(fanin.at(0))->getvar() << " ";


	if (realgate(fanin.at(1))->isUNDEF())  cout << "*";
	if (isInverse(fanin.at(1)))  cout << "!";
	cout << realgate(fanin.at(1))->getvar() << endl;

	++index;
	resetref();
   }
   void writeoutInfo(ostream& outfile, size_t& total_aig, bool& printing) const 
   {
	if (_ref == _globalref) return;
	if (!printing) 
	{ 
	    ++ total_aig;
	    realgate(fanin.at(0))->writeoutInfo(outfile, total_aig, printing);
	    realgate(fanin.at(1))->writeoutInfo(outfile, total_aig, printing); 
	    resetref();
	    return;
	}
	realgate(fanin.at(0))->writeoutInfo(outfile, total_aig, printing);
	realgate(fanin.at(1))->writeoutInfo(outfile, total_aig, printing); 
	outfile << _var*2 << " ";

	if (isInverse(fanin.at(0)))
	    outfile << realgate(fanin.at(0))->getvar()*2 + 1 << " ";
	else
	    outfile << realgate(fanin.at(0))->getvar()*2 << " ";

	if (isInverse(fanin.at(1)))
	    outfile << realgate(fanin.at(1))->getvar()*2 + 1 << endl;
	else
	    outfile << realgate(fanin.at(1))->getvar()*2 << endl;

	resetref();
   }
   void UpdateDFS(IdList& DFSList) const
   {
	if (_ref == _globalref) return;
	realgate(fanin.at(0))->UpdateDFS(DFSList);
	realgate(fanin.at(1))->UpdateDFS(DFSList);
	DFSList.push_back(_var);
	resetref();
   }

   void setSimValue(const Parallel_sim& fanin_value)
   {
	if (_ref == _globalref)
	{
	    fanin2 = fanin_value;
	    sim_value = fanin1 & fanin2;
	    for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	    {
		if (isInverse(*iter))   realgate(*iter)->setSimValue(~sim_value);
		else			realgate(*iter)->setSimValue(sim_value);		    
	    }
	    return;
	}
	fanin1 = fanin_value;
	resetref();
   }

   void UpdateBFS(IdList& BFSList) const
   {
	if (_ref == _globalref) return;
	BFSList.push_back(_var);
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	    realgate(*iter)->UpdateBFS(BFSList);
	resetref();
   }
private:
   Parallel_sim fanin1;
   Parallel_sim fanin2;
};

class PIgate
: public CirGate
{
public:
   PIgate(unsigned linenumber, unsigned var): CirGate(linenumber, var) {}
   ~PIgate() {}
   bool hasfanout() const { if (fanout.size() != 0) return true; else return false; }
   void printGate(int, int, bool, bool) const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void getGateInfo(size_t &index) const 
   {
	if (_ref == _globalref) return;
	cout << "[" << index << "] PI  " << _var;
	if (_symbol.size() != 0)
	    cout << " (" << _symbol << ")";
	cout << endl;
	++index;
	resetref();
   }
   void writeoutInfo(ostream& outfile, size_t& total_aig, bool& printing) const 
   {
	resetref();
   }
   void setSimValue(const Parallel_sim& fanin_value)
   {
	if (_ref == _globalref)  return;
	sim_value = fanin_value;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	{
	    if (isInverse(*iter))   realgate(*iter)->setSimValue(~sim_value);
	    else		    realgate(*iter)->setSimValue(sim_value);		    
	}
	resetref();
   }
   void UpdateBFS(IdList& BFSList) const
   {
	if (_ref == _globalref) return;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	    realgate(*iter)->UpdateBFS(BFSList);
	resetref();
   }
private:

};

class POgate
: public CirGate
{
public:
   POgate(unsigned linenumber, unsigned var): CirGate(linenumber, var) { fanin.push_back(0); }
   ~POgate() {}
   bool isfanindef() const { if (realgate(fanin.at(0))->isUNDEF()) return false; else return true; }
   void printGate(int, int, bool, bool) const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void getGateInfo(size_t &index) const 
   {
	if (_ref == _globalref) return;
	realgate(fanin.at(0))->getGateInfo(index);
	cout << "[" << index << "] PO  " << _var << " ";

	if (realgate(fanin.at(0))->isUNDEF())  cout << "*";
	if (isInverse(fanin.at(0)))  cout << "!";
	cout << realgate(fanin.at(0))->getvar();

	if (_symbol.size() != 0)
	    cout << " (" << _symbol << ")";
	cout << endl;
	++index;
	resetref();
   }
   void writeoutInfo(ostream& outfile, size_t& total_aig, bool& printing) const 
   {
	if (_ref == _globalref) return;
	if (! printing)
	{
	    realgate(fanin.at(0))->writeoutInfo(outfile, total_aig, printing);
	    resetref();
	    return;
	}
	realgate(fanin.at(0))->writeoutInfo(outfile, total_aig, printing);
	resetref();
   }
   void UpdateDFS(IdList& DFSList) const
   {
	if (_ref == _globalref) return;
	realgate(fanin.at(0))->UpdateDFS(DFSList);
	resetref();
   }

   void setSimValue(const Parallel_sim& fanin_value)
   {
	if (_ref == _globalref)  return;
	sim_value = fanin_value;
	resetref();
   }

private:

};

class CONSTgate
: public CirGate
{
public:
   CONSTgate(): CirGate(0, 0) {}
   ~CONSTgate() {}
   bool hasfanout() const { return true; }
   void printGate(int, int, bool, bool) const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void getGateInfo(size_t &index) const { if (_ref == _globalref) return; cout << "[" << index << "] CONST0" << endl; 
   ++index;  resetref(); }
   bool InDFS() const { return true; }
   void setSimValue(const Parallel_sim& fanin_value)
   {
	if (_ref == _globalref)  return;
	sim_value = fanin_value;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	{
	    if (isInverse(*iter))   realgate(*iter)->setSimValue(~sim_value);
	    else		    realgate(*iter)->setSimValue(sim_value);		    
	}
	resetref();
   }
   void UpdateBFS(IdList& BFSList) const
   {
	if (_ref == _globalref) return;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	    realgate(*iter)->UpdateBFS(BFSList);
	resetref();
   }
};

class UNDEFgate
: public CirGate
{
public:
   UNDEFgate(unsigned linenumber, unsigned var): CirGate(linenumber, var) {}
   ~UNDEFgate() {}
   bool hasfanout() const { if (!fanout.empty()) return true; else return false; }
   void printGate(int, int, bool, bool) const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void getGateInfo(size_t &index) const { resetref(); }
   bool isUNDEF() const { return true; }
   void setSimValue(const Parallel_sim& fanin_value)
   {
	if (_ref == _globalref)  return;
	sim_value = fanin_value;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	{
	    if (isInverse(*iter))   realgate(*iter)->setSimValue(~sim_value);
	    else		    realgate(*iter)->setSimValue(sim_value);		    
	}
	resetref();
   }
   void UpdateBFS(IdList& BFSList) const
   {
	if (_ref == _globalref) return;
	for (vector<size_t>::const_iterator iter = fanout.begin(); iter != fanout.end(); iter++)
	    realgate(*iter)->UpdateBFS(BFSList);
	resetref();
   }
private:

};

struct Fanin { 
   size_t fanin1, fanin2;  
  
   Fanin(CirGate* gate) 
   { 
	fanin1 = 0;
	fanin2 = 0;
	gate->getfanin(fanin1, fanin2);
   } 
  
   // hash table matching functions
   bool operator==(const Fanin& gate) const
   {
     return (((fanin1 == gate.fanin1) && (fanin2 == gate.fanin2)) || ((fanin2 == gate.fanin1) && (fanin1 == gate.fanin2)));
   } 
}; 
  
class HashFunction {
public:
    size_t operator()(const Fanin& gate) const
    {
	return (gate.fanin1 + gate.fanin2)/59;
    }
}; 


#endif // CIR_GATE_H
