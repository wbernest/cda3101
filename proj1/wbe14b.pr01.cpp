/**
 *	@file 		wbe14b.pr01.cpp
 *	@author		William Ernest
 *	@date		5/26/2015
 *	@brief		Simple assembler.
 *
 *	@section 	DESCRIPTION	
 * 	This program will translate MIPS assembly 
 *      language into machine code
 **********************************************************/
 
 #ifndef wbe14b_PR01_CPP
 #define wbe14b_PR01_CPP
 
 #include <iostream>
 #include <fstream>
 #include <boost/tokenizer.hpp>
 #include <string>
 #include <list>
 #include <iomanip>
 
// used for setting up address table for first 
// pass of assembler
 struct label
 {
	std::string labelName;
	int address;
 };
 
 // used for setting up machine code
 struct machine
 {
	 int address;
	 int machineCode;
 };
 
 // passes
 void PassOne(std::list<std::string>&, std::list<label>&, 
              std::list<machine>&);
 void PassTwo(std::list<std::string>&, std::list<label>&,
              std::list<machine>&);
 
 // helper functions to get integers values 
 int opCode(std::string);
 int regCode(std::string);
 int functCode(std::string);

 // helper functions to test for various scenarios
 bool isRType(std::string);
 bool isIType(std::string);
 bool isJType(std::string);
 bool isThreeArg(std::string);
 bool isTwoArg(std::string);
 bool isOneArg(std::string);
 
 
 int main( int argc, char *argv[])
 {
   std::ifstream asmFile;    // code to be translated
   asmFile.open(argv[1]);    // Get file from CLA
	 
   std::string lineIn;	    // Used for parsing

   // will contain instructions from asmFile
   std::list<std::string> sourceCode;	
   // will contain finished machineCode for output
   std::list<machine> machineCode;
   // will contain the address table made in first pass
   std::list<label> addressTable;	
   std::ofstream outFile;   // output obj file 

	 
   // put asmFile line by line into sourceCode
   do
   {
     std::ws(asmFile);
     std::getline(asmFile, lineIn);
     if(!asmFile) break;
     sourceCode.push_back(lineIn); 
   }while(asmFile.eof() == 0);
	
   // Run the first pass of the assembler
   PassOne(sourceCode, addressTable, machineCode);
   PassTwo(sourceCode, addressTable, machineCode);

   // for output machine code into file
   std::string out;
   int n = 0;
   while( argv[1][n] != '.')
   {
     out = out + argv[1][n];
     ++n;
   }
   out = out + ".obj";

   std::cout << std::endl;
   outFile.open(out.c_str());
	
   for(std::list<machine>::iterator it = machineCode.begin(); 
       it != machineCode.end(); ++it)
   {
     outFile << std::hex << std::setw(8) << std::setfill('0') 
             <<(*it).machineCode << std::endl;

     std::cout << std::hex << std::setw(8) << std::setfill('0')
               <<(*it).machineCode << std::endl; 
   }
	
		
   return 0;
	 
}

 
// in pass one we will find all the labels and 
// assign any data to their places in memory
void PassOne(std::list<std::string> &sourceCode, 
             std::list<label> &addressTable,
             std::list<machine> &machineCode)
{
  int globalPointer = 0;
  label lbl;
  machine mc;
  bool flag2 = false;
	 
  // in the .text section we will iterate through each line of 
  // code and see if it is a label, if so put it in the 
  // label table
  std::list<std::string>::iterator it = sourceCode.begin(); 
  while(!flag2 && it != sourceCode.end())	
  {
    // flag when we get to .data so we can move to that loop
    flag2 = (*it) == ".data";
    if((*it) == ".text")  // if the string is .text go to next
    {
      ++it;
      continue;
    }
		
    // if the string contains a : then it is a label and we 
    // will place it into the address table
    if( (*it).find_first_of(":") != std::string::npos)
    {
      lbl.labelName = (*it).substr(0,(*it).find_first_of(":"));
      lbl.address = globalPointer;
      addressTable.push_back(lbl);
      ++it;
      continue;
    }
    ++globalPointer;
    ++it;
  }

  // we will iterate through the .data section and place 
  // labels in the address table and the words we want placed 
  // in memory will be put into the machine code
  while( it != sourceCode.end())
  {
    // if the string is .data move to the next line of code
    if(*it == ".data")	
    {
      it++;
      continue;
    }
    
    // we will use boost tokenizer to parse apart code
    boost::char_separator<char> delimeter(":, ");
    boost::tokenizer< boost::char_separator<char> > tokens( *it , delimeter);
    boost::tokenizer< boost::char_separator<char> >::iterator itr = tokens.begin();
		
    // put the label name and address in the label table
    lbl.address = globalPointer;
    lbl.labelName = *itr;
    addressTable.push_back(lbl);
    
    // move to the next chunk of code
    ++itr;
    // if .word we will put the next item into memory at the 
    // current address
    if((*itr) == ".word")
    {
      // move to the next chunk of code
      ++itr;
      // we have given the option to list words in 1 line
      while( itr != tokens.end())
      {
        mc.address = globalPointer;
        mc.machineCode = atoi((*itr).c_str());
        machineCode.push_back(mc);
        ++globalPointer;
        ++itr;
      }
    }
    
    // if the word is .space we will place n sets of 0 on the 
    // back of the machine code
    else if((*itr) == ".space")
    {
      ++itr;
      for( int x = 0; x < (atoi((*itr).c_str())); ++x )
      {
        mc.address = globalPointer;
        mc.machineCode = 0;
        machineCode.push_back(mc);
        ++globalPointer;
      }
    }
    ++it;
    
  }
}
 
// in pass two we will generate the machine code for every 
// line of code
void PassTwo(std::list<std::string> &sourceCode,
             std::list<label> &addressTable, 
             std::list<machine> &machineCode)
{
  int globalPointer = 0;
  std::list<machine>::iterator iter = machineCode.begin();
  
  // begin stepping through code
  for(std::list<std::string>::iterator it = sourceCode.begin();
      it != sourceCode.end(); ++it)
  {
    int mc = 0;			// to hold temp machine code
    machine code;
    
    // use tokenizer to break up string by ,  ()
    boost::char_separator<char> delimeter(", ()");
    boost::tokenizer< boost::char_separator<char> > tokens( *it , delimeter);
    boost::tokenizer< boost::char_separator<char> >::iterator itr = tokens.begin();
    
    // if .text or .data go to next iteration
    if((*itr) == ".text" || (*itr) == ".data") continue;
    // if a label go to next iteration
    else if((*itr).find_first_of(':') != std::string::npos ) 
      continue;
    // if a line of code, convert to machine code
    else
    {  
      // if it is a Rtype instruction
      if(isRType(*itr))
      { // with 1 argument
        if(isOneArg(*itr))
        {
          mc = mc | (opCode(*itr) << 26); // put in op code 
          mc = mc | (functCode(*itr));    // put in funct code
          ++itr;
          mc = mc | (regCode(*itr) << 11); // put in reg code
          
        }
        // if it has 2 arguments
        else if(isTwoArg(*itr))
        {
          mc = mc | (opCode(*itr) << 26);  // put in op code
          mc = mc | (functCode(*itr));     // put in funct code
          ++itr;
          mc = mc | (regCode(*itr) << 21); // put in rs code
          ++itr;
          mc = mc | (regCode(*itr) << 16); // put in rt code
          
        }
        // if it has 3 arguments
        else if(isThreeArg(*itr))
        {
          mc = mc | (opCode(*itr) << 26);  // put in op code
          mc = mc | (functCode(*itr));     // put in funct code
          ++itr;
          mc = mc | (regCode(*itr) << 11);   // put in rd code
          ++itr;
          mc = mc | (regCode(*itr) << 21);   // put in rs code
          ++itr;
          mc = mc | (regCode(*itr) << 16);   // put in rt code
          
        }
        // if syscall just put in 12 for machine code
        else mc = 12;
      }
      // if it is a I type instruction
      else if(isIType(*itr))
      {
        // if addiu
        if((*itr) == "addiu")
        {
          mc = mc | (opCode(*itr) << 26);     // put in op code
          ++itr;
          mc = mc | (regCode(*itr) << 16);    // put in rt
          ++itr;
          mc = mc | (regCode(*itr) << 21);    // put in rs
          ++itr;
          mc = mc | (atoi((*itr).c_str()));    // load offset
        }
        // if one of the branch instructions
        else if((*itr) == "beq" || (*itr) == "bne")
        {
          mc = mc | (opCode(*itr) << 26);   // put in op code
          ++itr;                               
          mc = mc | (regCode(*itr) << 21);  // put in rs
          ++itr;
          mc = mc | (regCode(*itr) << 16);  // put in rt
          ++itr;
          // we will go search the address table for the 
          // address of the label used
          for(std::list<label>::iterator itt = addressTable.begin(); itt != addressTable.end(); ++itt)
          {
            if((*itr) == (*itt).labelName ) 
            {
              // we will subtract the global pointer from the 
              // address, masking the first 16 bits in case 
              // of negative offset
              int mask = 0x0000FFFF;
              mc = mc | (mask &((*itt).address - globalPointer - 1));
              continue;
            }
          }
        }
        else // if lw and sw
        {
          mc = mc | (opCode(*itr) << 26);     // put in op code
          ++itr;
          mc = mc | (regCode(*itr) << 16);    // put in rt code
          ++itr;
          // next we will search the address table in the 
          // case of a label being used
          for(std::list<label>::iterator itt = addressTable.begin(); itt != addressTable.end(); ++itt)
          {
            if((*itr) == (*itt).labelName ) 
            {
              // we will go get the data that the label points
              // to from the machine code
              std::list<machine>::iterator ittr = machineCode.begin();
              while( (*itt).address != (*ittr).address)
              {
                ++ittr;
              }	
              mc = mc | ((*ittr).machineCode);  // load offset
              continue;
            }
            else
            {
              mc = mc | atoi((*itr).c_str());
            }
          }
          ++itr;
          mc = mc | (regCode(*itr) << 21);   // put in rs code
        }
	
      }
      // if j type
      else if(isJType(*itr))
      {
        mc = mc | (opCode(*itr) << 26);   // load in op code
        ++itr;
        // go get address from the address table
        for(std::list<label>::iterator itt = addressTable.begin(); itt != addressTable.end(); ++itt)
        {
          if((*itr) == (*itt).labelName ) 
          {
            mc = mc | ((*itt).address);   // load address
            continue;
          }
        }  
      } 
      ++globalPointer;
    }
    
    // place address and machine code into list
    code.address = globalPointer;
    code.machineCode = mc;
    machineCode.insert(iter, code);
  }
}
 
// opCode, regCode, and functCode are helper functions, merely 
// returning the integer associated with the string
int opCode(std::string s)
{
  if ( s ==  "addiu")  return 9;
  else if ( s ==  "addu" )	return 0;
  else if ( s ==  "and")		return 0;
  else if ( s ==  "beq")		return 4;
  else if ( s ==  "bne")		return 5;
  else if ( s ==  "div")		return 0;
  else if ( s ==  "j")		return 2;
  else if ( s ==  "lw")		return 35;
  else if ( s ==  "mfhi")	return 0;
  else if ( s ==  "mflo")	return 0;
  else if ( s ==  "mult")	return 0;
  else if ( s ==  "or")		return 0;
  else if ( s ==  "slt")		return 0;
  else if ( s ==  "subu")	return 0;
  else if ( s ==  "sw")		return 43;
  else if ( s ==  "syscall")	return 0;
  else 		return 0;
 }
 
int regCode(std::string s)
{
  if ( s ==  "$0")			return 0;
  else if ( s ==  "$zero")	return 0;
  else if ( s ==  "$at")		return 1;
  else if ( s ==  "$v0")		return 2;
  else if ( s ==  "$v1")		return 3;
  else if ( s ==  "$a0")		return 4;
  else if ( s ==  "$a1")		return 5;
  else if ( s ==  "$a2")		return 6;
  else if ( s ==  "$a3")		return 7;
  else if ( s ==  "$t0")		return 8;
  else if ( s ==  "$t1")		return 9;
  else if ( s ==  "$t2")		return 10;
  else if ( s ==  "$t3")		return 11;
  else if ( s ==  "$t4")		return 12;
  else if ( s ==  "$t5")		return 13;
  else if ( s ==  "$t6")		return 14;
  else if ( s ==  "$t7")		return 15;
  else if ( s ==  "$s0")		return 16;
  else if ( s ==  "$s1")		return 17;
  else if ( s ==  "$s2")		return 18;
  else if ( s ==  "$s3")		return 19;
  else if ( s ==  "$s4")		return 20;
  else if ( s ==  "$s5")		return 21;
  else if ( s ==  "$s6")		return 22;
  else if ( s ==  "$s7")		return 23;
  else if ( s ==  "$t8")		return 24;
  else if ( s ==  "$t9")		return 25;
  else if ( s ==  "$k0")		return 26;
  else if ( s ==  "$k1")		return 27;
  else if ( s ==  "$gp")		return 28;
  else if ( s ==  "$sp")		return 29;
  else if ( s ==  "$fp")		return 30;
  else if ( s ==  "$ra")		return 31;
  else			return 0;
}

int functCode(std::string s)
{
  if ( s ==  "addu")	return 33;
  else if ( s ==  "and")		return 36;
  else if ( s ==  "div")		return 26;
  else if ( s ==  "mfhi")	return 16;
  else if ( s ==  "mflo")	return 18;
  else if ( s ==  "mult")	return 24;
  else if ( s ==  "or")		return 37;
  else if ( s ==  "slt")		return 42;
  else if ( s ==  "subu")	return 35;
  else if ( s ==  "syscall")	return 12;
  else 		return 0;
}


// the rest of the functions are helper functions that 
// return a boolean for testing is a type
bool isRType(std::string s)
{
  if ( s ==  "addu")	return true;
  else if ( s ==  "and")		return true;
  else if ( s ==  "div")		return true;
  else if ( s ==  "mfhi")	return true;
  else if ( s ==  "mflo")	return true;
  else if ( s ==  "mult")	return true;
  else if ( s ==  "or")		return true;
  else if ( s ==  "slt")		return true;
  else if ( s ==  "subu")	return true;
  else if ( s ==  "syscall")	return true;
  else 		return false;
}

bool isIType(std::string s)
{
  if ( s ==  "addiu")	return true;
  else if ( s ==  "beq")	return true;
  else if ( s ==  "bne")	return true;
  else if ( s ==  "lw")		return true;
  else if ( s ==  "sw")		return true;
  else		return false;
}

bool isJType(std::string s)
{
  if ( s ==  "j")		return true;
  else		return false;
}
 
bool isThreeArg(std::string s)
{
  if ( s ==  "addu")	return true;
  else if ( s ==  "and")		return true;
  else if ( s ==  "or")		return true;
  else if ( s ==  "slt")		return true;
  else if ( s ==  "subu")	return true;
  else 		return false;
}
 
bool isTwoArg(std::string s)
{
  if ( s ==  "div")		return true;
  else if ( s ==  "mult")	return true;
  else 		return false;
  
}
 
bool isOneArg(std::string s)
{
  if(s == "mfhi")	return true;
  else if( s == "mflo")	return true;
  else return false;
  
}
 
#endif
