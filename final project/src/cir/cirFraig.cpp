/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
typedef std::unordered_map< Fanin, CirGate*, HashFunction >  Strash_map;
typedef std::unordered_map< Fanin, CirGate*, HashFunction >::const_iterator  Strash_map_iter;
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   IdList DFSList;
   UpdateDFSList(DFSList);
   Strash_map hash_table;
   hash_table.reserve(AND_size);
   for (vector<unsigned>::const_iterator iter = DFSList.begin(); iter != DFSList.end() ; iter++)
   {
	Map_iter mapped = Gates_map.find(*iter);

	Fanin hash_key((*mapped).second);
	Strash_map_iter hash_iter = hash_table.find(hash_key);
	if (hash_iter != hash_table.end())  // need to do merging
	{
	    merging((*mapped).second, (*hash_iter).second);
	    cout << "Strashing: X merging " << *iter << "..." << endl;
	    vector<unsigned>::iterator it = find(total_gate_id.begin(), total_gate_id.end(), *iter);
	    total_gate_id.erase(it);
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	    --AND_size;
	}
	else
	    hash_table[hash_key] = (*mapped).second;

   }


   CirGate::setglobalref();
}

void
CirMgr::fraig()
{
   //IdList DFSList;
   //UpdateDFSList(DFSList);
   IdList DFSList;
   UpdateDFSList(DFSList);
   CirGate::setglobalref();

   SatSolver solver;
   solver.initialize();

   genProofModel(solver);

   Map_iter mapped;

   mapped = Gates_map.find(0);
   if ((*mapped).second->hasFECpair())
	proofFraig(solver, (*mapped).second);


   PI_sim_vector PI_sim, PO_sim;
   PI_sim.resize(PI_size);
   for (vector<PI_sim_vector*>::const_iterator iter=fail_pattern.begin(); iter!=fail_pattern.end(); iter++)
   {
	PI_sim = *(*iter);

	parallel_simulation(PI_sim, PO_sim);
	Update_FEC();
   }




   for (vector<unsigned>::const_iterator iter = DFSList.begin(); iter != DFSList.end() ; iter++)
   {
	mapped = Gates_map.find(*iter);
	if ((mapped == Gates_map.end()) || (!(*mapped).second->isAig()))
	    continue;
	if ((*mapped).second->hasFECpair())
	    proofFraig(solver, (*mapped).second);
   }

   for (Store_FEC_iter iter = FEC_group.begin(); iter != FEC_group.end(); ++ iter)
	delete (*iter);
   Store_FEC().swap(FEC_group);
   simulated = false;
   CirGate::setglobalref();
   strash();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::merging(CirGate* tobereplaced, CirGate* merge, const bool& inverse)
{
   tobereplaced->changefaningate(false);
   if (!inverse)
	tobereplaced->changefanoutgate(size_t(merge));
   else
	tobereplaced->changefanoutgate(size_t(merge)+1);
}


void
CirMgr::genProofModel(SatSolver& s)
{
   Map_iter mapped;
   // Allocate and record variables for Const 0, PIs and AIGs
   for (unsigned i = 0; i <= PI_size; ++i) 
   {
	Var v = s.newVar();
	mapped = Gates_map.find(total_gate_id.at(i));
        (*mapped).second->setSatVar(v);
   }
   for (unsigned i = PI_size + PO_size + 1; i <= PI_size + PO_size + AND_size; ++i) 
   {
	Var v = s.newVar();
	mapped = Gates_map.find(total_gate_id.at(i));
        (*mapped).second->setSatVar(v);
   }
   for (unsigned i = 0, n = Undefined_id.size(); i < n; ++i) 
   {
	Var v = s.newVar();
	mapped = Gates_map.find(Undefined_id.at(i));
        (*mapped).second->setSatVar(v);
   }
   for (unsigned i = PI_size + PO_size + 1; i <= PI_size + PO_size + AND_size; ++i) 
   {
	mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->addAIGCNF(s);
   }
}

void
CirMgr::proofFraig(SatSolver& solver, CirGate* aiggate)
{
   vector<size_t>* fec_pairs = aiggate->getFECpairs();
   //bool setlimits = false;
   //unsigned limitsize = 30;
   unsigned countlimit = 0;
   unsigned proving_effort = 300;
   if (fec_pairs->size() <= 1)
   {
	aiggate->resetFECpartner();
	return;
   }
   else if (aiggate->getvar() == 0) {}
   else if (fec_pairs->size() > proving_effort)
   {
	aiggate->resetFECpartner();
	return;
   }
   bool result;
   bool opposite = false;
   vector<size_t>::const_iterator iter;
   for (iter = fec_pairs->begin(); iter != fec_pairs->end(); iter++)
   {
	if (aiggate == aiggate->realgate(*iter))
	{
	    if (aiggate->isInverse(*iter))
		opposite = true;
	    fec_pairs->erase(iter);
	    aiggate->resetFECpartner();
	    break;
	}
   }

   if ((aiggate->realgate(fec_pairs->at(0))->getvar() == 0) && (aiggate->getvar() != 0))
   {
	iter = fec_pairs->begin()+1;
   }
   else
	iter = fec_pairs->begin();
   //cout << "Proof: " << aiggate->getvar() << endl;

   Map_iter constant0 = Gates_map.find(0);
   

   for ( ; iter != fec_pairs->end(); ++ countlimit)
   {
	//if (setlimits && countlimit > limitsize)
	    //break;
	Var newV = solver.newVar();
	//cout << "Proof: " << aiggate->getvar() << " v.s. " << aiggate->realgate(*iter)->getvar() << endl;
	if (aiggate->getvar() != 0)
	    solver.addXorCNF(newV, aiggate->getSatVar(), opposite, aiggate->realgate(*iter)->getSatVar(), aiggate->isInverse(*iter));
	else
	{
	    solver.addXorCNF(newV, aiggate->getSatVar(), opposite, aiggate->realgate(*iter)->getSatVar(), aiggate->isInverse(*iter));
	}    
	solver.assumeRelease();  // Clear assumptions
	solver.assumeProperty(newV, true);  // k = 1
	solver.assumeProperty((*constant0).second->getSatVar(), false);
	result = solver.assumpSolve();
	if (result)  // SAT, proven to be not equal
	{
	    if (aiggate->getvar() == 0)
		reportResult(solver, result);
	    ++ iter;
	}
	else  // UNSAT, proven to be equal
	{
	    if (opposite == aiggate->isInverse(*iter))
	        merging(aiggate->realgate(*iter), aiggate);
	    else
	        merging(aiggate->realgate(*iter), aiggate, true);
	    cout << "Fraig: " << aiggate->getvar() << " merging " << aiggate->realgate(*iter)->getvar() << "..." << endl;
	    vector<unsigned>::iterator it = find(total_gate_id.begin(), total_gate_id.end(), aiggate->realgate(*iter)->getvar());
	    total_gate_id.erase(it);
	    Map_iter mapped = Gates_map.find(aiggate->realgate(*iter)->getvar());
	    delete (*mapped).second;
	    Gates_map.erase(mapped);
	    --AND_size;
	    iter = fec_pairs->erase(iter);
	}
   }

}


void
CirMgr::reportResult(const SatSolver& solver, bool result)
{
   int nums;
   if (fail_pattern_num == 0)
   {
	PI_sim_vector* fail_PI = new PI_sim_vector;
	fail_pattern.push_back(fail_PI);
	fail_PI->resize(PI_size);
   }
   for (size_t i = 1; i <= PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	nums = solver.getValue((*mapped).second->getSatVar());
	if (nums == -1)
	    (*(fail_pattern.end()-1))->at(i-1)[fail_pattern_num] = 0;
	else
	    (*(fail_pattern.end()-1))->at(i-1)[fail_pattern_num] = nums;
   }
   if (fail_pattern_num == 63)
   {
	fail_pattern_num = 0;
   }
   else
	++ fail_pattern_num;
}

void
CirMgr::UpdateBFSList(IdList& BFSList)
{
   for (unsigned i=0; i<=PI_size; ++i)
   {
	Map_iter mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->UpdateBFS(BFSList);
   }
}
