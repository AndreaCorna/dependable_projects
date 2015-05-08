#include<iostream>
#include<cmath>
#include<cstdlib>
#include<cstring>
#include<iomanip>

#define ACTIVATIONENERGY 0.48
#define BOLTZMANCONSTANT 8.6173324*0.00001
#define CONST_JMJCRIT 1500000
#define CONST_N 1.1
#define GAMMA_2 0.88623
#define CONST_A0 30000 //cross section = 1um^2  material constant = 3*10^12
#define BETA 2

double getAlpha(double temp) {
  return ((CONST_A0 * (pow(CONST_JMJCRIT,(-CONST_N))) * exp(ACTIVATIONENERGY / (BOLTZMANCONSTANT * temp))) / GAMMA_2);
}

int main(int argc, char* argv[]){
  double T, a;
  
  if(argc != 2){
    std::cout << "USAGE: " << argv[0] << " [TEMP]" << std::endl;
    std::cout << "The temperature has to be specified in celsius degrees" << std::endl;
    return 1;
  }

	std::cout << std::setprecision(10);
  
  T = atof(argv[1]);
  a = ((CONST_A0 * (pow(CONST_JMJCRIT,(-CONST_N))) * exp(ACTIVATIONENERGY / (BOLTZMANCONSTANT * T ))) / GAMMA_2);
  
  std::cout << "alpha: " << a << std::endl;  
    
  return 0;
}

