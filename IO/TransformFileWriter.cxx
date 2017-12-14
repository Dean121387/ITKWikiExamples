#include "itkVersion.h"

#include "itkRigid2DTransform.h"
#include "itkTransformFileWriter.h"

int main(int argc, char *argv[])
{
  std::string fileName;
  if(argc == 1) // No arguments were provided
  {
    fileName = "test.tfm";
  }
  else
  {
    fileName = argv[1];
  }

  typedef itk::Rigid2DTransform< float > TransformType;
  TransformType::Pointer transform = TransformType::New();

  itk::TransformFileWriterTemplate<float>::Pointer writer =
    itk::TransformFileWriterTemplate<float>::New();

  writer->SetInput(transform);
  writer->SetFileName(fileName);
  writer->Update();

  return EXIT_SUCCESS;
}
