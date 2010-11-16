#include "itkImageToHistogramFilter.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageRegionIteratorWithIndex.h"

#include <fstream>

typedef itk::RGBPixel<unsigned char> RGBPixelType;
typedef itk::Image< RGBPixelType, 2> RGBImageType;

void CreateImage(RGBImageType::Pointer image);

int main(int, char *[])
{
  const unsigned int MeasurementVectorSize = 3; // RGB

  RGBImageType::Pointer image = RGBImageType::New();
  CreateImage(image);

  typedef itk::Statistics::ImageToHistogramFilter< RGBImageType >         HistogramFilterType;
  typedef HistogramFilterType::HistogramMeasurementVectorType             HistogramMeasurementVectorType;
  typedef HistogramFilterType::HistogramSizeType                          HistogramSizeType;
  typedef HistogramFilterType::HistogramType                              HistogramType;

  HistogramFilterType::Pointer filter = HistogramFilterType::New();
  filter->SetInput(image);
  filter->SetAutoMinimumMaximum(true);

  HistogramSizeType histogramSize( MeasurementVectorSize );

  histogramSize[0] = 256;  // number of bins for the Red   channel
  histogramSize[1] = 256;  // number of bins for the Green channel
  histogramSize[2] = 256;  // number of bins for the Blue  channel

  // Compute the bounds of the histrogram automatically
  filter->SetHistogramSize(histogramSize);
  filter->SetMarginalScale(10); // Required (could this be set in the filter?)
  filter->Update();

  const HistogramType * histogram = filter->GetOutput();

  HistogramType::ConstIterator histogramItr = histogram->Begin();

  std::string filename = "/home/doriad/histogram.txt";
  std::ofstream fout(filename.c_str());

  while( histogramItr != histogram->End() )
    {
    //std::cout << "Index = " << histogram->GetIndex(histogramItr.GetMeasurementVector()) << "Frequency = " << histogramItr.GetFrequency() << std::endl;
    fout << "Index = " << histogram->GetIndex(histogramItr.GetMeasurementVector()) << "Frequency = " << histogramItr.GetFrequency() << std::endl;
    ++histogramItr;
    }
  fout.close();
  
  HistogramType::MeasurementVectorType mv(3);
  mv[0] = 255;
  mv[1] = 0;
  mv[2] = 0;
  std::cout << "Frequency = " << histogram->GetFrequency(histogram->GetIndex(mv)) << std::endl;
  return EXIT_SUCCESS;
}

void CreateImage(RGBImageType::Pointer image)
{
  // Create a black image with a red square and a green square.
  // This should produce a histogram with very strong spikes.
  RGBImageType::SizeType   size;
  size[0] = 3;
  size[1] = 3;

  RGBImageType::IndexType  start;
  start[0] = 0;
  start[1] = 0;

  RGBImageType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);

  image->SetRegions(region);
  image->Allocate();

  itk::ImageRegionIteratorWithIndex< RGBImageType > iterator( image, image->GetLargestPossibleRegion() );
  iterator.GoToBegin();

  RGBPixelType redPixel;
  redPixel.SetRed(255);
  redPixel.SetGreen(0);
  redPixel.SetBlue(0);

  RGBPixelType blackPixel;
  blackPixel.SetRed(0);
  blackPixel.SetGreen(0);
  blackPixel.SetBlue(0);

  itk::ImageRegionIterator<RGBImageType> imageIterator(image,region);

  while(!imageIterator.IsAtEnd())
    {
    imageIterator.Set(blackPixel);
    ++imageIterator;
    }

  RGBImageType::IndexType index;
  index[0] = 0; index[1] = 0;
  image->SetPixel(index, redPixel);

  index[0] = 1; index[1] = 0;
  image->SetPixel(index, redPixel);  
}
