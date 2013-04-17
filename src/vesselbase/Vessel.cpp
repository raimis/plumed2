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
#include "ActionWithVessel.h"
#include "Vessel.h"
#include "tools/Exception.h"
#include "tools/Communicator.h"
#include "tools/Log.h"

namespace PLMD {
namespace vesselbase{

Keywords VesselOptions::emptyKeys;

VesselOptions::VesselOptions(const std::string& thisname, const std::string& thislab, const unsigned& nlab, const std::string& params, ActionWithVessel* aa ):
myname(thisname),
mylabel(thislab),
numlab(nlab),
action(aa),
keywords(emptyKeys),
parameters(params)
{
}

VesselOptions::VesselOptions(const VesselOptions& da, const Keywords& keys ):
myname(da.myname),
mylabel(da.mylabel),
numlab(da.numlab),
action(da.action),
keywords(keys),
parameters(da.parameters)
{
}

void Vessel::registerKeywords( Keywords& keys ){
  plumed_assert( keys.size()==0 );
}

Vessel::Vessel( const VesselOptions& da ):
myname(da.myname),
numlab(da.numlab),
action(da.action),
keywords(da.keywords),
finished_read(false),
comm(da.action->comm),
log((da.action)->log)
{
  line=Tools::getWords( da.parameters );
  if( da.mylabel.length()==0 && numlab>=0 ){
      mylabel=myname; std::string nn; 
      std::transform( mylabel.begin(), mylabel.end(), mylabel.begin(), tolower );
      if(numlab>0){ Tools::convert( numlab, nn ); mylabel = mylabel + "-" + nn; }
  } else if( numlab==0 ) {
      mylabel=da.mylabel;
  }
}

std::string Vessel::getName() const {
  return myname;
}

std::string Vessel::getLabel() const {
  return mylabel;
}

std::string Vessel::getAllInput(){
  std::string fullstring;
  for(unsigned i=0;i<line.size();++i){ 
      fullstring = fullstring + " " + line[i];
  } 
  line.clear(); line.resize(0);
  return fullstring;
}

void Vessel::parseFlag(const std::string&key, bool & t){
  // Check keyword has been registered
  plumed_massert(keywords.exists(key), "keyword " + key + " has not been registered");
  // Check keyword is a flag
  if(!keywords.style(key,"nohtml")){
     plumed_massert(keywords.style(key,"flag"), "keyword " + key + " is not a flag");
  }

  // Read in the flag otherwise get the default value from the keywords object
  if(!Tools::parseFlag(line,key,t)){
     if( keywords.style(key,"nohtml") ){ 
        t=false; 
     } else if ( !keywords.getLogicalDefault(key,t) ){
        plumed_merror("there is a flag with no logical default in a vessel - weird");
     }
  }
}

void Vessel::checkRead(){
  if(!line.empty()){
    std::string msg="cannot understand the following words from input : ";
    for(unsigned i=0;i<line.size();i++) msg = msg + line[i] + ", ";
    error(msg);
  }
  finished_read=true;
  std::string describe=description();
  if( describe.length()>0 ) log.printf("  %s\n", describe.c_str() );
}

void Vessel::error( const std::string& msg ){
  action->log.printf("ERROR for keyword %s in action %s with label %s : %s \n \n",myname.c_str(), (action->getName()).c_str(), (action->getLabel()).c_str(), msg.c_str() );
  if(finished_read) keywords.print( log );
  plumed_merror("ERROR for keyword " + myname + " in action "  + action->getName() + " with label " + action->getLabel() + " : " + msg );
}

void Vessel::stashBuffers(){
  unsigned stride=comm.Get_size(); unsigned rank=comm.Get_rank();
  unsigned n=0; for(unsigned i=rank;i<bufsize;i+=stride){ stash[n]=getBufferElement(i); n++; }
}

void Vessel::setBufferFromStash(){
  unsigned stride=comm.Get_size(); unsigned rank=comm.Get_rank();
  unsigned n=0; for(unsigned i=rank;i<bufsize;i+=stride){ addToBufferElement( i, stash[n]); n++; }
}

}
}
