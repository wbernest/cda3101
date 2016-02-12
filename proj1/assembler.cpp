/**
 *@file ch03c.pr01.cpp
 *@authorWilliam Ernest
 *@date5/26/2015
 *@briefSimple assembler.
 *
 *@section DESCRIPTION
 * This program will translate MIPS assembly language into machine code
 ***************************************************************************/
 
#ifndef CH03C_PR01_CPP
#define CH03C_PRO1_CPP

#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <string>
#include <list>
 
void parse(std::string);
 
int main( int argc, char *argv[])
{
  std::ifstream asmFile;     // Assembly code to be translated
  asmFile.open(argv[1]);// Get the file from command line
  std::string lineIn;    // Used for parsing 
  std::list<std::string> sourceCode; // will contain instructions from asmFile
   
   do
   {
     std::ws(asmFile);
     std::getline(asmFile, lineIn);
     sourceCode.push_back(lineIn); //add to the list of instructions from source
   }while(asmFile.eof() == 0);

   for(std::list<std::string>::iterator it = sourceCode.begin(); 
       it != sourceCode.end(); ++it)
   {
     parse(*it);
   }
   return 0;
    
}
 
void parse(std::string input)
{
  boost::char_separator<char> delimeter(", ()");
  boost::tokenizer<boost::char_separator<char> > tokens(input, delimeter);
   
  for(boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin(); 
      it != tokens.end(); ++it)
  {
    std::cout << *it << std::endl;
  }
}

#endif
