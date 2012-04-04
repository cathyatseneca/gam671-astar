/* Design Implementation - Application Layer
 *
 * Design.cpp
 * fwk4gps version 3.0
 * gam666/dps901/gam670/dps905
 * January 14 2012
 * copyright (c) 2012 Chris Szalwinski 
 * distributed under TPL - see ../Licenses.txt
 */

#include "Design.h"          // for the Design class definition
#include "iText.h"           // for the Text Interface
#include "iHUD.h"            // for the HUD Interface
#include "iSound.h"          // for the Sound Interface
#include "iLight.h"          // for the Light Interface
#include "iObject.h"         // for the Object Interface
#include "iTexture.h"        // for the Texture Interface
#include "Camera.h"          // for the Camera Interface
#include "iGraphic.h"        // for the Graphic Interface
#include "iUtilities.h"      // for strcpy()
#include "MathDefinitions.h" // for MODEL_Z_AXIS
#include "ModellingLayer.h"  // for MOUSE_BUTTON_SCALE, ROLL_SPEED
#include "Common_Symbols.h"  // for Action and Sound enumerations
#include "Physics.h"
#include "CSphere.h"
#include "iSimpleCollisionSpace.h"
#include "minheap.h"
#include "llist.h"
#include "adjacencylist.h"
#include "searchdata.h"

#include <strsafe.h>
const wchar_t* orient(wchar_t*, const iFrame*, char, unsigned = 1u);
const wchar_t* position(wchar_t*, const iFrame*, char = ' ', unsigned = 1u);
const wchar_t* onOff(wchar_t*, const iSwitch*);
//const Colour red(1,0,0);
//-------------------------------- Design -------------------------------------
//
// The Design class implements the game design within the Modelling Layer
//
// constructor initializes the engine and the instance pointers
//
Design::Design(void* h, int s) : Coordinator(h, s), 	map_("graph.txt") {


}

// initialize initializes the general display design coordinator, creates the 
// primitive sets, textures, objects, lights, sounds, cameras, and text items
//
void Design::initialize() {

    // general parameters
    //
	Reflectivity redish = Reflectivity(red);
	Reflectivity greenish = Reflectivity(green);
	Reflectivity bluish = Reflectivity(blue);
	Reflectivity whitish = Reflectivity(white);
	iGraphic* box;
    setProjection(0.9f, 1.0f, 1000.0f);
    setAmbientLight(0.9f, 0.9f, 0.9f);

    iCamera* camera = CreateCamera();
    camera->translate(0,150,0);
    camera->setRadius(17.8f);
	camera->rotatex(3.1459/2.0f);



    int i=0;
    for(i=0;i<map_.numvert();i++){
		 box   = CreateBox(-3, -3, -3 * MODEL_Z_AXIS,  3, 3, 3 * MODEL_Z_AXIS);
		CreateObject(box,&redish)->translate(map_.vx(i),map_.vy(i),map_.vz(i));
    }
    for(int j=0;j<map_.numvert();j++){
      LList<EdgeInfo>& edgelist=map_.edges(j);
      Node<EdgeInfo>* curr;
      while(curr=edgelist.curr()){
        int from=curr->data().from();
        int to=curr->data().to();
        if(from < to){
          Vector frompos=map_.pos(from);
          Vector topos=map_.pos(to);
		  float speed= curr->data().speed();
		  iGraphic* path=CreatePath(frompos.x,frompos.y,frompos.z,topos.x,topos.y,topos.z);
		  Reflectivity* pathcolour=(speed<FAST)?((speed<MEDIUM)?&redish:&bluish):&greenish;
		  CreateObject(path,pathcolour);
          #if DEBUG==1
          fprintf(debugfile,"from/to: %d %d\n",from,to);          
          fprintf(debugfile,"frompos %f %f %f\n",frompos.x,frompos.y,frompos.z);
          fprintf(debugfile,"topos %f %f %f\n",topos.x,topos.y,topos.z);
          fprintf(debugfile,"v %f %f %f\n",v.x,v.y,v.z);
          fprintf(debugfile,"edglen= %f\n",edgelen);
          #endif
		  i++;
        }
        edgelist.gonext();
      }
    }
	bucket_=CreatePhysicsBox(-2,-2,-2,2,2,2,&bluish,1,PHYS_FixedInSpace);
    whichbox_=0;
    Vector boxloc=map_.pos(whichbox_);
    bucket_->translate(boxloc.x,boxloc.y+4,boxloc.z);
	box = CreateBox(-3.1, -3.1, -3.1 * MODEL_Z_AXIS,  3.1, 3.1, 3.1 * MODEL_Z_AXIS);
	(highlighter_ = CreateObject(box,&greenish))->translate(map_.vx(0),map_.vy(0),map_.vz(0));
	selectloc_=0;
	lastFireTime_=0;
	ismoving_=false;
	searchroutine_=GREEDY;


	hud = CreateHUD(0.72f, 0.01f, 0.27f, 0.99f, CreateTexture(HUD_IMAGE));


	velocitytxt_=CreateText(Rectf(0.05f,0.27f,0.95f,0.37f),hud,L"",TEXT_HEIGHT,TEXT_TYPEFACE,TEXT_LEFT);
	deltatxt_=CreateText(Rectf(0.05f,0.17f,0.95f,0.27f),hud,L"",TEXT_HEIGHT,TEXT_TYPEFACE,TEXT_LEFT);
	positiontxt_=CreateText(Rectf(0.05f,0.38f,0.95f,0.48f),hud,L"",TEXT_HEIGHT,TEXT_TYPEFACE,TEXT_LEFT);


	lasttextupdate=now;
	
}

// update updates the position and orientation of each object according to the 
// actions initiated by the user
//
void Design::update() {


    int delta;
	int dt=0;
	int ds=0;
    delta = now - lastUpdate;
    lastUpdate = now;
	Vector hpos;
    if (now - lastFireTime_ > 100) {
		if(pressed(GREEDY_KEY)){
			searchroutine_=GREEDY;
			lastFireTime_=now;
		}
		else if(pressed(UNIFORM_KEY)){
			searchroutine_=UNIFORM;
			lastFireTime_=now;
		}
		else if(pressed(ASTAR_KEY)){
			searchroutine_=ASTAR;
			lastFireTime_=now;
		}
		else if(pressed(IDASTAR_KEY)){
			searchroutine_=IDASTAR;
			lastFireTime_=now;
		}
		else if(pressed(NEXTBOX)){
			selectloc_=(selectloc_+1)%map_.numvert();
			hpos=highlighter_->position();
			highlighter_->translate(-hpos.x,-hpos.y,-hpos.z);
			highlighter_->translate(map_.vx(selectloc_),map_.vy(selectloc_),map_.vz(selectloc_));
			lastFireTime_=now;
		}
		else if(pressed(PREVBOX)){
			selectloc_=(selectloc_==0)?map_.numvert()-1:selectloc_-1;
			hpos=highlighter_->position();
			highlighter_->translate(-hpos.x,-hpos.y,-hpos.z);
			highlighter_->translate(map_.vx(selectloc_),map_.vy(selectloc_),map_.vz(selectloc_));
			lastFireTime_=now;
		}
		else if(pressed(STARTANIM)){
			if(!ismoving_){
				if(selectloc_!=whichbox_){
					bool searchsuccess=false;        //stores return value of path finding function
					switch (searchroutine_){
						case GREEDY: searchsuccess=greedy(selectloc_); break;
						case UNIFORM: searchsuccess=uniformCost(selectloc_); break;
						case ASTAR: searchsuccess=aStar(selectloc_); break;
						case IDASTAR: searchsuccess=iDAStar(selectloc_); break;
					}/*switch*/
					if(searchsuccess){
						ismoving_=true;
						currloc_=0;
					}
				}
			}//!ismoving_
		}
	}
	if(ismoving_){
		  //move bucket along
		  bool done=false;
		  float leftover=(float)delta/UNITS_PER_SEC;  //amount of time leftover moving bucket along the edge
		  //in this time frame.
		  int dest=path_[currloc_+1];
	  	  while(!done && moveBucket(path_[currloc_],path_[currloc_+1],leftover)){

			//function returns true if destination was reached
			//in this time frame.  so if it was reached,
			//continue to next node or if end of path stop the movment
			currloc_++;
			if(currloc_==numnodes_-1){
				whichbox_=path_[numnodes_-1];
				done=true;
				currloc_=0;
				ismoving_=false;
			}
		  }
	   }


	
 
    lastUpdate = now;
 
}

/*function moves bucket along path from src to dest.  It has delta
  time to do this if it reaches the dest function returns true,false otherwise
  when it reaches the destination, amount of left over time is returned*/
bool Design::moveBucket(int src,int dest,float& leftover){
  Vector startpos=bucket_->position(); //buckets starting position
  
//  fprintf(debugfile_,"bucket pos: %f,%f,%f\n",object[bucketidx_]->position().x,
//                                    object[bucketidx_]->position().y,object[bucketidx_]->position().z);
//  fflush(debugfile_);
  float speed=map_.speed(src,dest);         //speed of the edge between src and dest
//  fprintf(debugfile_,"speed: %f\n",speed);
//  fflush(debugfile_);
  startpos.y=startpos.y-4;
  Vector destvec=map_.pos(dest)-startpos;   //vector between buckets starting pos
                                            //and its destination.
  //destvec.y=destvec.y+4;
  Vector ndestvec=normal(destvec);
  Vector edgevec=speed*ndestvec;
  bool rc=false;
  //sets the velocity of the bucket to that of the edge
  bucket_->setVelocity(edgevec);

  Vector displacement = leftover*bucket_->velocity();
//  fprintf(debugfile_,"speed: %f\n",speed);
//  fprintf(debugfile_,"velocity : (%f,%f,%f), |velocity| = %f\n",object[bucketidx_]->velocity().x,
//                                    object[bucketidx_]->velocity().y,object[bucketidx_]->velocity().z,
//                                    object[bucketidx_]->velocity().length());
//  fprintf(debugfile_,"displacement: %f,%f,%f\n",displacement.x,displacement.y,displacement.z);
//  fprintf(debugfile_,"destination: %f,%f,%f\n",destvec.x,destvec.y,destvec.z);
  if(displacement.length() >= destvec.length()){  //statement is true, if bucket move
                                                 //beyond the destination
     rc=true;                                    //reaches the destination node
     bucket_->translate(destvec.x,destvec.y,destvec.z);
     leftover=leftover - (destvec.length()/speed);
  }/*if*/
  else{
    bucket_->updateKinematics(leftover);
  }
  bucket_->setVelocity(Vector(0,0,0));
  return rc;
}
//total cost of travel to node currconnection
float Design::timecost(float oldtotal,Node<EdgeInfo>* currconnect){
  int to=currconnect->data().to();
  int from=currconnect->data().from();
  float sp=currconnect->data().speed();
  Vector displacement=map_.pos(to)-map_.pos(from);
  float time=abs(displacement.length()/sp);
  return time+oldtotal;
}
//estimated time to destination from node stored in currconnection
float Design::timeleft(Node<EdgeInfo>* currconnect,int dest){
  int to=currconnect->data().to();
  Vector displacement=map_.pos(to)-map_.pos(dest);
  double speed=FAST; //fastest
  float time=abs(displacement.length()/speed);
  return time;
}
//given a closelist and the element in the closelist to look at this function
//sets the path_ data member and the number of nodes in the path_
void Design::setPath(CloseStructure* closelist,int dest){
  int nodeidx=closelist[dest].nodeinf_.parent();
  int i=1;
  path_[0]=dest;
  while(nodeidx!=-1){
    path_[i]=closelist[nodeidx].nodeinf_.nodenum();
    nodeidx=closelist[nodeidx].nodeinf_.parent();
    i++;
  }
  for(int j=0;j<i/2;j++){
    int tmp=path_[j];
    path_[j]=path_[i-j-1];
    path_[i-j-1]=tmp;
  }
  numnodes_=i;
}
/*For your assignment, code this function*/
bool Design::aStar(int dest){
  return false;
}
/*For your assignment, code this function*/
bool Design::iDAStar(int dest){
  return false;
}
bool Design::uniformCost(int dest){
#if DEBUG==3
    fprintf(debugfile_,"UNIFORM COST");
    fprintf(debugfile_,"dest: %d\n",dest);
    fflush(debugfile_);
#endif
  MinHeap<SearchData> openlist;
  CloseStructure* closelist=new CloseStructure[map_.numvert()];
  bool rc=false;
  Node<EdgeInfo>* currconnect;
  int nn;
  SearchData curr;
  SearchData tmp;
  curr.set(whichbox_,-1,0);  //start at current node, parent is -1.
                             //uniform cost so cost incurred so far is 0
  bool done=false;
  bool found=false;
  float biggestcost=0;
  int numclosed=0;
  do{
   #if DEBUG==3
    fprintf(debugfile_,"curr.nodenum: %d\n",curr.nodenum());
    fflush(debugfile_);
    #endif
    LList<EdgeInfo>& edgelist=map_.edges(curr.nodenum());  //get the conections to curr

    while(currconnect=edgelist.curr()){              //for each node connected to curr
      nn=currconnect->data().to();
      if(!closelist[nn].closed_){                     //if its not in the closed list
        #if DEBUG==3
        fprintf(debugfile_,"add to openlist: %d\n",nn);
        fflush(debugfile_);
        #endif
        tmp.set(nn,curr.nodenum(),timecost(curr.cost(),currconnect));
        openlist.insert(tmp);
      }
      edgelist.gonext();
    }//while
    closelist[curr.nodenum()].closed_=true;  //add it to the close list
    closelist[curr.nodenum()].nodeinf_=curr;
    numclosed++;
    if(!openlist.isempty()){    //if there are still nodes to consider
      curr=openlist.remove();//remove lowest cost item from list
      #if DEBUG==3
      fprintf(debugfile_,"removed from openlist: %d\n",curr.nodenum());
      fflush(debugfile_);
      #endif      
      while(!openlist.isempty() && closelist[curr.nodenum()].closed_){            //if already encountered
        curr=openlist.remove();//remove lowest cost item from list  //consider the next node
      }
      if(closelist[curr.nodenum()].closed_){
        //only way to reach this part of code is if the open list is empty and 
        //we have not found the next node to examine
        done=true;
      }
      else{  
        if(curr.nodenum()==dest){
          found=true;       //found a node to the destination... question is, is it the best.
          biggestcost=curr.cost();
        }
        if(found && curr.cost() > biggestcost){
          done=true;
        }
      }//else still have node
    }//if list is not empty
    else{
      done=true;
    }//open list is empty
  }while(!done);
  if(found){ 
    rc=true;
    setPath(closelist,dest);
    delete [] closelist;
  }
    return rc;
}

/*code implements Greedy search*/
bool Design::greedy(int dest){
  #if DEBUG==3
    fprintf(debugfile_,"GREEDY");
    fprintf(debugfile_,"dest: %d\n",dest);
    fflush(debugfile_);
  #endif
  MinHeap<SearchData> openlist;
  CloseStructure* closelist=new CloseStructure[map_.numvert()];
  bool rc=false;
  Node<EdgeInfo>* currconnect;
  int nn;
  SearchData curr;
  SearchData tmp;
  curr.set(whichbox_,-1,0);  //start at current node, parent is -1.
                             //uniform cost so cost incurred so far is 0
  bool done=false;
  bool found=false;
  float biggestcost=0;
  int numclosed=0;
  do{
    #if DEBUG==3
    fprintf(debugfile_,"curr.nodenum: %d\n",curr.nodenum());
    fflush(debugfile_);
    #endif
    LList<EdgeInfo>& edgelist=map_.edges(curr.nodenum());  //get the conections to curr

    while(currconnect=edgelist.curr()){              //for each node connected to curr
      nn=currconnect->data().to();
      if(!closelist[nn].closed_){                     //if its not in the closed list
        #if DEBUG==3
        fprintf(debugfile_,"add to openlist: %d\n",nn);
        fflush(debugfile_);
        #endif
        tmp.set(nn,curr.nodenum(),timeleft(currconnect,dest));
        openlist.insert(tmp);
      }
      edgelist.gonext();
    }//while
    closelist[curr.nodenum()].closed_=true;  //add it to the close list
    closelist[curr.nodenum()].nodeinf_=curr;
    numclosed++;
    if(!openlist.isempty()){    //if there are still nodes to consider
      curr=openlist.remove();//remove lowest cost item from list
      #if DEBUG == 3
      fprintf(debugfile_,"removed from openlist: %d\n",curr.nodenum());
      fflush(debugfile_);
      #endif
      while(!openlist.isempty() && closelist[curr.nodenum()].closed_){            //if already encountered
        curr=openlist.remove();//remove lowest cost item from list  //consider the next node
      }
      if(closelist[curr.nodenum()].closed_){
        //only way to reach this part of code is if the open list is empty and 
        //we have not found the next node to examine
        done=true;
      }
      else{  
        if(curr.nodenum()==dest){
          found=true;       //found a node to the destination... question is, is it the best.
          biggestcost=curr.cost();
        }
        if(found && curr.cost() > biggestcost){
          done=true;
        }
      }//else still have node
    }//if list is not empty
    else{
      done=true;
    }//open list is empty
  }while(!done);
  if(found){ 
    rc=true;
    setPath(closelist,dest);
    delete [] closelist;
  }
    return rc;
}