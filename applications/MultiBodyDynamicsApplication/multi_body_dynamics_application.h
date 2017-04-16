// KRATOS __   __     ____  _ ___          __           
//       /  | / /_ __/ / /_(_) _ )___  ___/ /_ __       
//      / /|_/ / // / / __/ / _  / _ \/ _  / // /       
//     /_/  /_/\_,_/_/\__/_/____/\___/\_,_/\ , /        
//                                        /___/ DYNAMICS            
// License:		 	BSD License
//					license: license.txt
//
//  Main author:	Erich Wehrle
// 


#if !defined(KRATOS_MULTI_BODY_DYNAMICS_APPLICATION_H_INCLUDED )
#define  KRATOS_MULTI_BODY_DYNAMICS_APPLICATION_H_INCLUDED



// System includes
#include <string>
#include <iostream> 


// External includes 


// Project includes
#include "includes/define.h"
#include "includes/kratos_application.h"


#include "includes/variables.h"


namespace Kratos
{

	///@name Kratos Globals
	///@{ 

	// Variables definition 
//	KRATOS_DEFINE_VARIABLE(double, AUX_MESH_VAR )
//	KRATOS_DEFINE_VARIABLE(double, IS_INTERFACE)
//	KRATOS_DEFINE_VARIABLE(double, NODAL_AREA)


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
	class KratosMultiBodyDynamicsApplication : public KratosApplication
	{
	public:
		///@name Type Definitions
		///@{
		

		/// Pointer definition of KratosMultiBodyDynamicsApplication
		KRATOS_CLASS_POINTER_DEFINITION(KratosMultiBodyDynamicsApplication);

		///@}
		///@name Life Cycle 
		///@{ 

		/// Default constructor.
		KratosMultiBodyDynamicsApplication();

		/// Destructor.
		virtual ~KratosMultiBodyDynamicsApplication(){}


		///@}
		///@name Operators 
		///@{


		///@}
		///@name Operations
		///@{

		virtual void Register();



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
			return "KratosMultiBodyDynamicsApplication";
		}

		/// Print information about this object.
		virtual void PrintInfo(std::ostream& rOStream) const
		{
			rOStream << Info();
			PrintData(rOStream);
		}

		///// Print object's data.
      virtual void PrintData(std::ostream& rOStream) const
      {
      	KRATOS_WATCH("in my application");
      	KRATOS_WATCH(KratosComponents<VariableData>::GetComponents().size() );
		rOStream << "Variables:" << std::endl;
		KratosComponents<VariableData>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Elements:" << std::endl;
		KratosComponents<Element>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Conditions:" << std::endl;
		KratosComponents<Condition>().PrintData(rOStream);
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



		//       static const ApplicationCondition  msApplicationCondition; 

		///@} 
		///@name Member Variables 
		///@{ 
// 		const Elem2D   mElem2D; 
// 		const Elem3D   mElem3D; 


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
		KratosMultiBodyDynamicsApplication& operator=(KratosMultiBodyDynamicsApplication const& rOther);

		/// Copy constructor.
		KratosMultiBodyDynamicsApplication(KratosMultiBodyDynamicsApplication const& rOther);


		///@}    

	}; // Class KratosMultiBodyDynamicsApplication 

	///@} 


	///@name Type Definitions       
	///@{ 


	///@} 
	///@name Input and output 
	///@{ 

	///@} 


}  // namespace Kratos.

#endif // KRATOS_MULTI_BODY_DYNAMICS_APPLICATION_H_INCLUDED  defined 


