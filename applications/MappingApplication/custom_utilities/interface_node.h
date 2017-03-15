//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher, Jordi Cotela

#if !defined(KRATOS_INTERFACE_NODE_INCLUDED_H_INCLUDED )
#define  KRATOS_INTERFACE_NODE_INCLUDED_H_INCLUDED



// System includes
#include <string>
#include <iostream>


// External includes


// Project includes
#include "includes/define.h"
#include "interface_object.h"


namespace Kratos
{
  ///@addtogroup ApplicationNameApplication
  ///@{

  ///@name Kratos Globals
  ///@{

  ///@}
  ///@name Type Definitions
  ///@{

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
  class InterfaceNode : public InterfaceObject
    {
    public:
      ///@name Type Definitions
      ///@{

      /// Pointer definition of InterfaceNode
      KRATOS_CLASS_POINTER_DEFINITION(InterfaceNode);

      ///@}
      ///@name Life Cycle
      ///@{

      /// Default constructor.
      InterfaceNode(Node<3>& i_node) : InterfaceObject(i_node),  m_p_node(&i_node) {  }

      /// Destructor.
      virtual ~InterfaceNode() { }


      ///@}
      ///@name Operators
      ///@{


      ///@}
      ///@name Operations
      ///@{

      bool EvaluateResult(array_1d<double, 3> global_coords, double& min_distance,
                          double distance, array_1d<double,2>& local_coords,
                          std::vector<double>& shape_function_values) override { // I am an object in the bins
          bool is_closer = false;

          if (distance < min_distance){
              min_distance = distance;
              is_closer = true;
          }

          return is_closer;
      }

      // Scalars
      double GetObjectValue(const Variable<double>& variable,
                            const Kratos::Flags& options) override {
          if (options.Is(MapperFlags::NON_HISTORICAL_DATA)) {
              return m_p_node->GetValue(variable);
          } else {
              return m_p_node->FastGetSolutionStepValue(variable);
          }
      }

      void SetObjectValue(const Variable<double>& variable,
                          const double value,
                          const Kratos::Flags& options,
                          const double factor) override {
          if (options.Is(MapperFlags::NON_HISTORICAL_DATA)) {
              if (options.Is(MapperFlags::ADD_VALUES)) {
                  double old_value = m_p_node->GetValue(variable);
                  m_p_node->SetValue(variable, old_value + value * factor);
              } else {
                  m_p_node->SetValue(variable, value * factor);
              }
          } else { // Variable with history
              if (options.Is(MapperFlags::ADD_VALUES)) {
                  m_p_node->FastGetSolutionStepValue(variable) += value * factor;
              } else {
                  m_p_node->FastGetSolutionStepValue(variable) = value * factor;
              }
          }   
      }

      // Vectors
      array_1d<double,3> GetObjectValue(const Variable< array_1d<double,3> >& variable,
                                        const Kratos::Flags& options) override {
          if (options.Is(MapperFlags::NON_HISTORICAL_DATA)) {
              return m_p_node->GetValue(variable);
          } else {
              return m_p_node->FastGetSolutionStepValue(variable);
          }
      }

      void SetObjectValue(const Variable< array_1d<double,3> >& variable,
                          const array_1d<double,3>& value,
                          const Kratos::Flags& options,
                          const double factor) override {
          if (options.Is(MapperFlags::NON_HISTORICAL_DATA)) {
              if (options.Is(MapperFlags::ADD_VALUES)) {
                  array_1d<double,3> old_value = m_p_node->GetValue(variable);
                  m_p_node->SetValue(variable, old_value + value * factor);
              } else {
                  m_p_node->SetValue(variable, value * factor);
              }
          } else {
              if (options.Is(MapperFlags::ADD_VALUES)) {
                  m_p_node->FastGetSolutionStepValue(variable) += value * factor;
              } else {
                  m_p_node->FastGetSolutionStepValue(variable) = value * factor;
              }
          }
      }

      // Functions used for Debugging
      int GetObjectId() override {
          return m_p_node->Id();
      }

      void PrintMatchInfo(const int comm_rank) override {
          array_1d<double, 3> neighbor_coordinates = m_p_node->GetValue(NEIGHBOR_COORDINATES);
          std::cout << "InteraceNode; Rank " << comm_rank
                    << " , Id = " << GetObjectId()
                    << "; Coordinates = [" 
                    << this->X() << " "
                    << this->Y() << " " 
                    << this->Z() << "], Neighbor = ["
                    << neighbor_coordinates[0] << " " 
                    << neighbor_coordinates[1] << " "
                    << neighbor_coordinates[2] << "]" << std::endl;
      }

      void WriteCoordinatesToVariable() override {
          // This function writes the coordinates of the InterfaceObject 
          // to the variable "NEIGHBOR_COORDINATES", for debugging
          array_1d<double,3> neighbor_coordinates;
          neighbor_coordinates[0] = this->X();
          neighbor_coordinates[1] = this->Y();
          neighbor_coordinates[2] = this->Z();
          m_p_node->SetValue(NEIGHBOR_COORDINATES, neighbor_coordinates);
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
	       std::stringstream buffer;
         buffer << "InterfaceNode" ;
         return buffer.str();
      }

      /// Print information about this object.
      virtual void PrintInfo(std::ostream& rOStream) const {rOStream << "InterfaceNode";}

      /// Print object's data.
      virtual void PrintData(std::ostream& rOStream) const {}


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


      ///@}
      ///@name Protected Operators
      ///@{


      ///@}
      ///@name Protected Operations
      ///@{


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

      Node<3>* m_p_node;

      ///@}
      ///@name Private Operators
      ///@{


      ///@}
      ///@name Private Operations
      ///@{


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
      InterfaceNode& operator=(InterfaceNode const& rOther);

    //   /// Copy constructor.
    //   InterfaceNode(InterfaceNode const& rOther){}


      ///@}

    }; // Class InterfaceNode

  ///@}

  ///@name Type Definitions
  ///@{


  ///@}
  ///@name Input and output
  ///@{


  /// input stream function
  inline std::istream& operator >> (std::istream& rIStream,
				    InterfaceNode& rThis)
  {
      return rIStream;
  }

  /// output stream function
  inline std::ostream& operator << (std::ostream& rOStream,
				    const InterfaceNode& rThis)
  {
      rThis.PrintInfo(rOStream);
      rOStream << std::endl;
      rThis.PrintData(rOStream);

      return rOStream;
  }
  ///@}

  ///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_INTERFACE_NODE_INCLUDED_H_INCLUDED  defined
