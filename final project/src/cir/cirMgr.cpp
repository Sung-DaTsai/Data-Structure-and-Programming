/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr() 
{
   for (Map_iter iter = Gates_map.begin(); iter != Gates_map.end(); ++ iter)
	delete (*iter).second; 
   for (Store_FEC_iter iter = FEC_group.begin(); iter != FEC_group.end(); ++ iter)
	delete (*iter);
   for (vector<PI_sim_vector*>::const_iterator iter = fail_pattern.begin(); iter != fail_pattern.end(); ++ iter)
	delete (*iter);
   Store_FEC().swap(FEC_group);
   vector<PI_sim_vector*>().swap(fail_pattern);
}



CirGate* CirMgr::getGate(unsigned gid) const 
{
   Map_iter mapped = Gates_map.find(gid);

   if (mapped != Gates_map.end())
	return (*mapped).second;

   return 0;
}


bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream input(fileName);
   if (!input) {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   input.getline(buf, 1024);
   errMsg = buf;
   // parsing first line
   readfirstline();
   lineNo = 1;

   for (unsigned i=0; i<PI_size; ++i)
   {
	input.getline(buf, 1024);  // read PIs
	checkPIs();
   }

   unsigned temp_POs = maximum_var;
   for (unsigned i=0; i<PO_size; ++i)
   {
	input.getline(buf, 1024);  // read POs
	checkPOs(++temp_POs);
   }

   for (unsigned i=0; i<AND_size; ++i)
   {
	input.getline(buf, 1024);  // read AIGs
	checkAIGs();
   }

   unsigned nowsize = Undefined_pre.size();
   for (unsigned i=0; i<nowsize; ++i)
   {
	Map_iter mapped = Gates_map.find (Undefined_pre.at(i).second/2);

  	if ( mapped == Gates_map.end() )
	{
	    CirGate* undef = new UNDEFgate(0, Undefined_pre.at(i).second/2);
	    Gates_map[Undefined_pre.at(i).second/2] = undef;
    	    Undefined_pre.at(i).first->setfanin(size_t(undef) + Undefined_pre.at(i).second%2);
	    undef->setfanout(size_t(Undefined_pre.at(i).first) + Undefined_pre.at(i).second%2);
	    Undefined_id.push_back(Undefined_pre.at(i).second/2);
	}
  	else
	{
	    (*mapped).second->setfanout(size_t(Undefined_pre.at(i).first) + Undefined_pre.at(i).second%2);
	    Undefined_pre.at(i).first->setfanin(size_t((*mapped).second) + Undefined_pre.at(i).second%2);
	}
   }
   vector< pair < CirGate*, unsigned > >().swap(Undefined_pre);

   while (input.getline(buf, 1024))
   {
	errMsg = buf;
	if (buf[0] == 'c')
	    break;
	readsymbol();
   }


   return true;
}


void
CirMgr::readfirstline()
{
   size_t begin = 3;
   string temp_str;

   begin = myStrGetTok(errMsg, temp_str, begin);  // Maximum variable index
   maximum_var = static_cast<unsigned>(atoi(temp_str.c_str()));

   begin = myStrGetTok(errMsg, temp_str, begin);  // inputs
   PI_size = static_cast<unsigned>(atoi(temp_str.c_str()));

   begin = myStrGetTok(errMsg, temp_str, begin);  // latches
   begin = myStrGetTok(errMsg, temp_str, begin);  // outputs
   PO_size = static_cast<unsigned>(atoi(temp_str.c_str()));

   begin = myStrGetTok(errMsg, temp_str, begin);  // ands
   AND_size = static_cast<unsigned>(atoi(temp_str.c_str()));

   Gates_map.reserve(PI_size + AND_size + 1);
   total_gate_id.reserve(PI_size + PO_size + AND_size + 1);
   Undefined_pre.reserve(PO_size + AND_size*2);
   FEC_group.reserve(AND_size + 1);
   //fail_pattern.resize(PI_size);
   Gates_map[0] = new CONSTgate();
   total_gate_id.push_back(0);
}

bool
CirMgr::checkPIs()
{
   errMsg = buf;
   string temp_str;
   // size_t end = myStrGetTok(errMsg, temp_str);
   myStrGetTok(errMsg, temp_str);
   int PI_index;
   myStr2Int(temp_str, PI_index);
   Gates_map[PI_index / 2] = new PIgate(lineNo+1, PI_index / 2);
   total_gate_id.push_back(PI_index / 2);
   ++ lineNo;
   return true;
}

bool
CirMgr::checkPOs(unsigned& temp_PO)
{
   errMsg = buf;
   string temp_str;

   myStrGetTok(errMsg, temp_str);
   int PO_index;
   myStr2Int(temp_str, PO_index);
   CirGate* temp_gate = new POgate(lineNo + 1, temp_PO);
   Gates_map[temp_PO] = temp_gate;
   total_gate_id.push_back(temp_PO);

   Undefined_pre.push_back(pair< CirGate*, unsigned >(temp_gate, PO_index));
   ++ lineNo;
   return true;
}

bool
CirMgr::checkAIGs()
{
   errMsg = buf;
   string temp_str;

   int vari, fanin1, fanin2;

   size_t end = myStrGetTok(errMsg, temp_str);
   myStr2Int(temp_str, vari);

   end = myStrGetTok(errMsg, temp_str, end);
   myStr2Int(temp_str, fanin1);

   end = myStrGetTok(errMsg, temp_str, end);
   myStr2Int(temp_str, fanin2);

   vari = vari / 2;

   CirGate* temp_gate = new ANDgate(lineNo+1, vari);
   Gates_map[vari] = temp_gate;
   total_gate_id.push_back(vari);

   Undefined_pre.push_back(pair< CirGate*, unsigned >(temp_gate, fanin1));

   Undefined_pre.push_back(pair< CirGate*, unsigned >(temp_gate, fanin2));
   ++ lineNo;
   return true;
}


bool
CirMgr::readsymbol()
{
    errMsg = buf;
    string temp_str;

    size_t numofpipo = 1;
    int pos;
    numofpipo = myStrGetTok(errMsg, temp_str, numofpipo);

    myStr2Int(temp_str, pos);

    temp_str = errMsg.substr(numofpipo+1);

    if (buf[0] == 'i')
    {
	Map_iter mapped = Gates_map.find (total_gate_id.at(static_cast<unsigned>(pos)+1));
	(*mapped).second->setsymbol(temp_str);
    }
    else if (buf[0] == 'o')
    {
	Map_iter mapped = Gates_map.find (total_gate_id.at(static_cast<unsigned>(pos)+1+PI_size));
	(*mapped).second->setsymbol(temp_str);
    }
    ++ lineNo;
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << std::right << setw(12) << PI_size << endl;
   cout << "  PO" << std::right << setw(12) << PO_size << endl;
   cout << "  AIG" << std::right << setw(11) << AND_size << endl;
   cout << "------------------" << endl;
   cout << "  Total" << std::right << setw(9) << PI_size + PO_size + AND_size << endl;
}

void
CirMgr::printNetlist() const
{
/*
   cout << endl;
   for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
   }
*/
   cout << endl;
   size_t index = 0;
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->getGateInfo(index);
   }

   CirGate::setglobalref();
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (unsigned i=1; i<PI_size+1; ++i)
	cout << " " << total_gate_id.at(i);
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
	cout << " " << total_gate_id.at(i);
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   IdList floating;
   IdList notused;
   for (unsigned i=PI_size+1; i<total_gate_id.size(); ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if (! (*mapped).second->isfanindef())
	    floating.push_back(total_gate_id.at(i));
   }
   if (floating.size() != 0)
	cout << "Gates with floating fanin(s):";
   for (unsigned i=0; i<floating.size(); ++i)
   {
	for (unsigned j=i+1; j<floating.size(); ++j)
	{
	    if (floating.at(i) > floating.at(j))
	    {
		unsigned temp = floating.at(i);
		floating.at(i) = floating.at(j);
		floating.at(j) = temp;
	    }
	}
	cout << " " << floating.at(i);
   }
   if (floating.size() != 0)
   	cout << endl;
   unsigned loop_i;
   if (PI_size == 0)
	loop_i = PO_size + 1;
   else
	loop_i = 1;
   for (unsigned i=loop_i; i<total_gate_id.size(); ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if (!(*mapped).second->hasfanout())
	    notused.push_back(total_gate_id.at(i));
	if (i == PI_size)
	    i = PI_size + PO_size;
   }
   if (notused.size() != 0)
	cout << "Gates defined but not used  :";
   for (unsigned i=0; i<notused.size(); ++i)
   {
	for (unsigned j=i+1; j<notused.size(); ++j)
	{
	    if (notused.at(i) > notused.at(j))
	    {
		unsigned temp = notused.at(i);
		notused.at(i) = notused.at(j);
		notused.at(j) = temp;
	    }
	}
	cout << " " << notused.at(i);
   }
   if (notused.size() != 0)
	cout << endl;
}

bool sortingbyvar(vector<unsigned>* input1, vector<unsigned>* input2) 
{ return input1->at(0) < input2->at(0); } 


void
CirMgr::printFECPairs() const
{
   vector < vector <unsigned>* > print_var;
   Map_iter mapped = Gates_map.find(0);
   CirGate* dummy = (*mapped).second;
   for (Store_FEC_iter iter = FEC_group.begin(); iter != FEC_group.end(); iter++)
   {
	bool opposite = false;
	if (dummy->isInverse((*iter)->at(0)))
	    opposite = true;
	vector<unsigned>* temp_var = new vector<unsigned>;
	for (vector<size_t>::const_iterator variables = (*iter)->begin(); variables != (*iter)->end(); variables++)
	{
	    if (!opposite)
	    {
	    	if (!dummy->isInverse(*variables))
	    	    temp_var->push_back(dummy->realgate(*variables)->getvar()*2);
	    	else
	    	    temp_var->push_back(dummy->realgate(*variables)->getvar()*2 + 1);
	    }
	    else
	    {
	    	if (dummy->isInverse(*variables))
	    	    temp_var->push_back(dummy->realgate(*variables)->getvar()*2);
	    	else
	    	    temp_var->push_back(dummy->realgate(*variables)->getvar()*2 + 1);		
	    }
	}
	print_var.push_back(temp_var);
   }
   sort(print_var.begin(), print_var.end(), sortingbyvar);
   unsigned j = 0;
   for (vector < vector <unsigned>* >::const_iterator iter = print_var.begin(); iter != print_var.end();)
   {
	cout << "[" << j++ << "]";
	for (vector<unsigned>::const_iterator variables = (*iter)->begin(); variables != (*iter)->end(); variables++)
	{
	    cout << " ";
	    if ((*variables) % 2 == 1)
		cout << "!";
	    cout << (*variables)/2;
	}
	cout << endl;
	delete *iter;
	iter = print_var.erase(iter);
   }

}


void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << maximum_var << " " << PI_size << " 0 " << PO_size << " ";
   size_t num_aig = 0;
   bool printing = false;
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->writeoutInfo(outfile, num_aig, printing);
   }
   outfile << num_aig << endl;
   CirGate::setglobalref();
   printing = true;
   for (unsigned i=1; i<=PI_size; ++i)
	outfile << total_gate_id.at(i)*2 << endl;
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->getFaninindex(outfile);
   }
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->writeoutInfo(outfile, num_aig, printing);
   }

   CirGate::setglobalref();
   for (unsigned i=1; i<=PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if (! (*mapped).second->getsymbol().empty())
	    outfile << "i" << i-1 << " " << (*mapped).second->getsymbol() << endl;
   }
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if (! (*mapped).second->getsymbol().empty())
	    outfile << "o" << i-1-PI_size << " " << (*mapped).second->getsymbol() << endl;
   }
   outfile << "c" << endl;
   outfile << "Kopingchen Of First" << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   unsigned PI_pass = 0;
   size_t num_aig = 0;
   bool printing = false;
   g->writeoutInfo(outfile, num_aig, printing);
   for (unsigned i=1; i<=PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if ((*mapped).second->InDFS())
	    ++ PI_pass;
   }
   outfile << "aag " << g->getvar() << " " << PI_pass << " 0 1 " << num_aig << endl;
   printing = true;
   for (unsigned i=1; i<=PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if ((*mapped).second->InDFS())
	    outfile << total_gate_id.at(i)*2 << endl;
   }
   CirGate::setglobalref();
   outfile << g->getvar()*2 << endl;
   g->writeoutInfo(outfile, num_aig, printing);

   unsigned k = 0;
   for (unsigned i=1; i<=PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	if ((! (*mapped).second->getsymbol().empty())&&((*mapped).second->InDFS()))
	    outfile << "i" << k++ << " " << (*mapped).second->getsymbol() << endl;
   }
   CirGate::setglobalref();
   outfile << "o0 " << g->getvar() << endl;
   outfile << "c" << endl;
   outfile << "Kopingchen Of First" << endl;

}

