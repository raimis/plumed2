/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2016 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed.org for more information.

   This file is part of plumed, version 2.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#include "Bias.h"
#include "ActionRegister.h"
#include "core/PlumedMain.h"
#include "core/Atoms.h"
#include <cmath>

using namespace std;

namespace PLMD{
namespace bias{

//+PLUMEDOC BIAS EMRESTRAINT
/*
Put the doc here

*/
//+ENDPLUMEDOC


class EMrestraint : public Bias
{
  // temperature in kbt
  double kbt_;
  // exp data points
  vector<double> ovdd_;
  // output
  Value* valueBias;
  
public:
  EMrestraint(const ActionOptions&);
  void calculate();
  static void registerKeywords(Keywords& keys);
};


PLUMED_REGISTER_ACTION(EMrestraint,"EMRESTRAINT")

void EMrestraint::registerKeywords(Keywords& keys){
  Bias::registerKeywords(keys);
  keys.use("ARG");
  keys.add("compulsory","EXPVALUES","experimental data points");
  componentsAreNotOptional(keys); 
  keys.addOutputComponent("bias","default","the instantaneous value of the bias potential");
}

EMrestraint::EMrestraint(const ActionOptions&ao):
PLUMED_BIAS_INIT(ao)
{

  parseVector("EXPVALUES", ovdd_);
  checkRead();

  // check if experimental data points are as many as arguments
  if(ovdd_.size()!=getNumberOfArguments()) error("Wrong number of experimental data points\n");

  // get temperature
  kbt_ = plumed.getAtoms().getKbT();

  log.printf("  temperature of the system %f\n",kbt_);
  log.printf("  number of experimental data points %u\n",static_cast<unsigned>(ovdd_.size()));

  addComponent("bias");   componentIsNotPeriodic("bias");

  valueBias=getPntrToComponent("bias");

}


void EMrestraint::calculate(){
 
  // cycle on arguments 
  double ene = 0.0;
  vector<double> ene_der(getNumberOfArguments());
  
  for(unsigned i=0;i<getNumberOfArguments();++i){
    // individual term
    ene_der[i] = std::log(getArgument(i)/ovdd_[i]);
    // increment energy
    ene += ene_der[i] * ene_der[i];
  };

  // constant factor
  double fact = kbt_ * 0.5 * static_cast<double>(ovdd_.size());

  // get derivatives
  for(unsigned i=0;i<getNumberOfArguments();++i){
    // calculate derivative
    double der = 2.0 * fact / ene * ene_der[i] / getArgument(i);
    // set forces
    setOutputForce(i, -der);
  }

  // set value of the bias
  valueBias->set(fact * std::log(ene));
}


}
}

