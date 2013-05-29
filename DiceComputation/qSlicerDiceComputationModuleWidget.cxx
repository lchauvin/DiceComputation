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

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDiceComputationModuleWidgetPrivate: public Ui_qSlicerDiceComputationModuleWidget
{
public:
  qSlicerDiceComputationModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerDiceComputationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidgetPrivate::qSlicerDiceComputationModuleWidgetPrivate()
{
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

  connect(d->LabelMapNumberWidget, SIGNAL(valueChanged(double)),
	  this, SLOT(onLabelMapNumberChanged(double)));
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
        delete child->widget();
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
        new qSlicerDiceComputationLabelMapSelectorWidget();
      if (newItem)
        {
        d->LabelMapLayout->addWidget(newItem);
        }
      }
    }
}

