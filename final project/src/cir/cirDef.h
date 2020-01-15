/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <string>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;


typedef std::unordered_map< unsigned, CirGate* >  Map_index;
typedef std::unordered_map< unsigned, CirGate* >::const_iterator  Map_iter;
typedef std::bitset<64> Parallel_sim;
typedef vector <Parallel_sim> PI_sim_vector;
typedef std::unordered_map< Parallel_sim, vector <size_t>* >  Fec_Hash;
typedef std::unordered_map< Parallel_sim, vector <size_t>* >::const_iterator  Fec_iter;
typedef vector < vector <size_t>* > Store_FEC;
typedef vector < vector <size_t>* >::const_iterator Store_FEC_iter;

typedef vector<CirGate*>           GateList;
typedef vector<unsigned>           IdList;

#endif // CIR_DEF_H
