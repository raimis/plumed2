#ifndef __PLUMED_RMSD_h
#define __PLUMED_RMSD_h

#include "Vector.h"
#include "Log.h"
#include <vector>
#include <string>
#include "OptimalAlignment.h" 

namespace PLMD{

class PDB;

/// A class that implements RMSD calculations
class RMSD
{
  enum AlignmentMethod {SIMPLE, OPTIMAL};
  AlignmentMethod alignmentMethod;
  std::vector<Vector> reference;
  std::vector<double> align;
  std::vector<double> displace;
  OptimalAlignment *myoptimalalignment;
  Log &log;
public:
/// initialize the log in the constructor
  RMSD(Log & log ): myoptimalalignment(NULL),log(log){};
/// clear the structure
  void clear();
/// set reference, align and displace from input pdb structure
  void setFromPDB(const PDB&, std::string mytype);
/// set reference coordinates
  void setReference(const std::vector<Vector> & reference);
/// set weights
  void setAlign(const std::vector<double> & align);
/// set align
  void setDisplace(const std::vector<double> & displace);
/// 
  std::string getMethod();	
/// Compute rmsd
  double calculate(const std::vector<Vector> & positions,std::vector<Vector> &derivatives);
};

}

#endif

