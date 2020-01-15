/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <time.h>

using namespace std;

// TODO: Keep "CirMgr::randomSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   string pattern;
   PI_sim_vector PI_sim, PO_sim;
   PI_sim.resize(PI_size);
   PO_sim.resize(PO_size);
   unsigned pattern_num = 0;
   unsigned count_limit = 0;
   //unsigned limit_time = 150;
   //unsigned now_size, limit_size = 2*AND_size;
   srandom ( time(NULL) );
   while (true)
   {
   	//for (unsigned i=0; i<PI_size; ++i)
	    //PI_sim.at(i).reset();
   	/*for (unsigned i=0; i<64; ++i)
   	{
	    //cout << (rand() % 2);
	    for (unsigned j=0; j<PI_size; ++j)
		PI_sim[j][i] = (rand() % 2);
	}*/
	for (unsigned j=0; j<PI_size; ++j)
	    PI_sim[j] = (random() << 32) + random();


	pattern_num += 64;
	// do parallel simulation here
	if (!simulated)
	    SetFECGroup();

	parallel_simulation(PI_sim, PO_sim);
	Update_FEC();
	Write_POINFO(PI_sim, PO_sim, 64);

	string countings = "Total #FEC Group = ";
	countings += to_string(FEC_group.size());
	cout << countings;
	for (unsigned k=0; k<countings.length(); ++k)
	    cout << "\b" << flush;
	//now_size = FEC_group.at(0)->size();
	if (FEC_group.size() == 0)
	    break;
	else if (!fec_limit)
	{
	    if (++count_limit > 50)
		break;
	}
	else
	{
	    count_limit = 0;
	}
	/*else if (limit_size <= FEC_group.at(0)->size())
	{
	    if (++count_limit > limit_time)
	        break;
	}
	else
	{
	    limit_size = FEC_group.at(0)->size();
	    count_limit = 0;
	}*/
   	//CirGate::setglobalref();
   }

   CirGate::setglobalref();
   for (vector<PI_sim_vector*>::const_iterator iter=fail_pattern.begin(); iter!=fail_pattern.end(); iter++)
   {
	PI_sim = *(*iter);

	pattern_num += 64;
	// do parallel simulation here
	if (!simulated)
	    SetFECGroup();

	parallel_simulation(PI_sim, PO_sim);
	Update_FEC();
	Write_POINFO(PI_sim, PO_sim, 64);

	string countings = "Total #FEC Group = ";
	countings += to_string(FEC_group.size());
	cout << countings;
	for (unsigned k=0; k<countings.length(); ++k)
	    cout << "\b" << flush;
   }
   CirGate::setglobalref();
   cout << "Total #FEC Group = " << FEC_group.size() << endl;
   cout << pattern_num << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   string pattern;
   PI_sim_vector PI_sim, PO_sim;
   PI_sim.resize(PI_size);
   PO_sim.resize(PO_size);
   bool finish = false;
   bool loopfinish = false;
   unsigned pattern_num = 0;
   while (!finish)
   {
   	for (unsigned i=0; i<PI_size; ++i)
	    PI_sim.at(i).reset();
   	for (unsigned i=0; i<64; ++i)
   	{
	    if (patternFile >> pattern)
	    {
		if (pattern.length() != PI_size)
		{
		    cerr << "Error: Pattern(" << pattern << ") length(" << pattern.length() << ") does not match the number of inputs(" << PI_size << ") in a circuit!!" << endl;
		    cout << pattern_num << " patterns simulated." << endl;
		    return;
		}
		for (unsigned j=0; j<PI_size; ++j)
		{
		    if ((pattern[j] != '1') && (pattern[j] != '0'))
		    {
			cerr << "Error: Pattern(" << pattern << ") contains a non-0/1 character('" << pattern[j] << "')." << endl;
			cout << pattern_num << " patterns simulated." << endl;
			return;
		    }
		    PI_sim[j][i] = atoi(pattern.substr(j, 1).c_str());
		}
	    }
	    else
	    {
		finish = true;
		pattern_num += i;
		if (i == 0)
		    loopfinish = true;
		break;
	    }
	}
	if (!finish)
	    pattern_num += 64;
	// do parallel simulation here
	if (!simulated)
	    SetFECGroup();
	if (!loopfinish)
	{
	    parallel_simulation(PI_sim, PO_sim);
	    Update_FEC();
	    unsigned patterns = (pattern_num % 64 == 0) ? 64 : pattern_num % 64;
	    Write_POINFO(PI_sim, PO_sim, patterns);
	}
	/*
	string countings = "Total #FEC Group = ";
	countings += to_string(FEC_group.size());
	cout << countings;
	for (unsigned k=0; k<countings.length(); ++k)
	    cout << "\b" << flush;*/
   	//CirGate::setglobalref();
   }
   CirGate::setglobalref();
   cout << "Total #FEC Group = " << FEC_group.size() << endl;
   cout << pattern_num << " patterns simulated." << endl;
   patternFile.close();
   
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::Write_POINFO(PI_sim_vector& PI_sim, PI_sim_vector& PO_sim, const unsigned& num)
{
   if (_simLog == 0)
	return;
   for (unsigned i=0; i<num; ++i)
   {
	for (unsigned j=0; j<PI_size; ++j)
	    (*_simLog) << PI_sim[j][i];
	(*_simLog) << " ";
	for (unsigned j=0; j<PO_size; ++j)
	    (*_simLog) << PO_sim[j][i];
	(*_simLog) << endl;
   }
}


void
CirMgr::parallel_simulation(PI_sim_vector& PI_sim, PI_sim_vector& PO_sim)
{
   Map_iter mapped = Gates_map.find(0);
   (*mapped).second->setSimValue(0);
   for (vector<unsigned>::const_iterator iter = Undefined_id.begin(); iter != Undefined_id.end() ; iter++)
   {
	mapped = Gates_map.find(*iter);
	(*mapped).second->setSimValue(0);
   }
   for (unsigned i=1; i<=PI_size; ++i)
   {
	mapped = Gates_map.find(total_gate_id.at(i));
	(*mapped).second->setSimValue(PI_sim.at(i-1));
   }
   CirGate::setglobalref();
   if (_simLog == 0)
	return;
   for (unsigned i=PI_size+1; i<=PI_size+PO_size; ++i)
   {
	mapped = Gates_map.find(total_gate_id.at(i));
	PO_sim.at(i-1-PI_size) = (*mapped).second->getSimValue();
   }

}

void
CirMgr::SetFECGroup()
{
   IdList DFSList;
   UpdateDFSList(DFSList);
   Map_iter mapped = Gates_map.find(0);
   FEC_group.push_back(new vector<size_t>(1, size_t((*mapped).second)));
   FEC_group.at(0)->push_back(size_t((*mapped).second)+1);
   vector<unsigned> ordered;
   ordered.assign(total_gate_id.begin()+PI_size+PO_size+1, total_gate_id.end());
   sort(ordered.begin(), ordered.end());
   for (vector<unsigned>::const_iterator iter = ordered.begin(); iter != ordered.end(); iter++)
   {
	mapped = Gates_map.find(*iter);
	if (!(*mapped).second->InDFS())
	    continue;
	FEC_group.at(0)->push_back(size_t((*mapped).second));
	FEC_group.at(0)->push_back(size_t((*mapped).second)+1);
   }

   CirGate::setglobalref();
}

void
CirMgr::Update_FEC()
{
   Store_FEC new_FEC;
   new_FEC.reserve(FEC_group.size());
   Map_iter mapped = Gates_map.find(0);
   fec_limit = false;
   CirGate* dummy = (*mapped).second;
   for (Store_FEC_iter fec_g = FEC_group.begin(); fec_g != FEC_group.end(); fec_g++)
   {
	Fec_Hash new_group;
	new_group.clear();
	for (vector<size_t>::const_iterator iter = (*fec_g)->begin(); iter != (*fec_g)->end(); iter++)
	{
	    Fec_iter fec = new_group.find(dummy->getSim(*iter));
	    if (fec != new_group.end())
		(*fec).second->push_back(*iter);
	    else
	    {
		if (!new_group.empty())
		    fec_limit = true;
		new_group[dummy->getSim(*iter)] = new vector<size_t>(1, *iter);
	    }
	}
	for (Fec_iter new_iter = new_group.begin(); new_iter != new_group.end(); new_iter++)
	{
	    if ((*new_iter).second->size() <= 1)
	    {
		dummy->realgate((*new_iter).second->at(0))->resetFECpartner();
		delete (*new_iter).second;
	    }
	    else if ((!simulated) && (dummy->isInverse((*new_iter).second->at(0))))
	    {
		delete (*new_iter).second;    
	    }
	    else
	    {
		for (vector<size_t>::const_iterator iter = (*new_iter).second->begin(); iter != (*new_iter).second->end(); iter++)
		{
			dummy->realgate(*iter)->setFECpartner((*new_iter).second);
		}
		new_FEC.push_back((*new_iter).second);
	    }
	}
   }

   for (Store_FEC_iter iter = FEC_group.begin(); iter != FEC_group.end(); ++ iter)
	delete (*iter);
   //Store_FEC().swap(FEC_group);
   FEC_group.clear();
   FEC_group = new_FEC;
   simulated = true;
}
