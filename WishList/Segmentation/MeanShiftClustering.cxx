#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include <itkScalarImageToListAdaptor.h>
#include <itkKdTree.h>
#include <itkKdTreeGenerator.h>
#include <itkMeanShiftModeCacheMethod.h>
#include <itkHypersphereKernelMeanShiftModeSeeker.h>
#include <itkSampleMeanShiftBlurringFilter.h>
#include <itkSampleMeanShiftClusteringFilter.h>

#include <itkImageFileWriter.h>


int main(int argc, char* argv[] ) 
{
  if (argc < 2)
    {
      std::cout << "ERROR: data file name argument missing." 
                << std::endl ;
      return EXIT_FAILURE;
    }

  std::string inputFilename = argv[1];
  
  typedef unsigned char PixelType ;
  typedef itk::Image< PixelType, 2 > ImageType ;
  typedef itk::ImageFileReader< ImageType > ImageReaderType ;
  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;

  imageReader->SetFileName(inputFilename.c_str());
  imageReader->Update() ;
  ImageType::Pointer image = imageReader->GetOutput() ;
  
  typedef itk::Statistics::ScalarImageToListAdaptor< ImageType >
    ListSampleType ;
  
  ListSampleType::Pointer listSample = 
    ListSampleType::New() ;
  listSample->SetImage( image ) ;

  typedef itk::Statistics::KdTreeGenerator< ListSampleType > 
    TreeGeneratorType ;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New() ;
  treeGenerator->SetSample( listSample ) ;
  treeGenerator->SetBucketSize( 200 ) ;
  treeGenerator->Update() ;

  typedef TreeGeneratorType::KdTreeType TreeType ;
  TreeType::Pointer tree = treeGenerator->GetOutput() ;

  typedef itk::Statistics::HypersphereKernelMeanShiftModeSeeker< 
    TreeType > ModeSeekerType ;
  ModeSeekerType::Pointer modeSeeker = ModeSeekerType::New() ;
  modeSeeker->SetInputSample( tree ) ;
//  modeSeeker->SetInputSample( listSample ) ;
  modeSeeker->SetSearchRadius( 4.0 ) ;

  typedef itk::Statistics::MeanShiftModeCacheMethod< TreeType::MeasurementVectorType > CacheMethodType ;
  CacheMethodType::Pointer cacheMethod = CacheMethodType::New() ;
  cacheMethod->SetMaximumEntries(255) ;
  cacheMethod->SetMaximumConsecutiveFailures(100) ;
  cacheMethod->SetHitRatioThreshold( 0.5 ) ;
  modeSeeker->SetCacheMethod( cacheMethod.GetPointer() ) ;

  typedef itk::Statistics::SampleMeanShiftBlurringFilter< TreeType >
    FilterType ;
  FilterType::Pointer filter = FilterType::New() ;
  filter->SetInputSample( tree ) ;
  filter->SetMeanShiftModeSeeker( modeSeeker ) ;
  filter->Update() ;

  std::cout << "Cache statistics: " << std::endl ;
  cacheMethod->Print(std::cout) ;

  typedef ImageType OutputImageType ;
  OutputImageType::Pointer outputImage = OutputImageType::New() ;
  outputImage->SetRegions( image->GetLargestPossibleRegion() ) ;
  outputImage->Allocate() ;

  typedef itk::ImageRegionIterator< OutputImageType > ImageIteratorType ;
  ImageIteratorType io_iter( outputImage,
                             outputImage->GetLargestPossibleRegion() ) ;
  io_iter.GoToBegin() ;

  FilterType::OutputType::Pointer output = filter->GetOutput() ;
  FilterType::OutputType::Iterator fo_iter = output->Begin() ;
  FilterType::OutputType::Iterator fo_end = output->End() ;

  while ( fo_iter != fo_end )
    {
    io_iter.Set( (PixelType) fo_iter.GetMeasurementVector()[0]) ;
    ++fo_iter ;
    ++io_iter ;
    }

  ListSampleType::Pointer listSample2 = ListSampleType::New() ;
  listSample2->SetImage( outputImage ) ;

  TreeGeneratorType::Pointer treeGenerator2 = TreeGeneratorType::New() ;
  treeGenerator2->SetSample( listSample2 ) ;
  treeGenerator2->SetBucketSize( 200 ) ;
  treeGenerator2->Update() ;

  typedef itk::Statistics::SampleMeanShiftClusteringFilter< TreeType >
    ClusteringMethodType ;

  ClusteringMethodType::Pointer clusteringMethod =
    ClusteringMethodType::New() ;
  clusteringMethod->SetInputSample( treeGenerator2->GetOutput() ) ;
  clusteringMethod->SetThreshold( 0.5 ) ;
  clusteringMethod->SetMinimumClusterSize( 16 ) ;
  clusteringMethod->DebugOn() ;
  clusteringMethod->Update() ;

  // save clustered image
  OutputImageType::Pointer clusterMap = OutputImageType::New() ;
  clusterMap->SetRegions( image->GetLargestPossibleRegion() ) ;
  clusterMap->Allocate() ;
  
  ImageIteratorType m_iter( clusterMap, 
                            clusterMap->GetLargestPossibleRegion() ) ;
  m_iter.GoToBegin() ;
  
  ClusteringMethodType::ClusterLabelsType clusterLabels = 
    clusteringMethod->GetOutput() ;
  
  ClusteringMethodType::ClusterLabelsType::iterator co_iter = 
    clusterLabels.begin() ;
  
  while ( co_iter != clusterLabels.end() )
    {
    m_iter.Set( (PixelType) *co_iter ) ;
    ++co_iter ;
    ++m_iter ;
    }
  
  typedef itk::ImageFileWriter< OutputImageType > ImageWriterType ;
  ImageWriterType::Pointer map_writer = ImageWriterType::New() ;
  map_writer->SetFileName("clustered_sf4.png") ;
  map_writer->SetInput( clusterMap ) ;
  map_writer->Update() ;


  return EXIT_SUCCESS;
}
