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
#include "ims/Science.hh"
#include "daq/Location.hh"
#include "daq/LocationSet.hh"
#include "daq/ScienceSet.hh"

#define PARTITION argv[1]
#define IMAGE     argv[2]
#define PIX_MASK 0x1FFFF
static const int NUMAMPS = 16;
static const char ERROR[] = "A partition name and image name must be provided as a command line argument\n";

static void close_file_handles(std::vector<std::ofstream*> &);
static void create_file_handles(std::vector<std::ofstream*> &,
                                unsigned, unsigned);
static void process_ccd(std::vector<std::ofstream*> const &,
                        const IMS::Stripe *, signed);

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
 

  while((id = images.id()))
  {
    IMS::Image image(id, images);
    //std::cout << image.name() << std::endl;    
    if(std::strcmp(IMAGE, image.name()) == 0)
    {
      std::cout << image.name() << std::endl;
      //image.synopsis();
      DAQ::LocationSet sources = image.sources();
      sources &= DAQ::ScienceSet();
      while(sources.remove(location))
      {
        std::vector<std::ofstream*> fh0;
        std::vector<std::ofstream*> fh1;
        std::vector<std::ofstream*> fh2;

        location.print();
	int board = location.board();
        create_file_handles(fh0, 0, board);
        create_file_handles(fh1, 1, board);
        create_file_handles(fh2, 2, board);

        IMS::Source source(location, image);
        std::cout << "Number of CCDs: " << source.numof_ccds() << std::endl;
        source.synopsis();
        IMS::Science slice(source);
        unsigned total_stripes = 0;
      	do
        {
          total_stripes += slice.stripes();
          IMS::Stripe *ccd0 = new IMS::Stripe[slice.stripes()];
      	  IMS::Stripe *ccd1 = new IMS::Stripe[slice.stripes()];
      	  IMS::Stripe *ccd2 = new IMS::Stripe[slice.stripes()];
          slice.decode012(ccd0, ccd1, ccd2);
          std::cout << "Slice " << slice.number();
      	  std::cout << " Size: " << slice.stripes() << std::endl;
          process_ccd(fh0, ccd0, slice.stripes());
          process_ccd(fh1, ccd1, slice.stripes());
          process_ccd(fh2, ccd2, slice.stripes());
          delete[] ccd0;
          delete[] ccd1;
          delete[] ccd2;
        } while(slice.advance());
        close_file_handles(fh0);
        close_file_handles(fh1);
        close_file_handles(fh2);
        std::cout << "Total Number of Stripes: " << total_stripes << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}

void process_ccd(std::vector<std::ofstream*> const &fhc,
                 const IMS::Stripe *ccd, signed numStripes)
{
  for(auto s = 0; s < numStripes; ++s)
  {
    for(auto amp = 0; amp < NUMAMPS; ++amp)
    {
      // This will eventually not be necessary, but requires a DAQ update
      int32_t X = PIX_MASK ^ ((ccd[s].segment[amp]));
      fhc[amp]->write(reinterpret_cast<const char *>(&X), 4);
    }
  } 
}

void create_file_handles(std::vector<std::ofstream*> &fhc,
                         unsigned ccd, unsigned bay)
{
  for(auto x = 0; x < NUMAMPS; ++x)
  {
    std::ostringstream seg;
    seg << std::setfill('0') << std::setw(2) << x;
    std::ostringstream fns;
    fns << "SCI_REB" << bay << "_CCD" << ccd << "_segment." << seg.str();
    std::ofstream *fh = new std::ofstream(fns.str(), std::ios::out | std::ios::app | std::ios::binary );
    fhc.push_back(fh); 
  } 
}

void close_file_handles(std::vector<std::ofstream*> &fhc)
{
  for(auto *fh: fhc)
  {
    fh->close();
  }
}
