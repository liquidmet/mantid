#ifndef MANTID_SUPPORTTEST_H_
#define MANTID_SUPPORTTEST_H_

#include <cxxtest/TestSuite.h>
#include <Poco/Path.h>

#include "MantidKernel/Strings.h"
#include <string>

using namespace Mantid::Kernel::Strings;

/**
   \class StringsTest
   \brief test of Strings components
   \date September 2005
   \author S.Ansell
  
   Checks the basic string operations in Strings
*/

class StringsTest : public CxxTest::TestSuite
{
public: 
  void test_replace()
  {
    std::string in = "hello\nI hate\nnewlines.\n";
    std::string out = replace(in, "\n", " ");
    TS_ASSERT_EQUALS(out, "hello I hate newlines. ");

    TS_ASSERT_EQUALS(replace("bla", "bla", ""), "");
    TS_ASSERT_EQUALS(replace("FirstSecond", "First", ""), "Second");
    TS_ASSERT_EQUALS(replace("FirstSecond", "Second", ""), "First");
    TS_ASSERT_EQUALS(replace("Hello You", " ", " I am stupid, "), "Hello I am stupid, You");
  }

  void test_replaceAll()
  {
    const std::string input = "Lots and lots of spaces in this sentence.";
    std::string out = replaceAll(input, " ", "_");
    std::string expected = "Lots_and_lots_of_spaces_in_this_sentence.";
    TS_ASSERT_EQUALS(out, expected);

    out = replaceAll(input, "L", "Lots and l");
    expected = "Lots and lots and lots of spaces in this sentence.";
    TS_ASSERT_EQUALS(out, expected);
  }

  void testSplitEmptyPath(){
      std::vector<std::string> result;
      TSM_ASSERT(" should return 0",!split_path("",result));	
      TSM_ASSERT(" final path should be emtpy",result.empty());
  }
  void testSplitRemoveSpaces(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS(" should return 1",1,split_path("aaaa bbbb",result));	
      TSM_ASSERT_EQUALS("should replace spaces","aaaa_bbbb",result[0]);
  }
  void testSplitIn2(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS("should split in 2",2,split_path("aaaa\\bbbbb",result));	
      TS_ASSERT_EQUALS("aaaa",result[0]);	
      TS_ASSERT_EQUALS("bbbbb",result[1]);	

  }
  void testSplitIn2IgnoreEdges(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS("should split in 2",2,split_path("/aaaa\\bbbbb/",result));	
      TS_ASSERT_EQUALS("aaaa",result[0]);	
      TS_ASSERT_EQUALS("bbbbb",result[1]);	

  }
  void testSplitIn3IgnoreEdgesDelete1(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS("should split in 2",2,split_path("/aaaa\\bbbbb/./cccccc/../",result));	
      TS_ASSERT_EQUALS("aaaa",result[0]);	
      TS_ASSERT_EQUALS("bbbbb",result[1]);	

  }
  void testSplitIn3IgnoreEdgesDelete1b(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS("should split in 3",3,split_path("/aaaa\\bbbbb/./cccccc/../ee",result));	
      TS_ASSERT_EQUALS("aaaa",result[0]);	
      TS_ASSERT_EQUALS("bbbbb",result[1]);	
      TS_ASSERT_EQUALS("ee",result[2]);	

  }
  void testSplitExpandFullPath(){
      Poco::Path test;
      test = test.absolute();
      std::string wkPath = test.toString();
      std::vector<std::string> base;
      size_t depth= split_path(wkPath,base);

      std::vector<std::string> result;
      TS_ASSERT_EQUALS(depth+2,split_path("./aaaa\\bbbbb/./",result));	
      TS_ASSERT_EQUALS("aaaa",result[depth]);	
      TS_ASSERT_EQUALS("bbbbb",result[depth+1]);	


  }
  void testSplitExpandMoveUpPath(){
      Poco::Path test;
      test = test.absolute();
      std::string wkPath = test.toString();
      std::vector<std::string> base;
      size_t depth= split_path(wkPath,base);

      std::vector<std::string> result;
      TS_ASSERT_EQUALS(depth+1,split_path("../aaaa\\bbbbb/./",result));	
      TS_ASSERT_EQUALS("aaaa",result[depth-1]);	
      TS_ASSERT_EQUALS("bbbbb",result[depth]);	


  }
  void testSplitTrhowOutOfrange(){
      std::vector<std::string> result;
      TSM_ASSERT_EQUALS("should return empty path",0,split_path("/aaaa\\bbbbb/../../",result));	
      TSM_ASSERT_EQUALS("should return empty path",0,result.size());
      TSM_ASSERT_THROWS(" this path should go out of range",split_path("/aaaa\\bbbbb/../../../",result),std::invalid_argument);

  }
  void testExtractWord()
  /**
     Applies a test to the extractWord
     The object is to find a suitable lenght
     of a string in a group of words
     @retval -1 :: failed find word in string
     when the pattern exists.
  */
  {
    std::string Ln="Name wav wavelength other stuff";
    int retVal=extractWord(Ln,"wavelengt",4);
    TS_ASSERT_EQUALS(retVal, 1);
    TS_ASSERT_EQUALS(Ln, "Name wav  other stuff"); 
  }

  void testConvert()
  {
        
    int i;
    //valid double convert
    TS_ASSERT_EQUALS(convert("   568   ",i), 1);
    TS_ASSERT_EQUALS(i,568);
    double X;
    //valid double convert
    TS_ASSERT_EQUALS(convert("   3.4   ",X), 1);
    TS_ASSERT_EQUALS(X,3.4);
    X=9.0;
    //invalid leading stuff
    TS_ASSERT_EQUALS(convert("   e3.4   ",X),0);
    TS_ASSERT_EQUALS(X,9.0);
    //invalid trailing stuff
    TS_ASSERT_EQUALS(convert("   3.4g   ",X),0);
    TS_ASSERT_EQUALS(X,9.0);
    std::string Y;
    TS_ASSERT_EQUALS(convert("   3.4y   ",Y),1);
    TS_ASSERT_EQUALS(Y,"3.4y");
  }

  void testSection()
  {
    std::string Mline="V 1 tth ";
    std::string Y;
    TS_ASSERT_EQUALS(section(Mline,Y),1);
    TS_ASSERT_EQUALS(Y,"V");
    TS_ASSERT_EQUALS(Mline," 1 tth ");  // Note the non-remove spc
  }

  void testSectPartNum()
  {
    double X;
    std::string NTest="   3.4   ";
    TS_ASSERT_EQUALS(sectPartNum(NTest,X),1);
    TS_ASSERT_EQUALS(X,3.4);
    X=9.0;
    NTest="   3.4g   ";
    TS_ASSERT_EQUALS(sectPartNum(NTest,X),1);
    TS_ASSERT_EQUALS(X,3.4);
    X=9.0;
    NTest="   e3.4   ";
    TS_ASSERT_DIFFERS(sectPartNum(NTest,X),1);
    TS_ASSERT_EQUALS(X,9.0);
  }


  void test_join()
  {
    std::vector<std::string> v;
    std::string out;

    out = join(v.begin(), v.end(), ",");
    TS_ASSERT_EQUALS( out, "");

    v.push_back("Help");
    v.push_back("Me");
    v.push_back("I'm");
    v.push_back("Stuck");
    v.push_back("Inside");
    v.push_back("A");
    v.push_back("Test");

    out = join(v.begin(), v.end(), ",");
    TS_ASSERT_EQUALS( out, "Help,Me,I'm,Stuck,Inside,A,Test");
  }

  void test_endsWithInt()
  {
    TS_ASSERT_EQUALS( endsWithInt("pixel22"), 22);
    TS_ASSERT_EQUALS( endsWithInt("pixel000123"), 123);
    TS_ASSERT_EQUALS( endsWithInt("pixel99"), 99);
    TS_ASSERT_EQUALS( endsWithInt("bla123bla"), -1);
    TS_ASSERT_EQUALS( endsWithInt(""), -1);
    TS_ASSERT_EQUALS( endsWithInt("123bla"), -1);
    TS_ASSERT_EQUALS( endsWithInt("123b"), -1);
    TS_ASSERT_EQUALS( endsWithInt("123"), 123);
  }


  void test_isMember()
  {
      std::vector<std::string> group(5,"");
      group[0]="A";
      group[1]="A1";
      group[2]="B0";
      group[3]="C";

      TS_ASSERT_EQUALS(1,isMember(group,"A1"));
      TS_ASSERT_EQUALS(-1,isMember(group," "));
      TS_ASSERT_EQUALS(-1,isMember(group,"nothing"));
      TS_ASSERT_EQUALS(0,isMember(group,"A"));
      TS_ASSERT_EQUALS(3,isMember(group,"C"));
  }


};

#endif //MANTID_SUPPORTTEST_H_
