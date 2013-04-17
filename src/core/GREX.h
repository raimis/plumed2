/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2013 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.0.

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
#ifndef __PLUMED_core_GREX_h
#define __PLUMED_core_GREX_h

#include "WithCmd.h"
#include <string>
#include <vector>

namespace PLMD{

class PlumedMain;
class Atoms;
class Communicator;

class GREX:
  public WithCmd
{
  bool initialized;
  Communicator& intracomm;
  Communicator& intercomm;
  PlumedMain& plumedMain;
  Atoms&      atoms;
  int partner;
  double localDeltaBias;
  double foreignDeltaBias;
  std::vector<double> allDeltaBias;
  std::string buffer;
  int myreplica;
public:
  GREX(PlumedMain&);
  ~GREX();
  void cmd(const std::string&key,void*val=NULL);
  void calculate();
  void savePositions();
};

}

#endif
