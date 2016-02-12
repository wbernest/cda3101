/**
 * @file 	wbe14b.pr02.cpp
 * @author	William Ernest
 * @date	7/12/15
 * @brief	Basic cache simulator.
 *
 * @section	DESCRIPTION
 * This program will take a cache configuration file and a 
 * memory trace file with cache "addressing" that will be used
 * to simulate hit/miss ratios of cache.
 **/
 
#ifndef WBE14B_PR02_CPP
#define WBE14B_PR02_CPP
 
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <string>
#include <list>
#include <iomanip>
#include <cmath>

struct Trace
{
  int refNum;
  std::string rw;
  int refSize;
  int address;
  int tag;
  int index;
  int offset;
  std::string hm;
};


class Cache
{
public:

  // constructor
  Cache(int css, int cls, int cs): _cacheSetSize(css), _cacheLineSize(cls),
                                   _cacheSize(cs), _misses(0),_hits(0)
  {
    // initiate a 2D array to simulate cache
    _data = new int*[GetSetNum()];
    for(int i = 0; i < GetSetNum() ; ++i)
      _data[i] = new int[_cacheSetSize];

    // fill 2D array with -1 value incase we need to 
    // check for an empty value
    for(int i = 0; i < GetSetNum(); ++i)
      for(int j = 0; j < _cacheSetSize;++j)
        _data[i][j] = -1;
  }

  // destructor
  ~Cache()
  {
    for(int i = 0; i < GetSetNum() ; ++i)
      delete [] _data[i];
    delete [] _data;
  }

  // returns number of lines
  int GetSetNum()
  {
    return _cacheSize/_cacheLineSize/_cacheSetSize;
  }

  // returns line size
  int GetLineSize()
  {
    return _cacheLineSize;
  }

  // returns hits
  int GetHits(){return _hits;}
 
  // returns misses
  int GetMisses(){return _misses;}

  // gives number of offset bit digits
  int GetOffset()
  {
    if(_cacheLineSize == 4)
      return 2;
    else if(_cacheLineSize == 8)
      return 3;
    else if(_cacheLineSize == 16)
      return 4;
    else if(_cacheLineSize == 32)
      return 5;
  }

  // will be called when the address calls for a read
  std::string Read(int index, int tag)
  {
    // If the cache set size is 1 then we can just replace
    // the tag if it is a miss
    if(_cacheSetSize == 1)
    {
      if(_data[index][0] == tag)
      {
        ++_hits;
        return "Hit";
      }
      else
      {
        _data[index][0]= tag;
        ++_misses;
        return "Miss";
      }
    }

    // if there is a hit on the first tag then we don't need to 
    // reshuffle
    if(_data[index][0] == tag)
    {
      ++_hits;
      return "Hit";
    }

    // if there is a hit some where else along the line, we need to
    // reshuffle the line moving the tag to the front and dropping
    // the LRU tag off the line
    for(int i = 0; i < _cacheSetSize ; ++i)
    {
      if(_data[index][i] == tag)
      {
        int temp;
        int next;
        for( int j = i; j > 0 ; --j)
        {                                
          temp = _data[index][j];
         _data[index][j] = _data[index][j-1];
         _data[index][j-1] = temp;
        }
        ++_hits;
        return "Hit";
      }
    }
    

    // if there is a miss we can just push the tag onto the front
    // of the line and move everything else down 1 and drop the LRU
    // tagg off the back of the line
    for(int i = _cacheSetSize - 1; i > 0; --i)
    {
      _data[index][i] = _data[index][i - 1];
    }
    _data[index][0] = tag;
    ++_misses;
    return "Miss";

  }

  // will be called for when address calls for writes
  std::string Write( int index, int tag)
  {
    // If the cache set size is 1 then we can just replace                     
    // the tag if it is a miss                                                 
    if(_cacheSetSize == 1)
    {
      if(_data[index][0] == tag)
      {
        ++_hits;
        return "Hit";
      }
      else
      {
        _data[index][0]= tag;
        ++_misses;
        return "Miss";
      }
    }

    // if there is a hit on the first tag then we don't need to                
    // reshuffle                                                               
    if(_data[index][0] == tag)
    {
      ++_hits;
      return "Hit";
    }

    // if there is a hit some where else along the line, we need to            
    // reshuffle the line moving the tag to the front and dropping             
    // the LRU tag off the line                                                
    for(int i = 0; i < _cacheSetSize ; ++i)
    {
      if(_data[index][i] == tag)
      {
        int temp;
        int next;
        for( int j = i; j > 0 ; --j)
        {
          temp = _data[index][j];
          _data[index][j] = _data[index][j-1];
          _data[index][j-1] = temp;
        }
        ++_hits;
        return "Hit";
      }
    }

    // if there is a miss we can just push the tag onto the front              
    // of the line and move everything else down 1 and drop the LRU            
    // tagg off the back of the line                                           
    for(int i = _cacheSetSize - 1; i > 0; --i)
    {
      _data[index][i] = _data[index][i - 1];
    }
    _data[index][0] = tag;
    ++_misses;
    return "Miss";

  }

  // this will print the cache diminsions
  void PrintConfig()
  {
    std::cout << "Total Cache Size:  " << _cacheSize << "B\n"
              << "Line Size:  " << _cacheLineSize << "B\n"
              << "Set Size:  " << _cacheSetSize << std::endl
              << "Number of Sets:  " << GetSetNum() << std::endl;
  }

  // this is a debug feature that allows you to see whats in the 
  // cache
  void PrintCache()
  {
    for( int i = 0 ; i < GetSetNum(); ++i)
    {
      for(int j = 0; j < _cacheSetSize ; ++j)
        std::cout << _data[i][j] << ' ';
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

private:
  // Data will be stored for cache in a 2D array. We will setup the cache
  // using the cache size, block size, and line size. 
  int _cacheSetSize;
  int _cacheLineSize;
  int _cacheSize;
  int** _data;
  int _hits;
  int _misses;
};


void ParseAddress(Cache &, std::list<Trace> &, std::list<std::string> &);
void PrintTable();
void PrintTrace(std::list<Trace> &);
void PrintSummary(Cache &);
 
int main(int argc, char * argv[])
{
  std::ifstream cacheConfigFile;	// Cache config file
  std::ifstream memoryTraceFile;	// Memory trace file
  std::list<std::string> memoryTrace;        // will hold memory traces
  std::list<Trace> memoryTraceResults;
  std::string lineIn;
  int cacheSetSize;	                // Cache set size
  int cacheLineSize;                    // Cache Line size
  int cacheSize;			// Cache total size
    	 
  cacheConfigFile.open(argv[1]);	// Get the file from command line
  cacheConfigFile >> cacheSetSize;
  cacheConfigFile >> cacheLineSize;
  cacheConfigFile >>  cacheSize;

  cacheConfigFile.close();		// close config file

  memoryTraceFile.open(argv[2]);        // open trace file

  do                                    // put the file into a list
  {
    std::getline(memoryTraceFile, lineIn);
    if(!memoryTraceFile) break;
    memoryTrace.push_back(lineIn);
  }while(memoryTraceFile.eof() == 0);  

  memoryTraceFile.close();              // cloase trace file

  // initialize cache
  Cache cache = Cache(cacheSetSize, cacheLineSize, cacheSize);
  std::cout << std::endl;
  
  // print cache diminsions
  cache.PrintConfig();
  std::cout << std::endl;
  
  // parse address and tally hits and misses
  ParseAddress(cache,memoryTraceResults,memoryTrace);
  
  // print results table header
  PrintTable();
  
  // print results from memmory trace
  PrintTrace(memoryTraceResults);
  
  // print the results from the hit and miss summary
  PrintSummary(cache);
  
  // used for debugging
  //cache.PrintCache();
  return 0;

	 
}


// parse address will take the string from the memorytrace file list and parse
// it into the action, acces size, and address. It will then calculate the 
// tag, index, and offset. Then it will run the trace to check hits and misses
void ParseAddress(Cache &c, std::list<Trace> &mt, std::list<std::string> &mtf)
{
  int i = 0;                             // to be stored as reference number
  // used as offset number size in bits
  int offsetNum = (int)log2(c.GetLineSize());
  // used as index number size in bits
  int bitNum = (int)log2(c.GetSetNum());
  // temp variable to populate before pushing onto list
  Trace temp;

  // begin parsing address using tokenizer
  for(std::list<std::string>::iterator it = mtf.begin(); it != mtf.end();
      ++it)
  {
    temp.refNum = i;
    boost::char_separator<char> delimeter(":");
    boost::tokenizer< boost::char_separator<char> > tokens ( *it, delimeter);
    boost::tokenizer< boost::char_separator<char> >::iterator itr 
      = tokens.begin();
    
    // check for read or write command
    if((*itr) == "R")
      temp.rw = " Read";
    else
      temp.rw = "Write";
    
    // get access size
    ++itr;
    temp.refSize = atoi((*itr).c_str());

    // get address
    ++itr;
    temp.address = strtoul((*itr).c_str(), NULL, 16);

    // calculate value for offset
    temp.offset = temp.address & (c.GetLineSize() - 1);

    // calculate index value
    temp.index = (temp.address & ( (c.GetSetNum() -  1) << offsetNum))
      >> offsetNum;

    // calculate tag value
    temp.tag = (temp.address & 
                (0xFFFFFFFF << offsetNum << bitNum)) >> offsetNum >> bitNum;
    
    //perform memory trace
    if(temp.rw == " Read")
      temp.hm = c.Read(temp.index, temp.tag);
    else
      temp.hm = c.Write(temp.index, temp.tag);

    // push temp onto back of list
    mt.push_back(temp);

    // for debugging
    //c.PrintCache();
    
    ++i;
  }



}

// This will print the table header using Dr. Hughes' format
void PrintTable()
{
  std::cout << std::setw(8)  << std::left << "RefNum";
  std::cout << std::setw(10) << std::left << "  R/W";
  std::cout << std::setw(13) << std::left << "Address";
  std::cout << std::setw(6)  << std::left << "Tag";
  std::cout << std::setw(8)  << std::left << "Index";
  std::cout << std::setw(10) << std::left << "Offset";
  std::cout << std::setw(8)  << std::left << "H/M";
  std::cout << std::setfill('*') << std::setw(64) << "\n" 
            << std::setfill(' ');
  std::cout << "\n";

}

// this will print the trace results using Dr. Hughes' format
void PrintTrace(std::list<Trace> &mt)
{
  for(std::list<Trace>::iterator i = mt.begin(); i != mt.end(); ++i)
    std::cout << "   " << std::setw(5) << std::left << (*i).refNum 
              << std::setw(8) << (*i).rw << "  " 
              << std::setw(8) << std::setfill('0') << std::hex 
              << std::right << (*i).address << std::setfill(' ')
              << std::setw(7) << (*i).tag 
              << std::setw(8) << std::dec << (*i).index 
              << std::setw(8) << (*i).offset 
              << std::setw(10) << (*i).hm << std::endl;
}

// this will print the hit or miss summary using Dr. Hughes' format
void PrintSummary(Cache &c)
{
  float hr = (float)c.GetHits()/(c.GetHits()+c.GetMisses());
  float mr = (float)c.GetMisses()/(c.GetHits()+c.GetMisses());
  std::cout << std:: endl 
            << "    Simulation Summary\n"
            << "**************************\n"
            << "Total Hits:\t" << c.GetHits() << std::endl
            << "Total Misses:\t" << c.GetMisses() << std::endl
            << "Hit Rate:\t" << std::setprecision(5) << hr << '\n'
            << "Miss Rate:\t" << std::setprecision(5) << mr << '\n';
}


#endif
