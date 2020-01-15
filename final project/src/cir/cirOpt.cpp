/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static variables and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   IdList DFSList;
   UpdateDFSList(DFSList);
   for (vector<unsigned>::const_iterator iter = total_gate_id.begin()+PI_size+PO_size+1; iter != total_gate_id.end() ;)
   {
	Map_iter mapped = Gates_map.find(*iter);
	if (!(*mapped).second->InDFS())
	{
	    cout << "Sweeping: AIG(" << *iter << ") removed..." << endl;
	    iter = total_gate_id.erase(iter);
	    (*mapped).second->changefaningate(false);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	    --AND_size;
	}
	else
	    ++ iter;
   }
   for (vector<unsigned>::const_iterator iter = Undefined_id.begin(); iter != Undefined_id.end() ;)
   {
	Map_iter mapped = Gates_map.find(*iter);
	if (!(*mapped).second->InDFS())
	{
	    cout << "Sweeping: UNDEF(" << *iter << ") removed..." << endl;
	    iter = Undefined_id.erase(iter);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	}
	else
	    ++ iter;
   }

   CirGate::setglobalref();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   IdList DFSList;
   UpdateDFSList(DFSList);
   Map_iter constant_0 = Gates_map.find(0);
   for (vector<unsigned>::const_iterator iter = DFSList.begin(); iter != DFSList.end() ;iter++)
   {
	Map_iter mapped = Gates_map.find(*iter);
	if (check_constant((*mapped).second, (*constant_0).second))
	{
	    cout << "Simplifying: 0 merging " << *iter << "..." << endl;
	    vector<unsigned>::iterator it = find(total_gate_id.begin(), total_gate_id.end(), *iter);
	    total_gate_id.erase(it);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	    --AND_size;
	}
	else if (check_trivial((*mapped).second, (*constant_0).second))
	{
	    cout << "Simplifying: X merging " << *iter << "..." << endl;
	    vector<unsigned>::iterator it = find(total_gate_id.begin(), total_gate_id.end(), *iter);
	    total_gate_id.erase(it);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	    --AND_size;
	}
   }
   for (vector<unsigned>::const_iterator iter = Undefined_id.begin(); iter != Undefined_id.end() ;)
   {
	Map_iter mapped = Gates_map.find(*iter);
	if (!(*mapped).second->hasfanout())
	{
	    cout << "Simplifying: X merging " << *iter << " (UNDEF)..." << endl;
	    iter = Undefined_id.erase(iter);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	}
	else
	    ++ iter;
   }
   CirGate::setglobalref();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

bool
CirMgr::check_constant(CirGate* aig, CirGate* constant_0)
{
   // check if it becomes const 0
   if (aig->matching_fanin(size_t(constant_0)))
   {
	aig->changefaningate(false);
	aig->changefanoutgate(size_t(constant_0));
	return true;
   }
   else if (aig->matching_fanin(size_t(constant_0)+1)) // const 1
   {
	aig->changefaningate(false);
	size_t first_in, second_in;
	aig->getfanin(first_in, second_in);
	if (first_in == size_t(constant_0)+1)
	    aig->changefanoutgate(second_in);
	else
	    aig->changefanoutgate(first_in);
	return true;
   }
   return false;
}

bool
CirMgr::check_trivial(CirGate* aig, CirGate* constant_0)
{
   // check if fanins are the same or inverse
   size_t first_in, second_in;
   aig->getfanin(first_in, second_in);
   if (first_in == second_in)
   {
	aig->changefaningate(false);
	aig->changefanoutgate(first_in);
	return true;
   }
   else if (aig->realgate(first_in) == aig->realgate(second_in)) // inverse
   {
	aig->changefaningate(false);
	aig->changefanoutgate(size_t(constant_0));
	return true;
   }
   return false;
}


void
CirMgr::UpdateDFSList(IdList& DFSList)
{
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->UpdateDFS(DFSList);
   }
}

