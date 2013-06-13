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

#include <cmath>

// Qt includes
#include <QDebug>
#include <QTimer>

// SlicerQt includes
#include "qSlicerDiceComputationModuleWidget.h"
#include "ui_qSlicerDiceComputationModuleWidget.h"

#include "vtkSlicerDiceComputationLogic.h"

#include <vtkImageLabelChange.h>
#include <vtkImageData.h>
#include <vtkPointData.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDiceComputationModuleWidgetPrivate: public Ui_qSlicerDiceComputationModuleWidget
{
public:
  qSlicerDiceComputationModuleWidgetPrivate();
  ~qSlicerDiceComputationModuleWidgetPrivate();

  std::vector<std::vector<double> > resultsArray;
  std::vector<vtkMRMLScalarVolumeNode*> labelMaps;
  std::vector<vtkImageData*> STAPLEImages;
  int labelMapSize;
};

//-----------------------------------------------------------------------------
// qSlicerDiceComputationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDiceComputationModuleWidgetPrivate::qSlicerDiceComputationModuleWidgetPrivate()
{
  this->labelMapSize = 0;
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

  connect(d->ComputeButton, SIGNAL(clicked()),
          this, SLOT(onComputeButtonClicked()));

  connect(d->ComputeStatsButton, SIGNAL(clicked()),
	  this, SLOT(onComputeStatsClicked()));

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
void qSlicerDiceComputationModuleWidget::onComputeButtonClicked()
{
  Q_D(qSlicerDiceComputationModuleWidget);

  // Check which statistics to compute
  if (d->DiceRadioButton->isChecked())
    {
    this->computeDiceCoefficient();
    }
  else if(d->SensitivityRadioButton->isChecked())
    {
    this->computeSensitivity();
    }
  else if(d->SpecificityRadioButton->isChecked())
    {
    this->computeSpecificity();
    }
}


//-----------------------------------------------------------------------------
bool qSlicerDiceComputationModuleWidget::findLabelMaps()
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
    return false;
    }
  
  return true;
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeDiceCoefficient()
{
  Q_D(qSlicerDiceComputationModuleWidget);
  
  if (!this->findLabelMaps())
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
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeSensitivity()
{
  Q_D(qSlicerDiceComputationModuleWidget);
  
  if (!this->findLabelMaps())
    {
    return;
    }

  d->STAPLEImages.clear();
  FilterType::Pointer filter = FilterType::New();
  filter->SetForegroundValue(1);

  for (int i = 0; i < d->labelMapSize; i++)
    {
    if (d->labelMaps[i] != NULL)
      {
      double* range = d->labelMaps[i]->GetImageData()->GetPointData()->GetScalars()->GetRange(0);

      if (range[1] != 1)
	{
	// Change label values to 1
	vtkSmartPointer<vtkImageLabelChange> labelChanger
	  = vtkSmartPointer<vtkImageLabelChange>::New();
	vtkSmartPointer<vtkImageData> tmpData
	  = vtkSmartPointer<vtkImageData>::New();
	labelChanger->SetInput(d->labelMaps[i]->GetImageData());
	labelChanger->SetOutput(tmpData.GetPointer());
	labelChanger->SetInputLabel(range[1]);
	labelChanger->SetOutputLabel(1);
	labelChanger->Update();
	d->STAPLEImages.push_back(tmpData.GetPointer());
	}
      
      // TODO: Set filter inputs here
      // Need to convert from VTK to ITK first
      }
    }
  filter->UpdateLargestPossibleRegion();
  
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeSpecificity()
{
  Q_D(qSlicerDiceComputationModuleWidget);
  
  if (!this->findLabelMaps())
    {
    return;
    }
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::onComputeStatsClicked()
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->StatsTable)
    {
    return;
    }

  // Clear table
  d->StatsTable->clear();
  d->StatsTable->clearContents();
  d->StatsTable->setRowCount(0);
  d->StatsTable->setColumnCount(d->labelMapSize);

  // Get checkbox states
  bool averageChecked = d->AverageCheckbox->isChecked();
  bool stdDevChecked  = d->StdDevCheckbox->isChecked();
  bool minChecked     = d->MinCheckbox->isChecked();
  bool maxChecked     = d->MaxCheckbox->isChecked();

  // -- Compute average if checked
  if (averageChecked || stdDevChecked)
    {
    // Add new row in the table
    d->StatsTable->insertRow(d->StatsTable->rowCount());
    QTableWidgetItem* averageHeader = new QTableWidgetItem();
    if (averageHeader)
      {
      averageHeader->setText("Average");
      d->StatsTable->setVerticalHeaderItem(0, averageHeader);
      }

    for (int i = 0; i < d->labelMapSize; i++)
      {
      this->computeAverage(i);
      }
    }

  // -- Compute Standard Deviation if checked
  if (stdDevChecked)
    {
    // Add new row in the table
    d->StatsTable->insertRow(d->StatsTable->rowCount());
    QTableWidgetItem* stdDevHeader = new QTableWidgetItem();
    if (stdDevHeader)
      {
      stdDevHeader->setText("StdDev");
      d->StatsTable->setVerticalHeaderItem(d->StatsTable->rowCount()-1, stdDevHeader);
      }
    
    for (int i = 0; i < d->labelMapSize; i++)
      {
      this->computeStdDev(i);
      }
    }

  // -- Compute Min if checked
  if (minChecked)
    {
    // Add new row in the table
    d->StatsTable->insertRow(d->StatsTable->rowCount());
    QTableWidgetItem* minHeader = new QTableWidgetItem();
    if (minHeader)
      {
      minHeader->setText("Min");
      d->StatsTable->setVerticalHeaderItem(d->StatsTable->rowCount()-1, minHeader);
      }

    for (int i = 0; i < d->labelMapSize; i++)
      {
      this->computeMin(i);
      }
    }

  // -- Compute Max if checked
  if (maxChecked)
    {
    // Add new row in the table
    d->StatsTable->insertRow(d->StatsTable->rowCount());
    QTableWidgetItem* maxHeader = new QTableWidgetItem();
    if (maxHeader)
      {
      maxHeader->setText("Max");
      d->StatsTable->setVerticalHeaderItem(d->StatsTable->rowCount()-1, maxHeader);
      }

    for (int i = 0; i < d->labelMapSize; i++)
      {
      this->computeMax(i);
      }
    }

}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeAverage(int column)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->StatsTable || !d->OutputResultsTable)
    {
    return;
    }

  int numberOfValues = 0;
  double average = 0.0;
  for (int row = 0; row < d->labelMapSize; row++)
    {
    if (column != row)
      {
      QTableWidgetItem* item = d->OutputResultsTable->item(row,column);
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
    d->StatsTable->setItem(d->StatsTable->rowCount()-1,column, newAverageItem);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeStdDev(int column)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->StatsTable || !d->OutputResultsTable)
    {
    return;
    }
    
  // Get average
  double average = -1.0;
  int numberOfValues = 0;
  double stdDeviation = -1.0;

  QTableWidgetItem* averageItem = d->StatsTable->item(0,column);
  if (averageItem)
    {
    if (averageItem->background().style() != Qt::FDiagPattern)
      {
      average = averageItem->text().toDouble();
      }
    }

  // Compute std dev
  if (average > 0)
    {
    for (int row = 0; row < d->labelMapSize; row++)
      {
      if (column != row)
        {
        QTableWidgetItem* item = d->OutputResultsTable->item(row,column);
        if (item && (item->background().style() != Qt::FDiagPattern))
          {
          if (stdDeviation < 0)
            {
            stdDeviation = 0.0;
            }
          double itemValue = item->text().toDouble();
          stdDeviation += std::pow((itemValue - average),2.0);
          ++numberOfValues;
          }
        }
      }
    }

  if (numberOfValues > 0)
    {
    stdDeviation /= numberOfValues;
    stdDeviation = std::sqrt(stdDeviation);
    }
  
  // Create new item
  QTableWidgetItem* newStdItem = new QTableWidgetItem();
  QBrush* newBrush = new QBrush();
  if (newStdItem && newBrush)
    {
    if (stdDeviation >= 0)
      {
      newBrush->setColor((QColor::fromRgb(30,144,255,stdDeviation*255)));
      newBrush->setStyle(Qt::SolidPattern);
      newStdItem->setText(QString::number(stdDeviation,'f',3));
      }
    else
      {
      newBrush->setColor(QColor::fromRgb(255,0,0,128));
      newBrush->setStyle(Qt::FDiagPattern);
      }
    newStdItem->setBackground(*newBrush);
    d->StatsTable->setItem(d->StatsTable->rowCount()-1,column, newStdItem);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeMin(int column)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->StatsTable || !d->OutputResultsTable)
    {
    return;
    }

  double min = 999.0;

  for (int row = 0; row < d->labelMapSize; row++)
    {
    if (column != row)
      {
      QTableWidgetItem* item = d->OutputResultsTable->item(row,column);
      if (item && (item->background().style() != Qt::FDiagPattern))
        {
        double itemValue = item->text().toDouble();
        min = (itemValue < min) ? itemValue : min; 
        }
      }
    }

 // Create new item
  QTableWidgetItem* newMinItem = new QTableWidgetItem();
  QBrush* newBrush = new QBrush();
  if (newMinItem && newBrush)
    {
    if (min <= 1.0 && min >= 0.0)
      {
      newBrush->setColor((QColor::fromRgb(0,255,0,min*255)));
      newBrush->setStyle(Qt::SolidPattern);
      newMinItem->setText(QString::number(min,'f',3));
      }
    else
      {
      newBrush->setColor(QColor::fromRgb(255,0,0,128));
      newBrush->setStyle(Qt::FDiagPattern);
      }
    newMinItem->setBackground(*newBrush);
    d->StatsTable->setItem(d->StatsTable->rowCount()-1,column, newMinItem);
    }
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationModuleWidget::computeMax(int column)
{
  Q_D(qSlicerDiceComputationModuleWidget);

  if (!d->StatsTable || !d->OutputResultsTable)
    {
    return;
    }

  double max = -999.0;

  for (int row = 0; row < d->labelMapSize; row++)
    {
    if (column != row)
      {
      QTableWidgetItem* item = d->OutputResultsTable->item(row,column);
      if (item && (item->background().style() != Qt::FDiagPattern))
        {
        double itemValue = item->text().toDouble();
        max = (itemValue > max) ? itemValue : max; 
        }
      }
    }

 // Create new item
  QTableWidgetItem* newMaxItem = new QTableWidgetItem();
  QBrush* newBrush = new QBrush();
  if (newMaxItem && newBrush)
    {
    if (max <= 1.0 && max >= 0.0)
      {
      newBrush->setColor((QColor::fromRgb(255,0,0,max*255)));
      newBrush->setStyle(Qt::SolidPattern);
      newMaxItem->setText(QString::number(max,'f',3));
      }
    else
      {
      newBrush->setColor(QColor::fromRgb(255,0,0,128));
      newBrush->setStyle(Qt::FDiagPattern);
      }
    newMaxItem->setBackground(*newBrush);
    d->StatsTable->setItem(d->StatsTable->rowCount()-1,column, newMaxItem);
    }
}
