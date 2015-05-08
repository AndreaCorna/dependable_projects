****************************** Copyright & License *****************************
CALIPER v1.1 is a software framework for the lifetime reliability evaluation of multicore architectures. Copyright (C) 2014 Politecnico di Milano.

This framework is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This framework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details (http://www.gnu.org/licenses/).

Neither the name of Politecnico di Milano nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
********************************************************************************

** Introduction:
CALIPER v1.1 is a software framework for the lifetime reliability evaluation of N-out-of-M multicore architectures. A theoretical description of the implemented lifetime reliability model can be found in:
C. Bolchini, M. Carminati, M. Gribaudo, A. MIELE: A lightweight and open-source framework for the lifetime estimation of multicore systems. In Proc. of IEEE International Conference on Computer Design (ICCD), Seoul, South Korea, 2014, pp. 166-172. 

If you use CALIPER in your research, we would appreciate a citation to:

@inproceedings{ICCD2014,
 author = {C. Bolchini and M. Carminati and M. Gribaudo and A. Miele},
 title = {A lightweight and open-source framework for the lifetime estimation of multicore systems},
 booktitle = {Proc. IEEE Intl. Conf. Computer Design - ICCD},
 year = {2014},
 month = {Oct.},
 pages = {166--172}
}


** This folder contains:
_ this "README.txt" file
_ a "Makefile" for the project
_ a "src" folder where to find the source code
_ an example of input configuration file of a 2oo3 system ("example2oo3.txt")


** How to compile and run CALIPER (on Linux/Mac):
_ just type "make" from the root of this folder
_ run the caliper executable which has been placed in the root folder
_ the following list of parameters is available (parameters in square brackets are to be considered as optional ones):
    -f INPUT_FILENAME: file containing the aging rate for any possible configuration
    [-m MIN_NUM_OF_CORES] : minimum number of cores for the QoS to be considered met; if not specified, it is derived from the aging rate file
    [-s RANDOM|num:num:num] : seed specification; three integer numbers separated by : need to be specified
    [-t NUMBER_OF_TESTS | -c CONFIDENCE_INTERVAL -r STOPPING_THRESHOLD] : Monte Carlo simulations to be performed: specify the exact number or the confidence interval and the stopping threshold
    [-p REMAPPING_PERIOD_IN_H] : duration (in hours) of the remapping period, in case multiple aging rates for each configuration are listed in the input file
    [-o R_OUTPUT_FILENAME] : output file where the system reliability function will be tabulated
    [-h] : print the help and specify the default values for the parameters


** Aging Rate Computation:
When compiling the code, an executable to compute a single alpha rate is created as well (./computeAlpha). The executable takes as argument a temperature in Celsius degrees and computes the related aging rate; technological parametrs are specified as constants in the source code file.

** Aging Rate File Format:
This file contains the aging rate of each alive core in the architecture is a specific scenario.
Each line in the aging rate input file must have the following format:
    CONFIGURATION AGING_RATES
where:
> CONFIGURATION: is a comma-separated string, listing the ids of failed cores in that configuration, sorted by time (with the oldest failure first). The initial configuration, where there is no failure, is indicated by means of the character ‘#’ (hash). A positizonal notation based on unique unsigned values is used for the cores (i.e. the is named 0, the secod one 1, ...).
> AGING_RATES: is a blank space-separate string, listing the aging rate for the Reliability formula (based on the Weibull distribution) for each core in the architecture (first value is referred to core 0, second value to core 1, ...); the Beta parameter in the Weibull formula is specified with a macro in the source code of CALIPER. A dummy value must be inserted also for failed core, even if it will be ignored by CALIPER.

The configuration file must contain all the possible failure combinations until a minimum number of core, which can be specified by the user or derived from the file itself. Moreover, in order to specify periodic remapping strategies, more than one entry for the same CONFIGURATION need to be specified on separate lines. example2oo3.txt is an example of configuration file of a 2oo3 system

** Sample Execution:
_ generate the aging rates manually by using computeAlpha program and save all values in the configuration file (in the folder an example of file is provided: example2oo3.txt)
_ run CALIPER:
    ./caliper -f example2oo3.txt -s RANDOM -t 10000 -o 2x3rel.txt
Here, the above generated "example2oo3.txt" configuration file is used; a RANDOM seed is selected; 10000 Monte Carlo simulations are run ; the reliability curve is saved in the file "2x3rel.txt", while other statistics are printed on the standard output.

 
 ** To signal bugs or for questions, please write to: antonio <dot> miele <at> polimi <dot> it
