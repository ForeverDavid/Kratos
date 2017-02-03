//
//   Project Name:        KratosPfemBaseApplication $
//   Created by:          $Author:      JMCarbonell $
//   Last modified by:    $Co-Author:               $
//   Date:                $Date:      February 2016 $
//   Revision:            $Revision:            0.0 $
//
//

#if !defined(KRATOS_BUILD_MODEL_PART_BOUNDARY_PROCESS_H_INCLUDED )
#define  KRATOS_BUILD_MODEL_PART_BOUNDARY_PROCESS_H_INCLUDED


// System includes
#include <string>
#include <iostream>

// External includes
#include <boost/timer.hpp>


// Project includes
#include "includes/define.h"
#include "processes/process.h"
#include "includes/node.h"
#include "includes/element.h"
#include "includes/model_part.h"

#include "geometries/line_2d_2.h"
#include "geometries/line_3d_2.h"
#include "geometries/triangle_3d_3.h"

#include "custom_conditions/composite_condition.hpp"
#include "custom_utilities/boundary_normals_calculation_utilities.hpp"
#include "custom_utilities/modeler_utilities.hpp"

#include "pfem_base_application_variables.h"

///VARIABLES used:
//Data:     MASTER_ELEMENTS(set), MASTER_NODES(set), NEIGHBOUR_ELEMENTS
//StepData: RIGID_WALL
//Flags:    (checked) CONTACT
//          (set)     BOUNDARY(set)
//          (modified)  
//          (reset)   
//(set):=(set in this process)

namespace Kratos
{

  ///@name Kratos Globals
  ///@{

  ///@}
  ///@name Type Definitions
  ///@{
  typedef  ModelPart::NodesContainerType NodesContainerType;
  typedef  ModelPart::ElementsContainerType ElementsContainerType;
  typedef  ModelPart::ConditionsContainerType ConditionsContainerType;

  ///@}
  ///@name  Enum's
  ///@{

  ///@}
  ///@name  Functions
  ///@{

  ///@}
  ///@name Kratos Classes
  ///@{

  /// Short class definition.
  /** Detail class definition.
   */
  class BuildModelPartBoundaryProcess
    : public Process
  {
  public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of BuildModelPartBoundaryProcess
    KRATOS_CLASS_POINTER_DEFINITION( BuildModelPartBoundaryProcess );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    BuildModelPartBoundaryProcess(ModelPart& rModelPart,
			     std::string const rModelPartName,
			     int EchoLevel = 0)
      : mrModelPart(rModelPart)
    { 
      mModelPartName = rModelPartName;
      mEchoLevel     = EchoLevel;
    }

    /// Destructor.
    virtual ~BuildModelPartBoundaryProcess()
    {
    }


    ///@}
    ///@name Operators
    ///@{

    void operator()()
    {
      Execute();
    }


    ///@}
    ///@name Operations
    ///@{

    virtual void Execute()
    {

      KRATOS_TRY

      bool success=false;

      boost::timer auxiliary;

      unsigned int NumberOfSubModelParts=mrModelPart.NumberOfSubModelParts();
      
      if( mModelPartName == mrModelPart.Name() ){
			  
	for(ModelPart::SubModelPartIterator i_mp= mrModelPart.SubModelPartsBegin() ; i_mp!=mrModelPart.SubModelPartsEnd(); i_mp++)
	  {
			    
	    if( mEchoLevel >= 1 )
	      std::cout<<" [ Construct Boundary on ModelPart ["<<i_mp->Name()<<"] ]"<<std::endl;

	    success=UniqueSkinSearch(*i_mp);
			    
	    if(!success)
	      {
		std::cout<<"  ERROR: BOUNDARY CONSTRUCTION FAILED ModelPart : ["<<i_mp->Name()<<"] "<<std::endl;
	      }
	    else
	      {
		if( mEchoLevel >= 1 )
		  std::cout<<" [ Performed in Time = "<<auxiliary.elapsed()<<" ]"<<std::endl;
		//PrintSkin(*i_mp);
	      }
	  }
      }
      else{
	
	if( mEchoLevel >= 1 )
	  std::cout<<" [ Construct Boundary on ModelPart["<<mModelPartName<<"] ]"<<std::endl;

	ModelPart& rModelPart = mrModelPart.GetSubModelPart(mModelPartName);
	success=UniqueSkinSearch(rModelPart);
			    
	if(!success)
	  {
	    std::cout<<"  ERROR: BOUNDARY CONSTRUCTION FAILED on ModelPart : ["<<rModelPart.Name()<<"] "<<std::endl;
	  }
	else
	  {
	    if( mEchoLevel >= 1 )
	      std::cout<<" [ Performed in Time = "<<auxiliary.elapsed()<<" ]"<<std::endl;
	    //PrintSkin(rModelPart);
	  }
      }
	
      if( NumberOfSubModelParts > 1 ){	
	SetMainModelPartConditions();
	SetComputingModelPart();
      }
      
      //ComputeBoundaryNormals BoundUtils;
      BoundaryNormalsCalculationUtilities BoundaryComputation;
      if( mModelPartName == mrModelPart.Name() ){
	BoundaryComputation.CalculateWeightedBoundaryNormals(mrModelPart, mEchoLevel);
      }
      else{
	ModelPart& rModelPart = mrModelPart.GetSubModelPart(mModelPartName);
	BoundaryComputation.CalculateWeightedBoundaryNormals(rModelPart, mEchoLevel);
      }

	
      if( mEchoLevel >= 1 )
	std::cout<<"  Boundary Normals Computed and Assigned ] "<<std::endl;

      KRATOS_CATCH( "" )
	
    }


    //**************************************************************************
    //**************************************************************************

    bool SearchConditionMasters(ModelPart& rModelPart)
    {

      KRATOS_TRY

      int composite_conditions = 0;
      int total_conditions = 0;
      int counter = 0;
      
      bool found=false;

      for(ModelPart::ConditionsContainerType::iterator i_cond = rModelPart.ConditionsBegin(); i_cond != rModelPart.ConditionsEnd(); i_cond++)
	{

	  if( i_cond->Is(BOUNDARY) ) //composite condition
	    composite_conditions++;
	  
	  //std::cout<<" Condition ("<<i_cond->Id()<<") : ME="<<i_cond->GetValue(MASTER_ELEMENTS)[0].Id()<<", MN= "<<i_cond->GetValue(MASTER_NODES)[0].Id()<<std::endl;

	  //********************************************************************

	  boost::numeric::ublas::matrix<unsigned int> lpofa; //points that define the faces

	  Geometry< Node<3> >& rConditionGeometry = i_cond->GetGeometry();
	  unsigned int size=rConditionGeometry.size();
			    
	  bool perform_search = true;
	  for(unsigned int i=0; i<size; i++)
	    {
	      if( rConditionGeometry[i].SolutionStepsDataHas(RIGID_WALL) ){
		if( rConditionGeometry[i].FastGetSolutionStepValue(RIGID_WALL) ) //if is a rigid wall do not search else do search
		  perform_search = false;
	      }
	    }		   		     

	  if( size != 2 ) 
	    perform_search = false;

	  if( i_cond->Is(CONTACT) )
	    perform_search = false;
	  
	  //********************************************************************
	  found=false;

	  if( perform_search )
	    {

	      WeakPointerVector<Element >& rE1 = rConditionGeometry[0].GetValue(NEIGHBOUR_ELEMENTS);				    
	      WeakPointerVector<Element >& rE2 = rConditionGeometry[1].GetValue(NEIGHBOUR_ELEMENTS);

	      if( rE1.size() == 0 || rE2.size() == 0 )
		std::cout<<" NO SIZE in NEIGHBOUR_ELEMENTS "<<std::endl;
	      
	      for(WeakPointerVector< Element >::iterator ie = rE1.begin(); ie!=rE1.end(); ie++)
		{
		  for(WeakPointerVector< Element >::iterator ne = rE2.begin(); ne!=rE2.end(); ne++)
		    {

		      if (ne->Id() == ie->Id() && !found)
			{
			  WeakPointerVector< Element > MasterElements;
			  MasterElements.push_back(Element::WeakPointer( *(ie.base()) ) );
			  i_cond->SetValue(MASTER_ELEMENTS,MasterElements);
					 
			  Geometry< Node<3> >& rElementGeom = ie->GetGeometry();

			  rElementGeom.NodesInFaces(lpofa);

			  int node = 0;
			  for (unsigned int i=0; i<rElementGeom.size(); i++)
			    {
			      if( (   rConditionGeometry[0].Id() == rElementGeom[lpofa(1,i)].Id() 
				      && rConditionGeometry[1].Id() == rElementGeom[lpofa(2,i)].Id() ) || 
				  (   rConditionGeometry[0].Id() == rElementGeom[lpofa(2,i)].Id() 
				      && rConditionGeometry[1].Id() == rElementGeom[lpofa(1,i)].Id() ) )
				{
				  node=i;
				  found = true;
				  break;
				}
			    }
						
			  if(found){
			    WeakPointerVector< Node<3> > MasterNodes;
			    MasterNodes.push_back( Node<3>::WeakPointer( rElementGeom(lpofa(0,node)) ) );
			    i_cond->SetValue(MASTER_NODES,MasterNodes);
			  }
			  else{						 
			    std::cout<<" MASTER_NODE not FOUND : something is wrong "<<std::endl;			  
			  }

			}
		    }
		}

	      total_conditions++;
	    }

	  //********************************************************************

	  //std::cout<<" After Condition ("<<i_cond->Id()<<") : ME="<<i_cond->GetValue(MASTER_ELEMENTS)[0].Id()<<", MN= "<<i_cond->GetValue(MASTER_NODES)[0].Id()<<std::endl;

	  if(found)
	    counter++;
		    
	}
		  

      if(counter == total_conditions){
	if( mEchoLevel >= 1 )
	  std::cout<<"   Condition Masters (ModelPart "<<rModelPart.Name()<<"): LOCATED ["<<counter<<"]"<<std::endl;
	found=true;
      }
      else{
	if( mEchoLevel >= 1 )
	  std::cout<<"   Condition Masters (ModelPart "<<rModelPart.Name()<<"): not LOCATED ["<<counter-total_conditions<<"]"<<std::endl;
	found=false;
      }

      if(counter!= composite_conditions)
	if( mEchoLevel >= 1 )
	  std::cout<<"   Condition Masters (ModelPart "<<rModelPart.Name()<<"): LOCATED ["<<counter<<"] COMPOSITE ["<<composite_conditions<<"] NO MATCH"<<std::endl;
			
      return found;

      KRATOS_CATCH( "" )
    }

    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const
    {
      return "BuildModelPartBoundaryProcess";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const
    {
      rOStream << "BuildModelPartBoundaryProcess";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const
    {
    }


    ///@}
    ///@name Friends
    ///@{


    ///@}

  protected:
    ///@name Protected static Member Variables
    ///@{


    ///@}
    ///@name Protected member Variables
    ///@{

    ModelPart& mrModelPart;

    std::string mModelPartName;

    int mEchoLevel;


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{

    //**************************************************************************
    //**************************************************************************


    bool UniqueSkinSearch( ModelPart& rModelPart )
    {

      KRATOS_TRY

      if( mEchoLevel > 0 ){
	std::cout<<" [ Initial Conditions : "<<rModelPart.Conditions().size()<<std::endl;
      }

      if( !rModelPart.Elements().size() || (rModelPart.Is(ACTIVE)) ){
	if( mEchoLevel > 0 ){
	  std::cout<<" [ Final Conditions   : "<<rModelPart.Conditions().size()<<std::endl;
	}
	return true;
      }
			
      //reset the boundary flag in all nodes and check if a remesh process has been performed
      bool any_node_to_erase = false;
      for(ModelPart::NodesContainerType::const_iterator in = rModelPart.NodesBegin(); in!=rModelPart.NodesEnd(); in++)
	{
	  in->Reset(BOUNDARY);

	  if( any_node_to_erase == false )
	    if( in->Is(TO_ERASE) )
	      any_node_to_erase = true;
	    
	}

      //swap conditions for a temporary use
      unsigned int ConditionId=1;
      ModelPart::ConditionsContainerType TemporaryConditions;

      //if there are no conditions check main modelpart mesh conditions
      if( !rModelPart.Conditions().size() ){

	for(ModelPart::ConditionsContainerType::iterator i_cond = rModelPart.GetParentModelPart()->ConditionsBegin(); i_cond!= rModelPart.GetParentModelPart()->ConditionsEnd(); i_cond++)
	  {
	    TemporaryConditions.push_back(*(i_cond.base()));
	    i_cond->SetId(ConditionId);
	    ConditionId++;
	  }

      }
      else{

	TemporaryConditions.reserve(rModelPart.Conditions().size());
	TemporaryConditions.swap(rModelPart.Conditions());

	//set consecutive ids in the mesh conditions
	if( any_node_to_erase ){
	  for(ModelPart::ConditionsContainerType::iterator i_cond = TemporaryConditions.begin(); i_cond!= TemporaryConditions.end(); i_cond++)
	    {
	      Geometry< Node<3> >& rConditionGeometry = i_cond->GetGeometry();
	      for( unsigned int i=0; i<rConditionGeometry.size(); i++ )
		{
		  if( rConditionGeometry[i].Is(TO_ERASE)){
		    i_cond->Set(TO_ERASE);
		    break;
		  }
		}
	  
	      i_cond->SetId(ConditionId);
	      ConditionId++;
	    }
	}
	else{
	  for(ModelPart::ConditionsContainerType::iterator i_cond = TemporaryConditions.begin(); i_cond!= TemporaryConditions.end(); i_cond++)
	    {

	      i_cond->SetId(ConditionId);
	      ConditionId++;
	    }
	}
	  
      }
         
      //control the previous mesh conditions
      std::vector<int> PreservedConditions( TemporaryConditions.size() + 1 );
      std::fill( PreservedConditions.begin(), PreservedConditions.end(), 0 );
		
      //build new skin for the Modelpart
      this->BuildCompositeConditions(rModelPart, TemporaryConditions, PreservedConditions, ConditionId);

      //add other conditions out of the skin space dimension
      this->AddOtherConditions(rModelPart, TemporaryConditions, PreservedConditions, ConditionId);
	
      return true;

            KRATOS_CATCH( "" )
    }

    //**************************************************************************
    //**************************************************************************


    virtual bool BuildCompositeConditions( ModelPart& rModelPart, ModelPart::ConditionsContainerType& rTemporaryConditions, std::vector<int>& rPreservedConditions, unsigned int& rConditionId )
    {

      KRATOS_TRY
      
      //properties to be used in the generation
      int number_properties = rModelPart.GetParentModelPart()->NumberOfProperties();
      Properties::Pointer properties = rModelPart.GetParentModelPart()->pGetProperties(number_properties-1);

      
      ModelPart::ElementsContainerType::iterator elements_begin  = rModelPart.ElementsBegin();
      ModelPart::ElementsContainerType::iterator elements_end    = rModelPart.ElementsEnd();

      rConditionId=0;
      for(ModelPart::ElementsContainerType::iterator ie = elements_begin; ie != elements_end ; ie++)
	{
	  
	  Geometry< Node<3> >& rGeometry = ie->GetGeometry();

	  const unsigned int dimension = rGeometry.WorkingSpaceDimension();
	  
	  if( rGeometry.FacesNumber() >= 3 ){ //3 or 4

	    //********************************************************************
	    /*each face is opposite to the corresponding node number so in 2D triangle
	      0 ----- 1 2
	      1 ----- 2 0
	      2 ----- 0 1
	    */

	    /*each face is opposite to the corresponding node number so in 3D tetrahedron
	      0 ----- 1 2 3
	      1 ----- 2 0 3
	      2 ----- 0 1 3
	      3 ----- 0 2 1
	    */
	    //********************************************************************

	    //finding boundaries and creating the "skin"	   
	    boost::numeric::ublas::matrix<unsigned int> lpofa; //connectivities of points defining faces
	    boost::numeric::ublas::vector<unsigned int> lnofa; //number of points defining faces
	 
	    WeakPointerVector<Element >& rE = ie->GetValue(NEIGHBOUR_ELEMENTS);
	    
	    //get matrix nodes in faces
	    rGeometry.NodesInFaces(lpofa);
	    rGeometry.NumberNodesInFaces(lnofa);
	    
	    //loop on neighbour elements of an element
	    unsigned int iface=0;
	    for(WeakPointerVector< Element >::iterator ne = rE.begin(); ne!=rE.end(); ne++)
	      {
		unsigned int NumberNodesInFace = lnofa[iface];

		if (ne->Id() == ie->Id())
		  {
		    //if no neighbour is present => the face is free surface
		    for(unsigned int j=1; j<=NumberNodesInFace; j++)
		      {
			rGeometry[lpofa(j,iface)].Set(BOUNDARY);
			//std::cout<<" node ["<<j<<"]"<<rGeometry[lpofa(j,iface)].Id()<<std::endl;
		      }

		    //1.- create geometry: points array and geometry type
		    Condition::NodesArrayType        FaceNodes;
		    Condition::GeometryType::Pointer ConditionVertices;
		      
		    FaceNodes.reserve(NumberNodesInFace);

		    for(unsigned int j=1; j<=NumberNodesInFace; j++)
		      {
			FaceNodes.push_back(rGeometry(lpofa(j,iface)));
		      }
				    
							
		    if( NumberNodesInFace == 2 ){
		      if( dimension == 2 )
			ConditionVertices = Condition::GeometryType::Pointer(new Line2D2< Node<3> >(FaceNodes) );
		      else
			ConditionVertices = Condition::GeometryType::Pointer(new Line3D2< Node<3> >(FaceNodes) );
		      
		    }
		    else if ( NumberNodesInFace == 3 ){
		      ConditionVertices = Condition::GeometryType::Pointer(new Triangle3D3< Node<3> >(FaceNodes) );
		    }

		    rConditionId +=1;
		    
		    //Create a composite condition
		    CompositeCondition::Pointer p_cond = CompositeCondition::Pointer(new CompositeCondition(rConditionId,ConditionVertices,properties) ); 

		    bool condition_found = false;
		    bool point_condition = false;
					       
		    // Search for existing conditions: start
		    for(ModelPart::ConditionsContainerType::iterator i_cond = rTemporaryConditions.begin(); i_cond!= rTemporaryConditions.end(); i_cond++)
		      {
			Geometry< Node<3> >& rConditionGeometry = i_cond->GetGeometry();
						
			condition_found = this->FindCondition(rConditionGeometry,rGeometry,lpofa,lnofa,iface);

			if( condition_found ){
			  
			  p_cond->AddChild(*(i_cond.base()));

			  rPreservedConditions[i_cond->Id()] += 1;		  

			  if( rConditionGeometry.PointsNumber() == 1 )
			    point_condition = true;
			}
			
		      }
		    // Search for existing conditions: end

		    if( !point_condition ){
		      // usually one MasterElement and one MasterNode for 2D and 3D simplex
		      // can be more than one in other geometries -> it has to be extended to that cases
		      
		      WeakPointerVector< Element >& MasterElements = p_cond->GetValue(MASTER_ELEMENTS);
		      MasterElements.push_back( Element::WeakPointer( *(ie.base()) ) );
		      p_cond->SetValue(MASTER_ELEMENTS,MasterElements);

		      WeakPointerVector< Node<3> >& MasterNodes = p_cond->GetValue(MASTER_NODES);
		      MasterNodes.push_back( Node<3>::WeakPointer( rGeometry(lpofa(0,iface)) ) );
		      p_cond->SetValue(MASTER_NODES,MasterNodes);
		    }

		    rModelPart.Conditions().push_back(Condition::Pointer(p_cond));
		    // Set new conditions: end
		    
		  } //end face condition

		iface+=1;
	      } //end loop neighbours

	  }
	}

      return true;

      KRATOS_CATCH( "" )
    }

    //**************************************************************************
    //**************************************************************************

    bool FindCondition(Geometry< Node<3> >& rConditionGeometry ,Geometry< Node<3> >& rGeometry, boost::numeric::ublas::matrix<unsigned int>& lpofa, boost::numeric::ublas::vector<unsigned int>& lnofa, unsigned int& iface)
    {
      
      KRATOS_TRY
      // not equivalent geometry sizes for boundary conditions:
      if( rConditionGeometry.size() != lnofa[iface] )
	return false;
      
      // line boundary condition:
      if( lnofa[iface] == 2 )
	{
	  if( (   rConditionGeometry[0].Id() == rGeometry[lpofa(1,iface)].Id() 
		  && rConditionGeometry[1].Id() == rGeometry[lpofa(2,iface)].Id() ) || 
	      (   rConditionGeometry[0].Id() == rGeometry[lpofa(2,iface)].Id() 
		  && rConditionGeometry[1].Id() == rGeometry[lpofa(1,iface)].Id() ) )
	  {	 
	    return true;
	  }
	  else
	  {
	    return false;
	  }
	    
	}
      
      //3D faces:
      if(  lnofa[iface] == 3 )
	{
	  if( (   rConditionGeometry[0].Id() == rGeometry[lpofa(1,iface)].Id() 
		  && rConditionGeometry[1].Id() == rGeometry[lpofa(2,iface)].Id()
		  && rConditionGeometry[2].Id() == rGeometry[lpofa(3,iface)].Id() ) || 
	      (   rConditionGeometry[0].Id() == rGeometry[lpofa(3,iface)].Id() 
		  && rConditionGeometry[1].Id() == rGeometry[lpofa(1,iface)].Id()
		  && rConditionGeometry[2].Id() == rGeometry[lpofa(2,iface)].Id() ) ||
	      (   rConditionGeometry[0].Id() == rGeometry[lpofa(2,iface)].Id() 
		  && rConditionGeometry[1].Id() == rGeometry[lpofa(3,iface)].Id()
		  && rConditionGeometry[2].Id() == rGeometry[lpofa(1,iface)].Id() ) )
	  {
	    return true;
	  }
	  else
	  {
	    return false;
	  }
	  
	}

      if(  lnofa[iface] > 3 )
	{
	  KRATOS_THROW_ERROR( std::logic_error, "Wrong Condition Number of Face Nodes",*this );
	}

      return false;

      KRATOS_CATCH( "" )
  
    }



    //**************************************************************************
    //**************************************************************************


    bool FindConditionID(ModelPart& rModelPart, Geometry< Node<3> >& rConditionGeometry)
    {
      KRATOS_TRY

      //check if the condition exists and belongs to the modelpart checking node Ids
      for(unsigned int i=0; i<rConditionGeometry.size(); i++)
	{
	  for(ModelPart::NodesContainerType::const_iterator in = rModelPart.NodesBegin(); in!=rModelPart.NodesEnd(); in++)
	    {			
	      if( rConditionGeometry[i].Id() == in->Id() )
		return true;
	    }
	}

      return false;

      KRATOS_CATCH( "" )
    }


    //**************************************************************************
    //**************************************************************************


    void PrintSkin(ModelPart& rModelPart)
    {

      KRATOS_TRY

      //PRINT SKIN:		
      std::cout<<" CONDITIONS: geometry nodes ("<<rModelPart.Conditions().size()<<")"<<std::endl;

      ConditionsContainerType& rCond = rModelPart.Conditions();
      for(ConditionsContainerType::iterator i_cond = rCond.begin(); i_cond!= rCond.end(); i_cond++)
	{
			
	  Geometry< Node<3> >& rConditionGeometry = i_cond->GetGeometry();
	  std::cout<<"["<<i_cond->Id()<<"]:"<<std::endl;
	  //i_cond->PrintInfo(std::cout);
	  std::cout<<"( ";
	  for(unsigned int i = 0; i < rConditionGeometry.size(); i++)
	    {
	      std::cout<< rConditionGeometry[i].Id()<<", ";
	    }
	  std::cout<<" ): ";

	  i_cond->GetValue(MASTER_ELEMENTS)[0].PrintInfo(std::cout);
							
	  std::cout<<std::endl;

	}
      std::cout<<std::endl;

      KRATOS_CATCH( "" )

    }

    //**************************************************************************
    //**************************************************************************


    bool AddOtherConditions(ModelPart& rModelPart, ModelPart::ConditionsContainerType& rTemporaryConditions, std::vector<int>& rPreservedConditions, unsigned int& rConditionId)
    {

      KRATOS_TRY

      unsigned int counter = 0;

      //add all previous conditions not found in the skin search:
      for(ModelPart::ConditionsContainerType::iterator i_cond = rTemporaryConditions.begin(); i_cond!= rTemporaryConditions.end(); i_cond++)
	{	

	  if( rPreservedConditions[i_cond->Id()] == 0 ){ //I have not used the condition and any node of the condition

	    if( this->CheckAcceptedCondition( rModelPart, *i_cond ) ){

	      Geometry< Node<3> >& rGeometry = i_cond->GetGeometry();

	      Condition::NodesArrayType FaceNodes;

	      FaceNodes.reserve(rGeometry.size() );

	      for(unsigned int j=0; j<rGeometry.size(); j++)
		{
		  FaceNodes.push_back(rGeometry(j));
		}

	      rPreservedConditions[i_cond->Id()] += 1;

	      rConditionId +=1;

	      Condition::Pointer p_cond = i_cond->Clone(rConditionId, FaceNodes);
	      //p_cond->Data() = i_cond->Data();

	      this->AddConditionToModelPart(rModelPart,p_cond);

	      counter++;

	    }

	  }
	}
	
      std::cout<<"   recovered conditions "<<counter<<std::endl;

      //control if all previous conditions have been added:
      bool all_assigned = true;
      unsigned int lost_conditions = 0;
      for(unsigned int i=1; i<rPreservedConditions.size(); i++)
	{
	  if( rPreservedConditions[i] == 0 ){	    
	    all_assigned = false;
	    lost_conditions++;
	  }
	}


      if( mEchoLevel >= 1 ){

	std::cout<<"   Final Conditions   : "<<rModelPart.NumberOfConditions()<<std::endl;

	if(all_assigned == true)
	  std::cout<<"   ALL_PREVIOUS_CONDITIONS_RELOCATED "<<std::endl;
	else
	  std::cout<<"   SOME_PREVIOUS_CONDITIONS_ARE_LOST [lost_conditions:"<<lost_conditions<<"]"<<std::endl;
      }

      return all_assigned;

      KRATOS_CATCH( "" )

    }

    //**************************************************************************
    //**************************************************************************


    virtual bool CheckAcceptedCondition(ModelPart& rModelPart, Condition& rCondition)
    {
      KRATOS_TRY
	
      Geometry< Node<3> >& rGeometry = rCondition.GetGeometry();
	
      return FindConditionID(rModelPart, rGeometry);	

      KRATOS_CATCH( "" )
    }


    //**************************************************************************
    //**************************************************************************


    virtual void AddConditionToModelPart(ModelPart& rModelPart, Condition::Pointer pCondition)
    {
      KRATOS_TRY
	
      rModelPart.AddCondition(pCondition);
      //rModelPart.Conditions().push_back(pCondition);

      KRATOS_CATCH( "" )
    }


    
    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{


    ///@}
    ///@name Protected LifeCycle
    ///@{


    ///@}

  private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{


    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    //**************************************************************************
    //**************************************************************************

    void SetComputingModelPart()
    {

      KRATOS_TRY

      std::string ComputingModelPartName;
      for(ModelPart::SubModelPartIterator i_mp= mrModelPart.SubModelPartsBegin(); i_mp!=mrModelPart.SubModelPartsEnd(); i_mp++)
	{
	  if( i_mp->Is(ACTIVE) )
	    ComputingModelPartName = i_mp->Name();
	}
      
      ModelPart& rModelPart = mrModelPart.GetSubModelPart(ComputingModelPartName);

      if( mEchoLevel >= 1 ){
	std::cout<<" ["<<ComputingModelPartName<<" :: CONDITIONS [OLD:"<<rModelPart.NumberOfConditions();
      }

      ModelPart::ConditionsContainerType KeepConditions;

      for(ModelPart::SubModelPartIterator i_mp= mrModelPart.SubModelPartsBegin(); i_mp!=mrModelPart.SubModelPartsEnd(); i_mp++)
	{
			    
	  if( i_mp->NumberOfElements() && ComputingModelPartName != i_mp->Name() ){ //conditions of model_parts with elements only

	    for(ModelPart::ConditionsContainerType::iterator i_cond = i_mp->ConditionsBegin() ; i_cond != i_mp->ConditionsEnd() ; i_cond++)
	      {
		// i_cond->PrintInfo(std::cout);
		// std::cout<<" -- "<<std::endl;
	      
		KeepConditions.push_back(*(i_cond.base()));

		// KeepConditions.back().PrintInfo(std::cout);
		// std::cout<<std::endl;
	      
	      }
	  }
	}
      
      rModelPart.Conditions().swap(KeepConditions);
      
      if( mEchoLevel >= 1 ){
	std::cout<<" / NEW:"<<rModelPart.NumberOfConditions()<<"] "<<std::endl;
      }
      

      KRATOS_CATCH( "" )

    }


    //**************************************************************************
    //**************************************************************************

    void SetMainModelPartConditions()
    {

      KRATOS_TRY

      if( mEchoLevel >= 1 ){
	std::cout<<" ["<<mrModelPart.Name()<<" :: CONDITIONS [OLD:"<<mrModelPart.NumberOfConditions();
      }

      //contact conditions are located on Mesh_0
      ModelPart::ConditionsContainerType KeepConditions;

      unsigned int condId=1;
      if( mModelPartName == mrModelPart.Name() ){

	for(ModelPart::SubModelPartIterator i_mp= mrModelPart.SubModelPartsBegin(); i_mp!=mrModelPart.SubModelPartsEnd(); i_mp++)
	  {
	    if( !(i_mp->Is(ACTIVE)) && !(i_mp->Is(CONTACT)) ){
	      //std::cout<<" ModelPartName "<<i_mp->Name()<<" conditions "<<i_mp->NumberOfConditions()<<std::endl;
	      for(ModelPart::ConditionsContainerType::iterator i_cond = i_mp->ConditionsBegin() ; i_cond != i_mp->ConditionsEnd() ; i_cond++)
		{
		  // i_cond->PrintInfo(std::cout);
		  // std::cout<<" -- "<<std::endl;

		  KeepConditions.push_back(*(i_cond.base()));
		  KeepConditions.back().SetId(condId);
		  condId+=1;
		
		  // KeepConditions.back().PrintInfo(std::cout);
		  // std::cout<<std::endl;
		
		}
	    }
	  }
      }
      
      for(ModelPart::ConditionsContainerType::iterator i_cond = mrModelPart.ConditionsBegin(); i_cond!= mrModelPart.ConditionsEnd(); i_cond++)
	{
	  if(i_cond->Is(CONTACT)){
	    KeepConditions.push_back(*(i_cond.base()));
	    KeepConditions.back().SetId(condId);
	    condId+=1;

	    //std::cout<<" -- "<<std::endl;
	    //KeepConditions.back().PrintInfo(std::cout);
	    //std::cout<<std::endl;
			  
	  }
		      
	}
      
 
      mrModelPart.Conditions().swap(KeepConditions);

      if( mEchoLevel >= 1 ){
	std::cout<<" / NEW:"<<mrModelPart.NumberOfConditions()<<"] "<<std::endl;
      }

      KRATOS_CATCH( "" )
    }



    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    BuildModelPartBoundaryProcess& operator=(BuildModelPartBoundaryProcess const& rOther);

    /// Copy constructor.
    //BuildModelPartBoundaryProcess(BuildModelPartBoundaryProcess const& rOther);


    ///@}

  }; // Class BuildModelPartBoundaryProcess

  ///@}

  ///@name Type Definitions
  ///@{


  ///@}
  ///@name Input and output
  ///@{


  /// input stream function
  inline std::istream& operator >> (std::istream& rIStream,
				    BuildModelPartBoundaryProcess& rThis);

  /// output stream function
  inline std::ostream& operator << (std::ostream& rOStream,
				    const BuildModelPartBoundaryProcess& rThis)
  {
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
  }
  ///@}


}  // namespace Kratos.

#endif // KRATOS_BUILD_MODEL_PART_BOUNDARY_PROCESS_H_INCLUDED  defined 