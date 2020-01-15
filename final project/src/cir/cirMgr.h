/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <bitset>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "sat.h"
/*
typedef std::unordered_map< unsigned, CirGate* >  Map_index;
typedef std::unordered_map< unsigned, CirGate* >::const_iterator  Map_iter;
typedef std::bitset<64> Parallel_sim;
typedef vector <Parallel_sim> PI_sim_vector;
typedef std::unordered_map< size_t, vector <size_t> >  Fec_Hash;
*/
extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() { simulated = false; fail_pattern_num = 0; }
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   void readfirstline();
   bool checkPIs();
   bool checkPOs(unsigned&);
   bool checkAIGs();
   bool readsymbol();

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;


private:
   ofstream           *_simLog;

   Map_index Gates_map;
   IdList total_gate_id; // 0: CONST0, [1, PI_size]: PIs, [PI_size + 1, PI_size + PO_size]: POs, [PI_size + PO_size + 1, ]: ANDs 
   unsigned maximum_var;
   unsigned PI_size;
   unsigned PO_size;
   unsigned AND_size;
   vector< pair < CirGate*, unsigned > > Undefined_pre;  // floating-gate, literal (odd if inverse)
   IdList Undefined_id;
   Store_FEC FEC_group;
   bool simulated;
   bool fec_limit;
   vector<PI_sim_vector*> fail_pattern;
   unsigned fail_pattern_num;

   // Private member functions

   // DFS list
   void UpdateDFSList(IdList&);

   // sweep and optimization
   bool check_constant(CirGate*, CirGate*);
   bool check_trivial(CirGate*, CirGate*);

   // structure hashing
   void merging(CirGate*, CirGate*, const bool& inverse = false);

   // simulation
   void parallel_simulation(PI_sim_vector&, PI_sim_vector&);
   void SetFECGroup();
   void Update_FEC();
   void Write_POINFO(PI_sim_vector&, PI_sim_vector&, const unsigned&);

   // fraig
   void genProofModel(SatSolver&);
   void proofFraig(SatSolver&, CirGate*);
   void reportResult(const SatSolver&, bool);
   void UpdateBFSList(IdList&);

};

#endif // CIR_MGR_H
