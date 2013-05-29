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

// DiceComputation Logic includes
#include "vtkSlicerDiceComputationLogic.h"

// MRML includes

// VTK includes
#include <vtkNew.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDiceComputationLogic);

//----------------------------------------------------------------------------
vtkSlicerDiceComputationLogic::vtkSlicerDiceComputationLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerDiceComputationLogic::~vtkSlicerDiceComputationLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic
::ComputeDiceCoefficient(int numberOfLabelMaps, 
			 std::vector<vtkMRMLScalarVolumeNode*> labelMaps,
			 std::vector<std::vector<double> >& resultsArray)
{  
  
  // Clean previous results and resize array
  resultsArray.clear();
  resultsArray.resize(numberOfLabelMaps);
  for (int i = 0; i < numberOfLabelMaps; i++)
    {
    resultsArray[i].clear();
    resultsArray[i].resize(numberOfLabelMaps);
    }

  // TODO
  // if i = j
    // resultsArray[i][j] = 1.0

  // else

    // Compute dice coefficient for each label map
    // int numberOfPixelIntersection = ComputeIntersection(label1, label2);
    // int pixelsLabel1 = GetNumberOfPixels(label1);
    // int pixelsLabel2 = GetNumberOfPixels(label2);
    // double diceCoeff = 2*numberOfPixelIntersection / (pixelsLabel1 + pixelsLabel2);

    // Write results in array (symmetric)
    // resultsArray[i][j] = resultsArray[j][i] = diceCoeff;
}
