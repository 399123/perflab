#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rtdsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();

  
  int divisor = filter-> getDivisor();//pulled all possible memory calls out of loop.
  int wid = input -> width-1;
  int heigh = input -> height-1;
  int size = filter -> getSize();
  output -> width = wid;
  output -> height = heigh;
  int value1;
  int value2;
  int value3;
  int value4;
  int value5;
  int value6;
  int value7;
  int value8;
  int value9;
  int Filters[size][size];

  
  for(int i=0; i<size;i++){
	for(int j=0;j<size;j++){
		Filters[i][j]=filter->get(i,j); // make array of filter so no need to call filter in loop
		  	
	}
  }	
	  for(int plane = 0; plane < 3; ++plane) { // switched plane and colum loops, so that color is outermost ot inermost
		for(int row = heigh ; row > 0 ; row -=3) { // change plane and col for loop order
		  for(int col = wid; col > 0; col -=3) { 
			value1=0;// loop unfolling used so col/ row go by 3s
			value2=0;
			value3=0;
			value4=0;
			value5=0;
			value6=0;
			value7=0;
			value8=0;
			value9=0;  
		
		for (int j = 0; j < size; ++j) {
			int row1 = row + j-1;
			int row2 = row + j;
			int row3 = row + 1+j;
		  for (int i = 0; i < size; ++i) {
			int col1= col+i-1;	// make var so not math in array 
			int col2= col+i;
			int col3= col+i+1; 
			int filterit= Filters[j][i];// make fxn for filter
			if (filterit ==1){
				value1 += input->color[plane][row1][col1];
				value2 += input->color[plane][row2][col2];
				value3 += input->color[plane][row3][col3];
				value4 += input->color[plane][row1][col1];
				value5 += input->color[plane][row2][col2];
				value6 += input->color[plane][row3][col3];
				value7 += input->color[plane][row1][col1];
				value8 += input->color[plane][row2][col2];
				value9 += input->color[plane][row3][col3];
			}
			else if (filterit ==0){
			}
			else if (filterit ==-1){
				value1 -= input->color[plane][row1][col1];
				value2 -= input->color[plane][row2][col2];
				value3 -= input->color[plane][row3][col3];
				value4 -= input->color[plane][row1][col1];
				value5 -= input->color[plane][row2][col2];
				value6 -= input->color[plane][row3][col3];
				value7 -= input->color[plane][row1][col1];
				value8 -= input->color[plane][row2][col2];
				value9 -= input->color[plane][row3][col3];
			} 		
			else{
				value1 += input->color[plane][row1][col1]*filterit;
				value2 += input->color[plane][row2][col2]*filterit;
				value3 += input->color[plane][row3][col3]*filterit;
				value4 += input->color[plane][row1][col1]*filterit;
				value5 += input->color[plane][row2][col2]*filterit;
				value6 += input->color[plane][row3][col3]*filterit;
				value7 += input->color[plane][row1][col1]*filterit;
				value8 += input->color[plane][row2][col2]*filterit;
				value9 += input->color[plane][row3][col3]*filterit;
			}	
		  }
		}
		if (divisor !=1){ // set if so only goes in if div is needed
			value1 = value1 / divisor;
			value2 = value2 / divisor;
			value3 = value3 / divisor;
			value4 = value4 / divisor;
			value5 = value5 / divisor;
			value6 = value6 / divisor;
			value7 = value7 / divisor;
			value8 = value8 / divisor;
			value9 = value9 / divisor;
		}
		// make if statements so it only checked if out of bounds <0 or >255
		if( value1 <0 || value1 >255){	
			if ( value1  < 0 ) { value1 = 0; }
			if ( value1  > 255 ) { value1 = 255; }
		}	
		if( value2 <0 || value2 >255){	
			if ( value2  < 0 ) { value2 = 0; }
			if ( value2  > 255 ) { value2 = 255; }
		}
		if( value3 <0 || value3 >255){	
			if ( value3  < 0 ) { value3 = 0; }
			if ( value3  > 255 ) { value3 = 255; }
		}
		if( value4 <0 || value4 >255){	
			if ( value4  < 0 ) { value4 = 0; }
			if ( value4  > 255 ) { value4 = 255; }
		}
		if( value5 <0 || value5 >255){	
			if ( value5  < 0 ) { value5 = 0; }
			if ( value5  > 255 ) { value5 = 255; }
		}
		if( value6 <0 || value6 >255){	
			if ( value6  < 0 ) { value6 = 0; }
			if ( value6  > 255 ) { value6 = 255; }
		}
		if( value7 <0 || value7 >255){	
			if ( value7  < 0 ) { value7 = 0; }
			if ( value7  > 255 ) { value7 = 255; }
		}
		if( value8 <0 || value8 >255){	
			if ( value8  < 0 ) { value8 = 0; }
			if ( value8  > 255 ) { value8 = 255; }
		}
		if( value9 <0 || value9 >255){	
			if ( value9  < 0 ) { value9 = 0; }
			if ( value9  > 255 ) { value9 = 255; }
		}
		
			
		output -> color[plane][row][col] = value1;
		output -> color[plane][row+1][col] = value2;
		output -> color[plane][row+2][col] = value3;
		output -> color[plane][row][col+1] = value4;
		output -> color[plane][row+1][col+1] = value5;
		output -> color[plane][row+2][col+1] = value6;
		output -> color[plane][row][col+2] = value7;
		output -> color[plane][row+1][col+2] = value8;
		output -> color[plane][row+2][col+2] = value9;
		  }
		}
	  }
	  
  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
