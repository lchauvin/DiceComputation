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
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageLogic.h>
#include <vtkNew.h>
#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkMergePoints.h>

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
::ComputeDiceCoefficient(std::vector<vtkMRMLScalarVolumeNode*> labelMaps,
                         std::vector<std::vector<double> >& resultsArray)
{

  // Clean previous results and resize array
  int numberOfSamples = labelMaps.size();
  resultsArray.clear();
  resultsArray.resize(numberOfSamples);
  for (int s = 0; s < numberOfSamples; s++)
    {
    resultsArray[s].clear();
    resultsArray[s].resize(numberOfSamples);
    }

  for (int i = 0; i < numberOfSamples; i++)
    {
    // Matrix is symmetric. Only do a half (j <= i)
    for (int j = 0; (j <= i) && (j < numberOfSamples); j++)
      {
      // Put -1 if one of the map is not selected
      vtkMRMLScalarVolumeNode* labelMap1 = labelMaps[i];
      vtkMRMLScalarVolumeNode* labelMap2 = labelMaps[j];
      if (labelMap1 != NULL && labelMap2 != NULL)
        {
        // Dice coeff of a map with itself is 1.0
        if (i == j)
          {
          resultsArray[i][j] = 1.0;
          }
        else
          {
          // Compute dice coefficient
          int pixelNumber1 = this->GetNumberOfPixels(labelMap1);
          int pixelNumber2 = this->GetNumberOfPixels(labelMap2);

          if ((pixelNumber1 > 0) && (pixelNumber2 > 0))
            {
            int numberOfPixelIntersection = this->ComputeIntersection(labelMap1, labelMap2);

            if (numberOfPixelIntersection >= 0)
              {
              // Symmetric matrix
              // Keep the 2.0 (instead of 2) otherwise results is converted in integer (or cast)
              double diceCoeff = 2.0*numberOfPixelIntersection / (pixelNumber1 + pixelNumber2);
              resultsArray[i][j] = resultsArray[j][i] = diceCoeff;
              }
            else
              {
              resultsArray[i][j] = resultsArray[j][i] = -1.0;
              }
            }
          else
            {
            resultsArray[i][j] = resultsArray[j][i] = -1.0;
            }
          }
        }
      else
        {
        resultsArray[i][j] = resultsArray[j][i] = -1.0;
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDiceComputationLogic
::ComputeHausdorffDistance(std::vector<vtkPolyData*> polyData,
			   std::vector<std::vector<double> >& resultsArray)
{
  // Clean previous results and resize array
  int numberOfSamples = polyData.size();
  resultsArray.clear();
  resultsArray.resize(numberOfSamples);
  for (int s = 0; s < numberOfSamples; s++)
    {
    resultsArray[s].clear();
    resultsArray[s].resize(numberOfSamples);
    }
  
  for (int i = 0; i < numberOfSamples; ++i)
    {
    // Matrix is symmetric. Only do a half (j <= i)
    for (int j = 0; (j <= i) && (j < numberOfSamples); ++j)
      {
      // Put -1 if one of the map is not selected
      vtkPolyData* poly1 = polyData[i];
      vtkPolyData* poly2 = polyData[j];
      if (poly1 != NULL && poly2 != NULL)
        {
        // Hausdorff distance of a polydata with itself is 0.0
        if (i == j)
          {
          resultsArray[i][j] = 0.0;
          }	
	else
	  {
	  double maximumDistance = 0.0;

	  // Compute haudorff distance
	  vtkSmartPointer<vtkMergePoints> loc1 = vtkSmartPointer<vtkMergePoints>::New();
	  loc1->SetDataSet(poly1);
	  loc1->AutomaticOn();
	  loc1->BuildLocator();

	  vtkSmartPointer<vtkMergePoints> loc2 = vtkSmartPointer<vtkMergePoints>::New();
	  loc2->SetDataSet(poly2);
	  loc2->AutomaticOn();
	  loc2->BuildLocator();
	  
	  vtkSmartPointer<vtkPoints> points1 = poly1->GetPoints();
	  vtkSmartPointer<vtkPoints> points2 = poly2->GetPoints();
	  
	  int nOfPoints1 = points1->GetNumberOfPoints();
	  int nOfPoints2 = points2->GetNumberOfPoints();
	  
 	  for (int i = 0; i < nOfPoints1; i++)
	    {
	    double* point1 = points1->GetPoint(i);
	    vtkIdType closestPoint1 = loc2->FindClosestPoint(point1);
	    double *point2 = points2->GetPoint(closestPoint1);
	    double distance = std::sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
	    if (distance > maximumDistance)
	      {
	      maximumDistance = distance;
	      }
	    }

 	  for (int i = 0; i < nOfPoints2; i++)
	    {
	    double* point3 = points2->GetPoint(i);
	    vtkIdType closestPoint2 = loc1->FindClosestPoint(point3);
	    double *point4 = points1->GetPoint(closestPoint2);
	    double distance2 = std::sqrt(vtkMath::Distance2BetweenPoints(point3, point4));
	    if (distance2 > maximumDistance)
	      {
	      maximumDistance = distance2;
	      }
	    }

	  if (maximumDistance == 0)
	    {
	    resultsArray[i][j] = resultsArray[j][i] = -1.0;	    
	    }
	  else
	    {
	    resultsArray[i][j] = resultsArray[j][i] = maximumDistance;	    
	    }
	  }
	}
      }
    }
}

//---------------------------------------------------------------------------
int vtkSlicerDiceComputationLogic
::ComputeIntersection(vtkMRMLScalarVolumeNode* map1,
                      vtkMRMLScalarVolumeNode* map2)
{
  if (!map1 || !map2)
    {
    return -1;
    }

  vtkImageData* imData1 = map1->GetImageData();
  vtkImageData* imData2 = map2->GetImageData();

  if (!imData1 || !imData2)
    {
    return -1;
    }

  int numberOfCommonPixels = 0;

  vtkSmartPointer<vtkImageLogic> logicFilter 
    = vtkSmartPointer<vtkImageLogic>::New();
  logicFilter->SetInput1(imData1);
  logicFilter->SetInput2(imData2);
  logicFilter->SetOperationToAnd();
  logicFilter->Update();

  vtkSmartPointer<vtkImageData> tmpOutput 
    = vtkSmartPointer<vtkImageData>::New();
  tmpOutput = logicFilter->GetOutput();

  numberOfCommonPixels = this->GetNumberOfPixels(tmpOutput.GetPointer());

  return numberOfCommonPixels;
}

//---------------------------------------------------------------------------
int vtkSlicerDiceComputationLogic
::GetNumberOfPixels(vtkMRMLScalarVolumeNode* map)
{
  if (!map || map->GetLabelMap() == 0)
    {
    return -1;
    }

  vtkImageData* imData = map->GetImageData();
  if (!imData)
    {
    return -1;
    }

  return this->GetNumberOfPixels(imData);
}

//---------------------------------------------------------------------------
int vtkSlicerDiceComputationLogic
::GetNumberOfPixels(vtkImageData* imData)
{
  int numberOfPixels = 0;

  // Efficient way to computer number of pixels != 0
  vtkSmartPointer<vtkImageAccumulate> pixelCounter
    = vtkSmartPointer<vtkImageAccumulate>::New();
  pixelCounter->SetInput(imData);
  pixelCounter->IgnoreZeroOn();
  pixelCounter->Update();

  numberOfPixels = pixelCounter->GetVoxelCount();

  return numberOfPixels;
}
