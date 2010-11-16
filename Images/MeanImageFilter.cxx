#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkMeanImageFilter.h"

#include <itkImageToVTKImageFilter.h>

#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

int main(int argc, char * argv[])
{
  // Verify command line arguments
  if( argc < 2 )
    {
	  std::cerr << "Usage: " << std::endl;
	  std::cerr << argv[0] << " inputImageFile" << std::endl;
	  return EXIT_FAILURE;
    }

  // Parse command line arguments
  std::string inputFilename = argv[1];

  // Setup types
  typedef itk::Image< unsigned char, 2 >                        UnsignedCharImageType;
  typedef itk::ImageFileReader< UnsignedCharImageType >         readerType;
  typedef itk::MeanImageFilter<
		  UnsignedCharImageType, UnsignedCharImageType > filterType;

  // Create and setup a reader
  readerType::Pointer reader = readerType::New();
  reader->SetFileName( inputFilename.c_str() );

  // Create and setup a mean filter
  filterType::Pointer meanFilter = filterType::New();
  meanFilter->SetInput( reader->GetOutput() );

  // Visualize the original image
  typedef itk::ImageToVTKImageFilter<UnsignedCharImageType> ConnectorType;
  ConnectorType::Pointer originalConnector = ConnectorType::New();
  originalConnector->SetInput(reader->GetOutput());

  vtkSmartPointer<vtkImageActor> originalActor =
    vtkSmartPointer<vtkImageActor>::New();
  originalActor->SetInput(originalConnector->GetOutput());

  // Visualize the filtered image
  typedef itk::ImageToVTKImageFilter<UnsignedCharImageType> ConnectorType;
  ConnectorType::Pointer filteredConnector = ConnectorType::New();
  filteredConnector->SetInput(meanFilter->GetOutput());

  vtkSmartPointer<vtkImageActor> filteredActor =
    vtkSmartPointer<vtkImageActor>::New();
  filteredActor->SetInput(filteredConnector->GetOutput());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(600,300);

  vtkSmartPointer<vtkRenderer> leftRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);

  vtkSmartPointer<vtkRenderer> rightRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);

  leftRenderer->AddActor(originalActor);
  rightRenderer->AddActor(filteredActor);

  leftRenderer->ResetCamera();
  rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleImage> style =
    vtkSmartPointer<vtkInteractorStyleImage>::New();

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
