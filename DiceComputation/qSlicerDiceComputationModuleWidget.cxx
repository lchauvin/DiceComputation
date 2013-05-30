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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerDiceComputationModuleWidget.h"
#include "ui_qSlicerDiceComputationModuleWidget.h"

#include "vtkSlicerDiceComputationLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDiceComputationModuleWidgetPrivate: public Ui_qSlicerDiceComputationModuleWidget
{
public:
  qSlicerDiceComputationModuleWidgetPrivate();
  
  std::vector<std::vector<double> > resultsArray;
  std::vector<vtkMRMLScalarVolumeNode*> labelMaps;
  int numberOfSelectedNodes;
};

//-----------------------------------------------------------------------------
// qSlicerDiceComputationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidgetPrivate::qSlicerDiceComputationModuleWidgetPrivate()
{
  this->numberOfSelectedNodes = 0;
}

//-----------------------------------------------------------------------------
// qSlicerDiceComputationModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidget::qSlicerDiceComputationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerDiceComputationModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidget::~qSlicerDiceComputationModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::setup()
{
  Q_D(qSlicerDiceComputationModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  d->ProgressBar->hide();

  connect(d->LabelMapNumberWidget, SIGNAL(valueChanged(double)),
	  this, SLOT(onLabelMapNumberChanged(double)));

  connect(d->ComputeDiceCoeff, SIGNAL(clicked()),
          this, SLOT(onComputeDiceCoeffClicked()));

  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
          this, SLOT(onMRMLSceneChanged(vtkMRMLScene*)));
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::onMRMLSceneChanged(vtkMRMLScene* newScene)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  // 2 widgets to start
  d->LabelMapNumberWidget->setValue(2);
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::onLabelMapNumberChanged(double newMapNumber)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  int currentMapNumber = d->LabelMapLayout->count();

  if (currentMapNumber > newMapNumber)
    {
    // Remove (currentMapNumber - newMapNumber) widget(s)
    for (int i = newMapNumber; i < currentMapNumber; i++)
      {
      // Safe way to remove item from layout (from Qt doc)
      QLayoutItem* child;
      if ((child = d->LabelMapLayout->takeAt(i)) != 0)
        {
        d->LabelMapLayout->removeWidget(child->widget());
        child->widget()->deleteLater();
        delete child;
        }
      }
    }
  else
    {
    // Add (newMapNumber - currentMapNumber) widget(s)
    for (int i = currentMapNumber; i < newMapNumber; i++)
      {
      // Add new widgets to the layout
      qSlicerDiceComputationLabelMapSelectorWidget* newItem =
        new qSlicerDiceComputationLabelMapSelectorWidget(d->LabelMapsFrame);
      if (newItem)
        {
        newItem->setMRMLScene(this->mrmlScene());
        d->LabelMapLayout->addWidget(newItem);
        newItem->setPosition(d->LabelMapLayout->count());
        }
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::onComputeDiceCoeffClicked()
{
  Q_D(qSlicerDiceComputationModuleWidget);

  // Check selected nodes
  d->labelMaps.clear();
  d->numberOfSelectedNodes = 0;

  for (int i = 0; i < d->LabelMapLayout->count(); i++)
    {
    QLayoutItem* child;
    if ((child = d->LabelMapLayout->itemAt(i)) != 0)
      {        
      qSlicerDiceComputationLabelMapSelectorWidget* tmpWidget
        = dynamic_cast<qSlicerDiceComputationLabelMapSelectorWidget*>(child->widget());
      if (tmpWidget)
        {
        d->labelMaps.push_back(tmpWidget->getSelectedNode());
        if (tmpWidget->getSelectedNode())
          {
          d->numberOfSelectedNodes++;
          }
        }
      }
    }

  // Compute dice coefficients
  vtkSlicerDiceComputationLogic* dcLogic =
    vtkSlicerDiceComputationLogic::SafeDownCast(this->logic());
  if (dcLogic)
    {
    dcLogic->ComputeDiceCoefficient(d->numberOfSelectedNodes, d->labelMaps, d->resultsArray);
    }
  
  // Display results
  std::cerr << "Dice Coeff: " << d->resultsArray[0][1] << std::endl;
  
  // TODO
}

