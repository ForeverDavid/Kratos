from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7
#import kratos core and applications
import KratosMultiphysics
import KratosMultiphysics.PfemBaseApplication as KratosPfemBase

# Check that KratosMultiphysics was imported in the main script
KratosMultiphysics.CheckForPreviousImport()

def CreateMeshingDomain(main_model_part, custom_settings):
    return MeshingDomain(main_model_part, custom_settings)

class MeshingDomain:
    
    ##constructor. the constructor shall only take care of storing the settings 
    ##and the pointer to the main_model part.
    ##
    ##real construction shall be delayed to the function "Initialize" which 
    ##will be called once the modeler is already filled
    def __init__(self, main_model_part, custom_settings): 
        
        self.main_model_part = main_model_part    
        
        ##settings string in json format
        default_settings = KratosMultiphysics.Parameters("""
        {
            "mesh_id": 1,
            "domain_size": 2,
            "echo_level": 0,
            "alpha_shape": 2.4,
            "offset_factor": 0.0,
            "meshing_strategy":{
               "strategy_type": "meshing_strategy",
               "remesh": False,
               "refine": False,
               "reconnect": False,
               "transfer": False,
               "constrained": False,
               "mesh_smoothing": False,
               "variables_smoothing": False,
               "elemental_variables_to_smooth":[ "DETERMINANT_F" ],
               "reference_element": "Element2D3N"
               "reference_condition": "CompositeCondition2D3N"
            }
            "spatial_bounding_box":{
               "refine_in_box_only": False,
               "radius": 0.0,
               "center": [0.0, 0.0, 0.0],
               "velocity": [0.0, 0.0, 0.0]
            },
            "refining_parameters":{
               "critical_size": 0.0,
               "threshold_variable": PLASTIC_STRAIN,
               "reference_threshold" : 0.0,
               "error_variable": NORM_ISOCHORIC_STRESS,
               "reference_error" : 0.0,
               "add_nodes": True,
               "insert_nodes": False,
               "remove_nodes": {
                   "apply_removal": False,
                   "on_distance": False,
                   "on_threshold": False,
                   "on_error": False
               },
               "remove_boundary": {
                   "apply_removal": False,
                   "on_distance": False,
                   "on_threshold": False,
                   "on_error": False
               },
               "refine_elements": {
                   "apply_refinement": False,
                   "on_distance": False,
                   "on_threshold": False,
                   "on_error": False
               },
               "refine_boundary": {
                   "apply_refinement": False,
                   "on_distance": False,
                   "on_threshold": False,
                   "on_error": False
               },              
               "refining_box":{
                   "radius": 0.0,
                   "center": [0.0, 0.0, 0.0],
                   "velocity": [0.0, 0.0, 0.0]
               }
            },            
            "elemental_variables_to_transfer":[ "CAUCHY_STRESS_VECTOR", "DEFORMATION_GRADIENT" ]
        }
        """)
        
        ##overwrite the default settings with user-provided parameters
        self.settings = custom_settings
        self.settings.ValidateAndAssignDefaults(default_settings)
        
        #construct the solving strategy
        meshing_module = __import__(self.settings["meshing_strategy"]["strategy_type"].GetString())
        self.MeshingStrategy = meshing_module.CreateMeshingStrategy(self.main_model_part, self.settings["meshing_strategy"])

        print("Construction of Mesh Modeler finished")
        

    
    def Initialize(self, imposed_walls):

        print("::[Mesh Modeler]:: -START-")
        
        self.domain_size = self.settings["domain_size"].GetInt()
        self.mesh_id     = self.settings["mesh_id"].GetInt()

        # Set MeshingParameters
        self.SetMeshingParameters()
        
        # Meshing Stratety
        self.MeshingStrategy.Initialize(self.MeshingParameters, imposed_walls, self.domain_size, self.mesh_id)
        
        print("::[Mesh Modeler]:: -END- ")

        
    #### 
    
    def SetInfoParameters(self):

        # Create InfoParameters        
        self.InfoParameters  = KratosPfemBase.InfoParameters()
        self.InfoParameters.Initialize()
        
        
    def SetTransferParameters(self):
        
        # Create TransferParameters
        self.TransferParameters = KratosPfemBase.TransferParameters()
        transfer_variables = self.settings["elemental_variables_to_transfer"]
        for variable in transfer_variables:
            self.TransferParameters.SetVariable(globals()[variable])


    def SetRefiningParameters(self):
        
        # Create RefiningParameters
        self.RefiningParameters = KratosPfemBase.RefiningParameters()
        self.RefiningParameters.Initialize()

        # parameters
        self.RefiningParameters.SetAlphaParameter(self.settings["alpha_shape"].GetDouble())
        
        critical_mesh_size = self.settings["refining_parameters"]["critical_mesh_size"].GetDouble()

        # set mesh refinement based on wall tip discretization size
        # if(parameters["TipRadiusRefine"]):
            # tip arch opening (in degrees = 5-7.5-10)
            #tool_arch_opening = 12
            # tip surface length
            #tool_arch_length = tool_arch_opening * (3.1416 / 180.0)
            # critical mesh size based on wall tip
            #critical_mesh_size = tool_arch_length * parameters["CriticalTipRadius"]
            
        critical_mesh_size = critical_mesh_size
        critical_mesh_side = critical_mesh_size * 3
            
        self.RefiningParameters.SetCriticalRadius(critical_mesh_size)                       
        self.RefiningParameters.SetCriticalSide(critical_mesh_side)
            
        # set mesh refinement in box
        size = self.domain_size
        if(self.settings["spatial_bounding_box"]["refine_in_box_only"].GetBool()):               
            radius   = self.settings["spatial_bounding_box"]["radius"].GetDouble()
            center   = Vector(size)
            velocity = Vector(size)
            
            for i in range(0, size):
                center[i]   = self.settings["spatial_bounding_box"]["center"][i].GetDouble()
                velocity[i] = self.settings["spatial_bounding_box"]["velocity"][i].GetDouble()
                
        refining_box = KratosPfemBase.SpatialBoundingBox(center, radius, velocity)
        self.RefiningParameters.SetRefiningBox(refining_box)

        self.RefiningParameters.SetThresholdVariable(globals()[self.settings["refining_parameters"]["threshold_variable"].GetString()])
        self.RefiningParameters.SetReferenceThreshold(self.settings["refining_parameters"]["reference_threshold"].GetDouble())
                
        self.RefiningParameters.SetErrorVariable(globals()[self.settings["refining_parameters"]["error_variable"].GetString()])
        self.RefiningParameters.SetReferenceError(self.settings["refining_parameters"]["reference_error"].GetDouble())


        removing_options = KratosMultiphysics.Flags()

        refining_options.Set(ModelerUtilities.REFINE, self.settings["refine"].GetBool())

        #remove nodes
        remove_nodes = self.settings["refining_parameters"]["remove_nodes"]
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS, remove_nodes["apply_refinement"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS_ON_DISTANCE, remove_nodes["on_distance"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS_ON_ERROR, remove_nodes["on_error"].GetBool())  
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS_ON_THRESHOLD, remove_nodes["on_threshold"].GetBool())  

        #remove boundary
        remove_boundary = self.settings["refining_parameters"]["remove_boundary"]
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY, remove_boundary["apply_refinement"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY_ON_DISTANCE, remove_boundary["on_distance"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY_ON_ERROR, remove_boundary["on_error"].GetBool())  
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY_ON_THRESHOLD, remove_boundary["on_threshold"].GetBool())  

        refining_options = KratosMultiphysics.Flags()
        refining_options.Set(ModelerUtilities.REFINE_ADD_NODES, self.settings["refining_parameters"]["add_nodes"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_INSERT_NODES, self.settings["refining_parameters"]["insert_nodes"].GetBool())

        #refine elements
        refine_elements = self.settings["refining_parameters"]["refine_elements"]
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS, refine_elements["apply_refinement"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS_ON_DISTANCE, refine_elements["on_distance"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_ELEMENTS_ON_THRESHOLD, refine_elements["on_threshold"].GetBool())  

        #refine boundary
        refine_boundary = self.settings["refining_parameters"]["refine_boundary"]
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY, refine_boundary["apply_refinement"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY_ON_DISTANCE, refine_boundary["on_distance"].GetBool())
        refining_options.Set(ModelerUtilities.REFINE_BOUNDARY_ON_THRESHOLD, refine_boundary["on_threshold"].GetBool())  
        
        self.RefiningParameters.SetRefiningOptions(refining_options)
        self.RefiningParameters.SetRemovingOptions(removing_options)


    def SetMeshingParameters(self):
              
        # Create MeshingParameters
        self.MeshingParameters = KratosPfemBase.MeshingParameters()
        self.MeshingParameters.Initialize()

        self.MeshingParameters.SetAlphaParameter(self.settings["alpha_shape"].GetDouble())
        self.MeshingParameters.SetOffsetFactor(self.settings["offset_factor"].GetDouble())
 
        self.SetInfoParameters();
        self.SetTransferParameters();
        self.SetRefiningParameters();
        
        self.MeshingParameters.SetInfoParameters(self.InfoParameters)
        self.MeshingParameters.SetTransferParameters(self.TransferParameters)
        self.MeshingParameters.SetRefiningParameters(self.RefiningParameters)
        


    def ExecuteMeshing(self):
        
        self.MeshingStrategy.GenerateMesh()
        