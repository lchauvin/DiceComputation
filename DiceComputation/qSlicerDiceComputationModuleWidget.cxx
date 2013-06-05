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
#include <QTimer>

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
  ~qSlicerDiceComputationModuleWidgetPrivate();

  std::vector<std::vector<double> > resultsArray;
  std::vector<vtkMRMLScalarVolumeNode*> labelMaps;
  int labelMapSize;
  bool averageComputed;
};

//-----------------------------------------------------------------------------
// qSlicerDiceComputationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidgetPrivate::qSlicerDiceComputationModuleWidgetPrivate()
{
  this->labelMapSize = 0;
  this->averageComputed = false;
}

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidgetPrivate::~qSlicerDiceComputationModuleWidgetPrivate()
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

  connect(d->ComputeDiceCoeff, SIGNAL(clicked()),
          this, SLOT(onComputeDiceCoeffClicked()));

  connect(d->AverageButton, SIGNAL(clicked()),
	  this, SLOT(onAverageClicked()));

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

  // Create list of scalar volume nodes
  d->labelMaps.clear();

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
        }
      }
    }

  // Check at least 2 label maps have been selected
  int numberOfLabelMaps = 0;
  d->labelMapSize = d->labelMaps.size();

  for (int i = 0; i < d->labelMapSize; i++)
    {
    if (d->labelMaps[i] != NULL)
      {
      numberOfLabelMaps++;
      }
    
    if (numberOfLabelMaps >= 2)
      {
      // At least 2 label maps found. No need to continue.
      break;
      }
    }

  if (numberOfLabelMaps < 2)
    {
    return;
    }
  

  // Compute dice coefficients
  vtkSlicerDiceComputationLogic* dcLogic =
    vtkSlicerDiceComputationLogic::SafeDownCast(this->logic());
  if (dcLogic)
    {
    dcLogic->ComputeDiceCoefficient(d->labelMaps, d->resultsArray);
    }

  // Display results
  if (d->OutputFrame->collapsed())
    {
    d->OutputFrame->setCollapsed(false);
    }

  d->OutputResultsTable->clear();
  d->OutputResultsTable->clearContents();
  d->OutputResultsTable->setRowCount(d->labelMapSize);
  d->OutputResultsTable->setColumnCount(d->labelMapSize);

  for (int i = 0; i < d->labelMapSize; i++)
    {
    for (int j = 0; j < d->labelMapSize; j++)
      {
      QTableWidgetItem* item = new QTableWidgetItem();
      if (item)
        {
        double diceCoeff = d->resultsArray[i][j];
        QBrush* brush = new QBrush();
        if (diceCoeff == -1)
          {
          // Wrong. Red color.
          brush->setColor(QColor::fromRgb(255,0,0,128));
          brush->setStyle(Qt::FDiagPattern);
          }
        else if (i == j)
          {
          brush->setColor(QColor::fromRgb(0,255,0,220));
          brush->setStyle(Qt::FDiagPattern);
          }
        else
          {
          // Good. Green color. Opacity depending on Dice's coefficient
          brush->setColor(QColor::fromRgb(0,255,0,diceCoeff*255));
          brush->setStyle(Qt::SolidPattern);
          }
        item->setBackground(*brush);

        if ((diceCoeff >= 0) && (i != j))
          {
          item->setText(QString::number(diceCoeff,'g',3));
          }
        d->OutputResultsTable->setItem(i,j,item);
        }
      }
    }

  // Turn on/off average button if data are present in the table
  d->AverageButton->setEnabled(d->OutputResultsTable->rowCount() != 0);

  // Average not computed for these values
  d->averageComputed = false;
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::onAverageClicked()
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->OutputResultsTable)
    {
    return;
    }

  // Average already computed for these values
  if (d->averageComputed)
    {
    return;
    }

  // Add new row in the table
  d->OutputResultsTable->insertRow(d->OutputResultsTable->rowCount());
  QTableWidgetItem* averageHeader = new QTableWidgetItem();
  if (averageHeader)
    {
    averageHeader->setText("Average");
    d->OutputResultsTable->setVerticalHeaderItem(d->OutputResultsTable->rowCount()-1, averageHeader);
    }
  
  // Compute averages
  for (int i = 0; i < d->labelMapSize; i++)
    {
    int numberOfValues = 0;
    double average = 0;
    for (int j = 0; j < d->labelMapSize; j++)
      {
      if (i != j)
	{
	QTableWidgetItem* item = d->OutputResultsTable->item(j,i);
	if (item && (item->background().style() != Qt::FDiagPattern))
	  {
	  double itemValue = item->text().toDouble();
	  average += itemValue;
	  ++numberOfValues;
	  }
	}
      }
    if (numberOfValues > 0)
      {
      average /= numberOfValues;
      }
    else
      {
      average = -1;
      }
    
    // Create new item
    QTableWidgetItem* newAverageItem = new QTableWidgetItem();
    QBrush* newBrush = new QBrush();
    if (newAverageItem && newBrush)
      {
      if (average > 0)
	{
	newBrush->setColor((QColor::fromRgb(126,30,156,average*255)));
	newBrush->setStyle(Qt::SolidPattern);
	newAverageItem->setText(QString::number(average,'f',3));
	}
      else
	{
	newBrush->setColor(QColor::fromRgb(255,0,0,128));
	newBrush->setStyle(Qt::FDiagPattern);
	}
      newAverageItem->setBackground(*newBrush);
      d->OutputResultsTable->setItem(d->OutputResultsTable->rowCount()-1,i, newAverageItem);
      }
    }

  d->averageComputed = true;
}
