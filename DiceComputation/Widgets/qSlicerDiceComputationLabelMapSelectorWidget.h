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

#ifndef __qSlicerDiceComputationLabelMapSelectorWidget_h
#define __qSlicerDiceComputationLabelMapSelectorWidget_h

// Qt includes
#include <QWidget>

// LabelMapSelector Widgets includes
#include "qSlicerDiceComputationModuleWidgetsExport.h"

class qSlicerDiceComputationLabelMapSelectorWidgetPrivate;

/// \ingroup Slicer_QtModules_DiceComputation
class Q_SLICER_MODULE_DICECOMPUTATION_WIDGETS_EXPORT qSlicerDiceComputationLabelMapSelectorWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerDiceComputationLabelMapSelectorWidget(QWidget *parent=0);
  virtual ~qSlicerDiceComputationLabelMapSelectorWidget();

protected slots:

protected:
  QScopedPointer<qSlicerDiceComputationLabelMapSelectorWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerDiceComputationLabelMapSelectorWidget);
  Q_DISABLE_COPY(qSlicerDiceComputationLabelMapSelectorWidget);
};

#endif
