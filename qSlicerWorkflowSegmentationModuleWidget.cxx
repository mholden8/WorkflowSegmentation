

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

==============================================================================*/
#include <iostream>
// Qt includes

// SlicerQt includes
#include "qSlicerWorkflowSegmentationModuleWidget.h"
#include "ui_qSlicerWorkflowSegmentationModule.h"
#include "qSlicerIO.h"
#include "qSlicerIOManager.h"
#include "qSlicerApplication.h"
#include <QtGui>

// MRMLWidgets includes
#include <qMRMLUtils.h>


#include "vtkMRMLWorkflowSegmentationNode.h"
#include "vtkMRMLLinearTransformNode.h"

#include "qMRMLNodeComboBox.h"
#include "vtkMRMLViewNode.h"
#include "vtkSlicerWorkflowSegmentationLogic.h"
#include "vtkMRMLWorkflowSegmentationNode.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_WorkflowSegmentation
class qSlicerWorkflowSegmentationModuleWidgetPrivate: public Ui_qSlicerWorkflowSegmentationModule
{
  Q_DECLARE_PUBLIC( qSlicerWorkflowSegmentationModuleWidget ); 

protected:
  qSlicerWorkflowSegmentationModuleWidget* const q_ptr;
public:
  qSlicerWorkflowSegmentationModuleWidgetPrivate( qSlicerWorkflowSegmentationModuleWidget& object );
  ~qSlicerWorkflowSegmentationModuleWidgetPrivate();

  vtkSlicerWorkflowSegmentationLogic* logic() const;
  vtkMRMLLinearTransformNode*   MRMLTransformNode;
};

//-----------------------------------------------------------------------------
// qSlicerWorkflowSegmentationModuleWidgetPrivate methods



qSlicerWorkflowSegmentationModuleWidgetPrivate::qSlicerWorkflowSegmentationModuleWidgetPrivate( qSlicerWorkflowSegmentationModuleWidget& object ) : q_ptr(&object)
{
  this->MRMLTransformNode = 0;
}

//-----------------------------------------------------------------------------

qSlicerWorkflowSegmentationModuleWidgetPrivate::~qSlicerWorkflowSegmentationModuleWidgetPrivate()
{
}


vtkSlicerWorkflowSegmentationLogic* qSlicerWorkflowSegmentationModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerWorkflowSegmentationModuleWidget );
  return vtkSlicerWorkflowSegmentationLogic::SafeDownCast( q->logic() );
}


//-----------------------------------------------------------------------------
// qSlicerWorkflowSegmentationModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerWorkflowSegmentationModuleWidget::qSlicerWorkflowSegmentationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerWorkflowSegmentationModuleWidgetPrivate( *this ) )
{
}



qSlicerWorkflowSegmentationModuleWidget::~qSlicerWorkflowSegmentationModuleWidget()
{
}



void qSlicerWorkflowSegmentationModuleWidget::setup()
{
  Q_D(qSlicerWorkflowSegmentationModuleWidget);

  d->StatusResultLabel= NULL;
  d->NumRecordsResultLabel=NULL;
  d->TotalTimeResultLabel=NULL;
  d->CurrentTaskResultLabel=NULL;

  d->setupUi(this);
  this->Superclass::setup();

  d->ModuleComboBox->setNoneEnabled( true );

  connect( d->ModuleComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModuleNodeSelected() ) );
  connect( d->LoadLogButton, SIGNAL(clicked()),this, SLOT (loadLogFile() ) );
  connect( d->TrainingDataButton, SIGNAL(clicked()), this, SLOT( onTrainingDataButtonClicked() ) );
  connect( d->InputParameterButton, SIGNAL(clicked()), this, SLOT( onInputParameterButtonClicked() ) );
  connect( d->TrainButton, SIGNAL(clicked()), this, SLOT( onTrainButtonClicked() ) );
  connect( d->StartButton, SIGNAL(clicked()),this,SLOT(onStartButtonClicked() ) );
  connect( d->StopButton, SIGNAL(clicked()),this,SLOT(onStopButtonClicked() ) );
  connect( d->ClearBufferButton, SIGNAL(clicked()),this,SLOT(onClearBufferButtonClicked() ) );
  connect( d->TransformCheckableComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ),
           this,                          SLOT( onTransformsNodeSelected(vtkMRMLNode*) ) );
  
  
  // GUI refresh: updates every 10ms
  QTimer *t = new QTimer( this );
  connect( t,  SIGNAL( timeout() ), this, SLOT( updateGUI() ) );
  t->start(10); 
  
  // onTransformsNodeSelected( 0 );

  //Annotations


  d->AnnotationListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(d->InsertAnnotationButton, SIGNAL(clicked()), this, SLOT(insertAnnotation()));
  connect(d->ClearAnnotationButton, SIGNAL(clicked()), this, SLOT(clearAnnotations()));

}



void qSlicerWorkflowSegmentationModuleWidget
::onTransformsNodeSelected( vtkMRMLNode* node )
{
  Q_D(qSlicerWorkflowSegmentationModuleWidget);
  /*
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  
  if ( transformNode != NULL
       && d->logic()->GetModuleNode() != NULL )
  {
    d->logic()->GetModuleNode()->AddObservedTransformNode( node->GetID() );
  }
  */
  
    // TODO: Why was this here?
    // Listen for Transform node changes
  /*
  this->qvtkReconnect( d->MRMLTransformNode, transformNode, vtkMRMLTransformableNode::TransformModifiedEvent,
                       this, SLOT( onMRMLTransformNodeModified( vtkObject* ) ) ); 
  */
}



void qSlicerWorkflowSegmentationModuleWidget::onMRMLTransformNodeModified( vtkObject* caller )
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  // TODO: I'm not sure this function is needed at all.
  /*
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( caller );
  if (!transformNode) { return; }
  
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  */
}



void qSlicerWorkflowSegmentationModuleWidget::loadLogFile()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  QString filename = QFileDialog::getSaveFileName( this, tr("Save record"), "", tr("XML Files (*.xml)") );
  
  if ( filename.isEmpty() == false )
  {
    d->logic()->GetModuleNode()->SaveIntoFile( filename.toStdString() );
    d->AnnotationListWidget->clear();
  }
  
  this->updateGUI();
}



void qSlicerWorkflowSegmentationModuleWidget::enter()
{
  this->Superclass::enter();
  this->updateGUI();
}



void qSlicerWorkflowSegmentationModuleWidget::onModuleNodeSelected()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
 
  vtkMRMLNode* currentNode = d->ModuleComboBox->currentNode();
  std::cout << "Current node pointer: " << currentNode << std::endl;
  
  vtkMRMLWorkflowSegmentationNode* TRNode = vtkMRMLWorkflowSegmentationNode::SafeDownCast( currentNode );
  if ( TRNode != NULL )
  {
    
  }
  
  d->logic()->SetModuleNode( TRNode );
  this->updateGUI();
}


void qSlicerWorkflowSegmentationModuleWidget
::onTrainingDataButtonClicked()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  QString dirName = QFileDialog::getExistingDirectory( this, tr("Open training directory"), "", QFileDialog::ShowDirsOnly );
  
  if ( dirName.isEmpty() == false )
  {
    
    QProgressDialog dialog;
    dialog.setModal( true );
    dialog.setLabelText( "Please wait while reading XML files..." );
    dialog.show();
    dialog.setValue( 10 );
    
	d->logic()->GetModuleNode()->ImportTrainingData( dirName.toStdString() );

    dialog.close();
    
  }
  
  this->updateGUI();
}


void qSlicerWorkflowSegmentationModuleWidget
::onInputParameterButtonClicked()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  QString fileName = QFileDialog::getOpenFileName( this, tr("Open parameters"), "", tr("XML Files (*.xml)") );
  
  if ( fileName.isEmpty() == false )
  {
	d->logic()->GetModuleNode()->ImportInputParameters( fileName.toStdString() );
  }
  
  this->updateGUI();
}

void qSlicerWorkflowSegmentationModuleWidget
::onTrainButtonClicked()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  d->logic()->GetModuleNode()->TrainSegmentationAlgorithm();
  
  this->updateGUI();
}



void qSlicerWorkflowSegmentationModuleWidget
::onStartButtonClicked()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  
  if ( d->logic()->GetModuleNode() == NULL )
  {
    return;
  }
  
  
    // Go through transform types (ie ProbeToReference, StylusTipToReference, etc) 
    // Save selected state as 1 and unselected state as 0 of each transform type in transformSelections vector.
  
  std::vector< int > transformSelections;
  const int unselected = 0;
  const int selected = 1;
  
  for ( int i = 0; i < d->TransformCheckableComboBox->nodeCount(); i++ )
  {
	  if( d->TransformCheckableComboBox->checkState( d->TransformCheckableComboBox->nodeFromIndex( i ) ) == Qt::Checked  )
	  {
	    d->logic()->GetModuleNode()->AddObservedTransformNode( d->TransformCheckableComboBox->nodeFromIndex( i )->GetID() );
	  }
	  else
	  {
	    d->logic()->GetModuleNode()->RemoveObservedTransformNode( d->TransformCheckableComboBox->nodeFromIndex( i )->GetID() );
	  }
	}
  
  
  d->logic()->GetModuleNode()->SetRecording( true );
  
  this->updateGUI();
  
}



void qSlicerWorkflowSegmentationModuleWidget
::onStopButtonClicked()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );

  if( d->logic()->GetModuleNode() == NULL )
  {
    return;
  }

  d->logic()->GetModuleNode()->SetRecording( false );
   
  this->updateGUI();
}



void qSlicerWorkflowSegmentationModuleWidget
::onClearBufferButtonClicked() 
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );

  if( d->logic()->GetModuleNode() == NULL )
  {
    return;
  }

  d->logic()->GetModuleNode()->ClearBuffer();
   
  this->updateGUI();
}



void qSlicerWorkflowSegmentationModuleWidget
::insertAnnotation()
{
	Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  vtkMRMLWorkflowSegmentationNode* moduleNode = d->logic()->GetModuleNode();
  if ( moduleNode == NULL )
  {
    return;
  }
  
  
  int sec = 0;
  int nsec = 0;
  
  moduleNode->GetTimestamp( sec, nsec );
  
  
    // Get the timestamp for this annotation.
  
  
  QString itemText = QInputDialog::getText(this, tr("Insert Annotation"),
      tr("Input text for the new annotation:"));

  if (itemText.isNull())
      return;
  
  
  QListWidgetItem *newItem = new QListWidgetItem;
  newItem->setText(itemText);
  
  QString toolTipText = tr("Tooltip:") + itemText;
  QString statusTipText = tr("Status tip:") + itemText;
  QString whatsThisText = tr("What's This?:") + itemText;

  newItem->setToolTip(toolTipText);
  newItem->setStatusTip(toolTipText);
  newItem->setWhatsThis(whatsThisText);

	d->AnnotationListWidget->addItem(newItem);
	std::string annotation = itemText.toStdString();
	moduleNode->CustomMessage( annotation, sec, nsec );

}



void qSlicerWorkflowSegmentationModuleWidget
::clearAnnotations()
{
	Q_D( qSlicerWorkflowSegmentationModuleWidget );

	d->AnnotationListWidget->clear();
}



void qSlicerWorkflowSegmentationModuleWidget::updateGUI()
{
  Q_D( qSlicerWorkflowSegmentationModuleWidget );
  
  
      // Disableing node selector widgets if there is no module node to reference input nodes.
    
  if ( d->logic()->GetModuleNode() == NULL )
  {
    d->TransformCheckableComboBox->setEnabled( false );
  }
  else
  {
    d->TransformCheckableComboBox->setEnabled( true );
  }
  
    // The following code requires a module node.
  
  if ( d->logic()->GetModuleNode() == NULL )
  {
    return;
  }
  
  
  
    // Update status descriptor labels.
  
  if ( d->logic()->GetModuleNode()->GetRecording() )
  {
    d->StatusResultLabel->setText( "Recording" );
    d->TransformCheckableComboBox->setEnabled( false );
  }
  else
  {
    d->StatusResultLabel->setText( "Waiting" );
    d->TransformCheckableComboBox->setEnabled( true );
  }
  
  
  int numRec = d->logic()->GetModuleNode()->GetTransformsBufferSize() + d->logic()->GetModuleNode()->GetMessagesBufferSize();
  std::stringstream ss;
  ss << numRec;
  d->NumRecordsResultLabel->setText( QString::fromStdString( ss.str() ) );
  
  ss.str( "" );
  ss.precision( 2 );
  ss << std::fixed << d->logic()->GetModuleNode()->GetTotalTime();
  d->TotalTimeResultLabel->setText( ss.str().c_str() );

  ss.str( "" );
  ss << d->logic()->GetModuleNode()->GetCurrentTask();
  d->CurrentTaskResultLabel->setText( ss.str().c_str() );

}
