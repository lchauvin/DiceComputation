/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Laurent Chauvin, Brigham and Women's
  Hospital. The project was supported by grants 5P01CA067165,
  5R01CA124377, 5R01CA138586, 2R44DE019322, 7R01CA124377,
  5R42CA137886, 8P41EB015898

  ==============================================================================*/

// .NAME vtkSlicerDiceComputationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerDiceComputationLogic_h
#define __vtkSlicerDiceComputationLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkPolyData.h"

// STD includes
#include <cstdlib>

#include "vtkSlicerDiceComputationModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_DICECOMPUTATION_MODULE_LOGIC_EXPORT vtkSlicerDiceComputationLogic :
public vtkSlicerModuleLogic
{
public:

  static vtkSlicerDiceComputationLogic *New();
  vtkTypeMacro(vtkSlicerDiceComputationLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  void ComputeDiceCoefficient(std::vector<vtkMRMLScalarVolumeNode*> labelMaps,
                              std::vector<std::vector<double> >& resultsArray);
  void ComputeHausdorffDistance(std::vector<vtkPolyData*> polyData,
				std::vector<std::vector<double> >& resultsArray);

protected:
  vtkSlicerDiceComputationLogic();
  virtual ~vtkSlicerDiceComputationLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  int ComputeIntersection(vtkMRMLScalarVolumeNode* map1,
                          vtkMRMLScalarVolumeNode* map2);
  int GetNumberOfPixels(vtkMRMLScalarVolumeNode* map);
  int GetNumberOfPixels(vtkImageData* imData);

private:

  vtkSlicerDiceComputationLogic(const vtkSlicerDiceComputationLogic&); // Not implemented
  void operator=(const vtkSlicerDiceComputationLogic&);               // Not implemented
};

#endif
