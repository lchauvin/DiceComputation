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

// LabelMapSelector Widgets includes
#include "qSlicerDiceComputationLabelMapSelectorWidget.h"
#include "ui_qSlicerDiceComputationLabelMapSelectorWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_DiceComputation
class qSlicerDiceComputationLabelMapSelectorWidgetPrivate
  : public Ui_qSlicerDiceComputationLabelMapSelectorWidget
{
  Q_DECLARE_PUBLIC(qSlicerDiceComputationLabelMapSelectorWidget);
protected:
  qSlicerDiceComputationLabelMapSelectorWidget* const q_ptr;

public:
  qSlicerDiceComputationLabelMapSelectorWidgetPrivate(
    qSlicerDiceComputationLabelMapSelectorWidget& object);
  virtual void setupUi(qSlicerDiceComputationLabelMapSelectorWidget*);
};

// --------------------------------------------------------------------------
qSlicerDiceComputationLabelMapSelectorWidgetPrivate
::qSlicerDiceComputationLabelMapSelectorWidgetPrivate(
  qSlicerDiceComputationLabelMapSelectorWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerDiceComputationLabelMapSelectorWidgetPrivate
::setupUi(qSlicerDiceComputationLabelMapSelectorWidget* widget)
{
  this->Ui_qSlicerDiceComputationLabelMapSelectorWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerDiceComputationLabelMapSelectorWidget methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationLabelMapSelectorWidget
::qSlicerDiceComputationLabelMapSelectorWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerDiceComputationLabelMapSelectorWidgetPrivate(*this) )
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerDiceComputationLabelMapSelectorWidget
::~qSlicerDiceComputationLabelMapSelectorWidget()
{
}
