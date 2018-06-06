#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ims/Store.hh"
#include "ims/Images.hh"
#include "ims/Image.hh"
#include "ims/Source.hh"
#include "ims/Slice.hh"
#include "ims/Stripe.hh"
#include "ims/WaveFront.hh"
#include "daq/Location.hh"
#include "daq/LocationSet.hh"
#include "daq/Sensor.hh"
#include "daq/WavefrontSet.hh"

#define PARTITION argv[1]
#define IMAGE     argv[2]
#define PIX_MASK 0x20000
static const int NUMAMPS = 16;
static const char ERROR[] = "A partition name and image name must be provided as a command line argument\n";

static void close_file_handles(std::vector<std::ofstream*> &);

int main(int argc, char **argv)
{
  if(argc != 3) 
  {
    std::cout << ERROR;
    return EXIT_FAILURE;
  }
  IMS::Store store(PARTITION);
  IMS::Images images(store);
  const char* id;
  DAQ::Location location;

  //IMS::Image::header();
  std::vector<std::ofstream*> fhAts;

  for(auto x = 0; x < NUMAMPS; ++x)
  {
    std::ostringstream seg;
    seg << std::setfill('0') << std::setw(2) << x;
    std::ostringstream fns;
    fns << "WFS_CCD0_segment." << seg.str();
    std::ofstream *fh = new std::ofstream(fns.str(), std::ios::out | std::ios::app | std::ios::binary );
    fhAts.push_back(fh); 
  } 

  while((id = images.id()))
  {
    IMS::Image image(id, images);
    //std::cout << image.name() << std::endl;    
    if(std::strcmp(IMAGE, image.name()) == 0)
    {
      std::cout << image.name() << std::endl;
      //image.synopsis();
      DAQ::LocationSet sources = image.sources();
      sources &= DAQ::WavefrontSet();
      while(sources.remove(location))
      {
        location.print();
        IMS::Source source(location, image);
        std::cout << "Number of CCDs: " << source.numof_ccds() << std::endl;
        source.synopsis();
        IMS::WaveFront slice(source);
      	unsigned total_stripes = 0;
      	do
        {
          total_stripes += slice.stripes();
      	  IMS::Stripe *ccd0 = new IMS::Stripe[slice.stripes()];
          slice.decode(ccd0);
          std::cout << "Slice " << slice.number();
          std::cout << " Size: " << slice.stripes();
          std::cout << std::endl;
          //std::cout << "Amp Start: " << amp << std::endl;
      	  for(auto s = 0; s < slice.stripes(); ++s)
          {
      	    for(auto amp = 0; amp < NUMAMPS; ++amp)
            {
              int32_t X = PIX_MASK ^ ((ccd0[s].segment[amp]));
              fhAts[amp]->write(reinterpret_cast<const char *>(&X), 4);
      	    }
      	  }  
          delete[] ccd0;
          //std::cout << "Amp End: " << amp << std::endl;
        } while(slice.advance());

        close_file_handles(fhAts);
 	      std::cout << "Total Number of Stripes: " << total_stripes << std::endl;
      }
    }
  }
  return EXIT_SUCCESS;
}

void close_file_handles(std::vector<std::ofstream*> &fh)
{
  for(auto *f: fh)
  {
    f->close();
  }
}
