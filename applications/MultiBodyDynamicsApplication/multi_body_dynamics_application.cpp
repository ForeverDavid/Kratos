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



// System includes


// External includes 


// Project includes
#include "includes/define.h"
#include "includes/variables.h"
#include "includes/constitutive_law.h"

#include "geometries/triangle_3d_3.h"
#include "geometries/triangle_3d_6.h"
#include "geometries/quadrilateral_3d_4.h"
#include "geometries/quadrilateral_3d_8.h"
#include "geometries/quadrilateral_3d_8.h"
#include "geometries/quadrilateral_3d_9.h"
#include "geometries/line_2d_2.h"
#include "geometries/line_2d_3.h"
#include "geometries/line_3d_2.h"
#include "geometries/line_3d_3.h"
#include "geometries/point_2d.h"
#include "geometries/point_3d.h"
#include "geometries/prism_3d_6.h"

#include "multi_body_dynamics_application.h"
#include "includes/variables.h"


namespace Kratos
{
	//Example
// 	KRATOS_CREATE_VARIABLE(double, AUX_MESH_VAR)
//	KRATOS_CREATE_VARIABLE(double, IS_INTERFACE);
//	KRATOS_CREATE_VARIABLE(double, NODAL_AREA);
//

 	KratosMultiBodyDynamicsApplication::KratosMultiBodyDynamicsApplication()
 	{}
 	
 	void KratosMultiBodyDynamicsApplication::Register()
 	{
 		// calling base class register to register Kratos components
 		KratosApplication::Register();
 		std::cout << "KRATOS __   __     ____  _ ___          __           " << std::endl;
 		std::cout << "      /  | / /_ __/ / /_(_) _ )___  ___/ /_ __       " << std::endl;
 		std::cout << "     / /|_/ / // / / __/ / _  / _ \\/ _  / // /       " << std::endl;
 		std::cout << "    /_/  /_/\\_,_/_/\\__/_/____/\\___/\\_,_/\\ , /        " << std::endl;
 		std::cout << "                                       /___/ DYNAMICS" << std::endl;
                
// 		KRATOS_REGISTER_VARIABLE( AUX_MESH_VAR )
// 		KRATOS_REGISTER_VARIABLE(IS_INTERFACE);
// 		KRATOS_REGISTER_VARIABLE(NODAL_AREA);

 
 	}

}  // namespace Kratos.


