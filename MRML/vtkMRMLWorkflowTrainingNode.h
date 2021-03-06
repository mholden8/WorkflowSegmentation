
#ifndef __vtkMRMLWorkflowTrainingNode_h
#define __vtkMRMLWorkflowTrainingNode_h

// Standard Includes
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// VTK includes
#include "vtkObject.h"
#include "vtkObjectBase.h"
#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"

#include "vtkMRMLStorableNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkMRMLWorkflowTrainingStorageNode.h"

// Workflow Segmentation includes
#include "vtkSlicerWorkflowSegmentationModuleMRMLExport.h"
#include "vtkLabelVector.h"
#include "vtkMarkovModelRT.h"

// This class stores a vector of values and a string label
class VTK_SLICER_WORKFLOWSEGMENTATION_MODULE_MRML_EXPORT 
vtkMRMLWorkflowTrainingNode : public vtkMRMLStorableNode
{
public:
  vtkTypeMacro( vtkMRMLWorkflowTrainingNode, vtkMRMLStorableNode );

  // Standard MRML node methods  
  static vtkMRMLWorkflowTrainingNode* New();  
  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "WorkflowTraining"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );

  // To use the storage node
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode() { return vtkMRMLWorkflowTrainingStorageNode::New(); };
  bool GetModifiedSinceRead() { return ( this->GetMTime() > this->GetStoredTime() ); };
  virtual void UpdateScene( vtkMRMLScene *scene ) { Superclass::UpdateScene(scene); };
  
protected:

  // Constructor/desctructor
  vtkMRMLWorkflowTrainingNode();
  virtual ~vtkMRMLWorkflowTrainingNode();
  vtkMRMLWorkflowTrainingNode ( const vtkMRMLWorkflowTrainingNode& ); // Required to prevent linking error
  void operator=( const vtkMRMLWorkflowTrainingNode& ); // Required to prevent linking error
  
public:

  // Getters/setters for properties
  std::vector< vtkSmartPointer< vtkLabelVector > > GetPrinComps();
  void SetPrinComps( std::vector< vtkSmartPointer< vtkLabelVector > > newPrinComps );

  std::vector< vtkSmartPointer< vtkLabelVector > > GetCentroids();
  void SetCentroids( std::vector< vtkSmartPointer< vtkLabelVector > > newCentroids );

  vtkLabelVector* GetMean();
  void SetMean( vtkLabelVector* newMean );

  vtkMarkovModelRT* GetMarkov();
  void SetMarkov( vtkMarkovModelRT* newMarkov );
  
  // Read/Write from/to file
  std::string ToXMLString( vtkIndent indent );
  void FromXMLElement( vtkXMLDataElement* element );

protected:

  std::vector< vtkSmartPointer< vtkLabelVector > > PrinComps;
  std::vector< vtkSmartPointer< vtkLabelVector > > Centroids;
  
  vtkSmartPointer< vtkLabelVector > Mean;
  vtkSmartPointer< vtkMarkovModelRT > Markov;

};

#endif