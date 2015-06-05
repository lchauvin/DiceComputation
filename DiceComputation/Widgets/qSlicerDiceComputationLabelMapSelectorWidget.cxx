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

#include "vtkMRMLScene.h"

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

  bool isNodeSelected;
  vtkMRMLNode* nodeSelected;
};

// --------------------------------------------------------------------------
qSlicerDiceComputationLabelMapSelectorWidgetPrivate
::qSlicerDiceComputationLabelMapSelectorWidgetPrivate(
  qSlicerDiceComputationLabelMapSelectorWidget& object)
  : q_ptr(&object)
{
  this->isNodeSelected = false;
  this->nodeSelected = NULL;
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

  d->LabelMapSelector->setRemoveEnabled(false);
  d->LabelMapSelector->setEditEnabled(false);
  d->LabelMapSelector->setAddEnabled(false);
  d->LabelMapSelector->setShowHidden(false);
  d->LabelMapSelector->setNoneEnabled(true);
  d->LabelMapSelector->setShowChildNodeTypes(true);
  d->LabelMapSelector->setRenameEnabled(true);
  d->LabelMapSelector->setNodeTypes(QStringList() << "vtkMRMLLabelMapVolumeNode" << "vtkMRMLModelNode");

  connect(d->LabelMapSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(onNodeChanged(vtkMRMLNode*)));
}

//-----------------------------------------------------------------------------
qSlicerDiceComputationLabelMapSelectorWidget
::~qSlicerDiceComputationLabelMapSelectorWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationLabelMapSelectorWidget
::setMRMLScene(vtkMRMLScene* newScene)
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);

  if (!newScene)
    {
    return;
    }

  d->LabelMapSelector->setMRMLScene(newScene);
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationLabelMapSelectorWidget
::setPosition(int itemPosition)
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);

  std::stringstream labelName;
  labelName << "Item " << itemPosition;
  
  d->LabelMapLabel->setText(labelName.str().c_str());
}

//-----------------------------------------------------------------------------
vtkMRMLNode* qSlicerDiceComputationLabelMapSelectorWidget
::getSelectedNode()
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);

  if (!d->isNodeSelected || !d->nodeSelected)
    {
    return NULL;
    }

  return d->nodeSelected;  
}

//-----------------------------------------------------------------------------
void qSlicerDiceComputationLabelMapSelectorWidget
::setCurrentNode(vtkMRMLNode* node)
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);

  d->LabelMapSelector->setCurrentNode(node);
}


//-----------------------------------------------------------------------------
void qSlicerDiceComputationLabelMapSelectorWidget
::onNodeChanged(vtkMRMLNode* newNode)
{
  Q_D(qSlicerDiceComputationLabelMapSelectorWidget);

  // TODO: Set a flag to tell if label map or not
    //TODO: It is now not a flag, but the node type that tells the labelmap and scalar volumes apart: vtkMRMLLabelMapVolumeNode

/*
  if (!newNode || !newNode->IsA("vtkMRMLScalarVolumeNode"))
    {
    d->nodeSelected = NULL;
    d->isNodeSelected = false;
    return;
    }
    
  vtkMRMLScalarVolumeNode* tmpNode = vtkMRMLScalarVolumeNode::SafeDownCast(newNode);
  
  if (!tmpNode->GetLabelMap())
    {
    // Not a label map. Do not select it.
    d->LabelMapSelector->setCurrentNode(NULL);
    d->nodeSelected = NULL;
    d->isNodeSelected = false;
    return;
    }

  d->isNodeSelected = true;
*/
  d->nodeSelected = NULL;
  d->isNodeSelected = false;
  
  if (newNode)
    {
    d->nodeSelected = newNode;
    d->isNodeSelected = true;
    }
}
