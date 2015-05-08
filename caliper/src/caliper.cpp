/*
****************************** Copyright & License *****************************
CALIPER v1.0 is a software framework for the reliability lifetime evaluation of Multicore architectures. Copyright (C) 2014 Politecnico di Milano.

This framework is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This framework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details (http://www.gnu.org/licenses/).

Neither the name of Politecnico di Milano nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
********************************************************************************

 This code implements the R/MTTF computation for a K-out-of-N system by means of Monte Carlo simulations, for the CALIPER framework
 */

#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <sstream>
#include <getopt.h>
#include <set>
#include <unistd.h>
#include "utils.h"

#define NTESTS 100000
#define BETA 2
#define MIN_NUM_OF_TRIALS 30

#define RANDOMSEED_STR "RANDOM"

// Support function to allow arbitrary confidence intervals
#define INV_ERF_ACCURACY 10e-6

double invErf(double f) {
// inverts the gaussian distribution N(0,1) using the bisection method
    double l = 0;
    double r = 1;
    double vl, vr, h, vh;

    // first iteration: find a v, such that N(v) > f
    while ((erf(r / M_SQRT2) + 1.0) / 2.0 < f) {
        r *= 2;
    }

    // solves the equation iteratively
    vl = (erf(l / M_SQRT2) + 1.0) / 2.0 - f;
    vr = (erf(r / M_SQRT2) + 1.0) / 2.0 - f;
    h = (l + r) / 2;
    while (fabs(r - l) / h > INV_ERF_ACCURACY) {
        vh = (erf(h / M_SQRT2) + 1.0) / 2.0 - f;
        if (vh * vl < 0.0) {
            r = h;
            vr = vh;
        } else {
            l = h;
            vl = vh;
        }
        h = (l + r) / 2;
    }

    return h;
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::vector<aging_config_t> > configurations;
    int left_cores, min_cores = 0, tmp_min_cores, max_cores;
    long num_of_tests = NTESTS;
    std::map<double, double> results;
    int i;
    time_t t_setup, t_start, t_end;

    char* filename = NULL;
    char* outputfilename = NULL;
    bool minNodeSpecified = false;
    bool numTest = false;

    unsigned short randomSeed[3] = { 0, 0, 0 };
    int period = 0;
    double confInt = 0, thr = 0;

    ////////////////////////////////////////////////////////////////////////////////
    //parsing input arguments
    ////////////////////////////////////////////////////////////////////////////////
    int next_option;
    //a string listing valid short options letters
    const char* const short_options = "hf:m:s:t:p:c:r:o:";
    //an array describing valid long options
    const struct option long_options[] = { { "help", no_argument, NULL, 'h' }, //help
            { "file", required_argument, NULL, 'f' }, //input file name
            { "min_cores", required_argument, NULL, 'm' }, //minimum number of cores
            { "seed", required_argument, NULL, 's' }, //seed
            { "n_tests", required_argument, NULL, 't' }, //number of tests
            { "period", required_argument, NULL, 'p' }, //period
            { "confInt", required_argument, NULL, 'c' }, //confidence interval
            { "thr", required_argument, NULL, 'r' }, //stopping threshold
            { "output", required_argument, NULL, 'o' }, //stopping threshold
            { NULL, 0, NULL, 0 } /* Required at end of array.  */
    };

    do {
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch (next_option) {
            case 'f':
                filename = optarg;
                break;
            case 'o':
                outputfilename = optarg;
                break;
            case 'm':
                minNodeSpecified = true;
                min_cores = atoi(optarg);
                if (min_cores < 0) {
                    std::cerr << "Negative minimum number of cores: " << min_cores << std::endl;
                    exit(1);
                }
                break;
            case 's':
                if (strcmp(optarg, RANDOMSEED_STR) == 0) {
                    randomSeed[0] = (unsigned short) time(NULL);
                    randomSeed[1] = (unsigned short) time(NULL);
                    randomSeed[2] = (unsigned short) time(NULL);
                } else {
                    std::vector<std::string> tokens;
                    stringTokenize(optarg, tokens, ":");
                    if (tokens.size() != 3) {
                        std::cerr << "Not valid seed: " << optarg << std::endl;
                        exit(1);
                    }
                    randomSeed[0] = (unsigned short) atoi(tokens[0].c_str());
                    randomSeed[1] = (unsigned short) atoi(tokens[1].c_str());
                    randomSeed[2] = (unsigned short) atoi(tokens[2].c_str());
                }
                break;
            case 't':
                numTest = true;
                num_of_tests = atol(optarg);
                if (num_of_tests <= 0) {
                    std::cerr << "Non-positive number of tests: " << num_of_tests << std::endl;
                    exit(1);
                }
                break;
            case 'p':
                period = atoi(optarg);
                if (period < 0) {
                    std::cerr << "Negative period: " << period << std::endl;
                    exit(1);
                }
                break;
            case 'c':
                confInt = atof(optarg);
                if (confInt <= 0) {
                    std::cerr << "Non-positive confidence interval: " << confInt << std::endl;
                    exit(1);
                }
                break;
            case 'r':
                thr = atof(optarg);
                if (thr <= 0) {
                    std::cerr << "Non-positive stopping threshold: " << thr << std::endl;
                    exit(1);
                }
                break;
            case -1: /* Done with options.  */
                break;
            case '?':
            case 'h':
            default: /* Something else: unexpected.  */
                std::cerr << std::endl << "USAGE: " << argv[0]
                        << " -f INPUT_FILENAME [-m MIN_NUM_OF_CORES] [-s RANDOM|num:num:num] [-t NUMBER_OF_TESTS | -c CONFIDENCE_INTERVAL -r STOPPING_THRESHOLD] "
                        << "[-p REMAPPING_PERIOD_IN_H] [-o R_OUTPUT_FILENAME]" << std::endl;
                std::cerr << "DEFAULTS: " << " -m NUMBER_OF_CORES-1 -s " << randomSeed[0] << ":" << randomSeed[1] << ":" << randomSeed[2] << " -t " << NTESTS
                        << " -p " << period << " (0=NO_REMAPPING)" << std::endl;
                return 0;
        }
    } while (next_option != -1);

    if (!filename) {
        std::cerr << std::endl << "USAGE: " << argv[0]
                << " -f INPUT_FILENAME [-m MIN_NUM_OF_CORES] [-s RANDOM|num:num:num]  [-t NUMBER_OF_TESTS | -c CONFIDENCE_INTERVAL -r STOPPING_THRESHOLD] "
                << "[-p REMAPPING_PERIOD_IN_H] [-o R_OUTPUT_FILENAME]" << std::endl;
        std::cerr << "DEFAULTS: " << " -m NUMBER_OF_CORES-1 -s " << randomSeed[0] << ":" << randomSeed[1] << ":" << randomSeed[2] << " -t " << NTESTS << " -p "
                << period << " (0=NO_REMAPPING)" << std::endl;
        return 0;
    }

    //check both stopping threshold and confidence interval are set if numTest is false
    if (!numTest && (confInt == 0 || thr == 0)) {
        if (!(confInt == 0 && thr == 0)) {
            std::cerr << "Confidence Interval / Stopping threshold missing!" << std::endl;
            exit(1);
        } else {
            numTest = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //set up environment
    ////////////////////////////////////////////////////////////////////////////////

    t_start = time(NULL);
    //set seed
    seed48(randomSeed);
    //set output precision
    std::cout << std::setprecision(10);
    std::cerr << std::setprecision(10);
    //load input file
    loadConfigurations(filename, configurations, max_cores, tmp_min_cores);
    //check if we have specified all the possible configurations
    std::set<std::string> tree;

    //if not specified in the arguments, set the minimum number of cores read from the file
    if (!minNodeSpecified)
        min_cores = tmp_min_cores;
    else if (min_cores > max_cores) {
        std::cerr << "The minimum number of cores " << min_cores << " cannot be greater than the initial number of cores " << max_cores << std::endl;
        return 1;
    }

//    generateTree(max_cores, min_cores, tree);
//    for (std::set<std::string>::iterator setIt = tree.begin(); setIt != tree.end(); setIt++) {
//        if (configurations.find(*setIt) == configurations.end()) {
//            std::cerr << "Not specified configuration: " << *setIt << std::endl;
//            exit(1);
//        }
//    }
    
    // check coherence of the configuration data structure (in the scenario of unbalanced tree)
    if (configurations.find(EMPTY_SET) == configurations.end()) {
        std::cerr << "Not specified configuration: " << EMPTY_SET << std::endl;
        exit(1);
    }
    for(std::map<std::string, std::vector<aging_config_t> >::iterator iter = configurations.begin(); iter != configurations.end(); iter++){
        unsigned pos = (iter->first).find_last_of(",");
        if(pos != std::string::npos) {
            std::string parent = (iter->first).substr(0,pos);
            if (configurations.find(parent) == configurations.end()) {
                std::cerr << "Not specified configuration: " << parent << std::endl;
                exit(1);
            }
        } 
    }
    t_setup = time(NULL);

    // confidence interval set up
    double Zinv = invErf(0.5 + confInt / 100.0 / 2);
    double ht = thr / 100.0 / 2; // half of the threshold
    double sumTTF = 0, sumTTFX2 = 0; //sum of times to failure and sum of squared times to failure
    double ciSize = 0; // current size of the confidence interval
    double mean;   // current mean of the distribution
    double var;	   // current variance of the distribution
    std::map<int, double> sumTTnF;

    ////////////////////////////////////////////////////////////////////////////////
    //run Monte Carlo simulation
    ////////////////////////////////////////////////////////////////////////////////
    //for (i = 0, sumTTF = 0; i < num_of_tests; i++) {
    //when using the confidence interval, we want to execute at least MIN_NUM_OF_TRIALS
    for (i = 0; (numTest && (i < num_of_tests)) || (!numTest && ((i < MIN_NUM_OF_TRIALS) || (ciSize / mean > ht))); i++) {
        //std::cerr << i << std::endl;
        double random;
        std::vector<double> currR;
        double stepT;
        std::string currConf;
        int minIndex;
        double totalTime;
        std::vector<bool> alives;
        int j;
        double t, eqT;

        // experiment initialization
        left_cores = max_cores;
        totalTime = 0;
        stepT = 0;
        minIndex = 0;
        currConf = EMPTY_SET;
        currR.clear();
        alives.clear();
        for (j = 0; j < max_cores; j++) {
            currR.push_back(1.0);
            alives.push_back(true);
        }

        //run current experiment
        //std::cout << std::endl << "NEW EXP ";
        while (left_cores >= min_cores && configurations.find(currConf) != configurations.end()) {
            // generate failure times for alive cores and find the shortest one
            minIndex = -1;
            //std::cout << currConf << " ";
            for (j = 0; j < max_cores; j++) {
                if (alives[j] == true) {
                    random = (double) drand48() * currR[j]; //current core will potentially die when its R will be equal to random. drand48() generates a number in the interval [0.0;1.0)
                    if (period == 0) { //NON PERIODIC SCENARIO
                        int currMapping;
                        double elapsedT;
                        double testR;
                        double cR = currR[j];
                        double currSparePeriod;
                        double accTime;

                        //scan the list of mapping in order to identify the current one (it is performed by considering the periods' durations)
                        //DO NOTE: last mapping is not considered in the iterations since it will be constant for the remaining lifetime
                        //therefore R^-1 and R formulas will be applied by using "random" as parameter
                        for (currMapping = 0, accTime = configurations[currConf][0].duration;
                                currMapping < configurations[currConf].size() - 1 && accTime <= totalTime; currMapping++, accTime +=
                                        configurations[currConf][currMapping].duration) {
                            //do nothing :)
                        }
                        //compute the time left on the current mapping. it will be used in the below loop (thus not considered for the last mapping in the list)
                        currSparePeriod = 0;
                        if (currMapping < configurations[currConf].size() - 1)
                            currSparePeriod = accTime - totalTime;

                        //std::cerr << "XXX " << currSparePeriod << std::endl;

                        //reset current time
                        t = 0;

                        //it is necessary to compute first the equivalent time necessary to obtain the previous R with the current alpha
                        elapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA);
                        //and then compute the new R
                        testR = exp(-pow((elapsedT + currSparePeriod) / configurations[currConf][currMapping].alphas[j], BETA));

                        bool firstIter = true;

                        //std::cerr << "XXX " << testR << std::endl;
                        while (testR > random && currMapping < configurations[currConf].size() - 1) {
                            //advance step by step a period per time if testR > random and update cR and iterations
                            //store "real" elapsed time since the previous failure
                            if (firstIter) {
                                t = currSparePeriod;
                                firstIter = false;
                            } else
                                t += configurations[currConf][currMapping].duration;
                            cR = testR;

                            //move to the next mapping
                            currMapping++;

                            //it is necessary to compute first the equivalent time necessary to obtain the previous R with the current alpha
                            elapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA);
                            //and then compute the new R
                            testR = exp(
                                    -pow((elapsedT + configurations[currConf][currMapping].duration) / configurations[currConf][currMapping].alphas[j], BETA));
                        }

                        //std::cerr << "XXX " << t << std::endl;

                        //It is impossible that R(duration1+duration2+...+durationN) is exactly equal to the randomly generated R value.
                        //in previous iteration we have computed the largest N such that we obtain the closest R(duration1+duration2+...+durationN) value
                        //greater than the randomly generated R value
                        //Now we have to compute the last deltaT (that is less than the given period) to obtain the randomly generated R value
                        //DO NOTE: this block of code manages also the last mapping in the list since no other remapping will be performed.
                        //DO NOTE: therefore this block of code manages the scenario in which we have a single mapping in the list
                        if (cR > random) {
                            double lastElapsedT;
                            lastElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(random), (double) 1 / BETA); //elapsed time from 0 to obtain the new R value equal to random
                            eqT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA); //elapsed time from 0 to obtain the previous R value
                            //the difference between the two values represents the time period elapsed from the previous failure to the current failure
                            //(we will sum to the total time the minimum of such values)
                            t += (lastElapsedT - eqT);
                        }
                    } else { //PERIODIC SCENARIO - TODO: at the present moment the period is specified as argument and the values annotated on the tree are ignored!
                        int currMapping;
                        double elapsedT;
                        double testR;
                        double cR = currR[j];

                        //computation of current mapping according to the period and the current totalTime
                        //identify current period and related mapping
                        double currPeriod = totalTime / period;
                        currMapping = ((int) currPeriod) % configurations[currConf].size();
                        //compute the spare time to complete current period
                        double currSparePeriod = (((int) currPeriod) + 1 - currPeriod) * period;  //TODO: fixed a bug
                        //DO NOTE: this block of code is equivalent to the above instruction
                        //double currSparePeriod = (ceil(currPeriod) - currPeriod) * period;
                        ////do consider that in each period the lower bound is included and the upper bound is not included [lowB;highB)
                        //if (currSparePeriod == 0) {
                        //    currSparePeriod = period;
                        //}

                        //reset current time
                        t = 0;

                        //it is necessary to compute first the equivalent time to obtain the previous R with the current alpha
                        elapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA);
                        //and then compute the new R
                        testR = exp(-pow((elapsedT + currSparePeriod) / configurations[currConf][currMapping].alphas[j], BETA));

                        bool firstIter = true;
                        while (testR > random) {
                            //store "real" elapsed time since the previous failure
                            if (firstIter) {
                                t = currSparePeriod;
                                firstIter = false;
                            } else
                                t += period;
                            cR = testR;

                            //advance step by step a period per time if testR > random and update cR and interations
                            currMapping = (currMapping + 1) % configurations[currConf].size();
                            //it is necessary to compute first the equivalent time to obtain the previous R with the current alpha
                            elapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA);
                            //and then compute the new R
                            testR = exp(-pow((elapsedT + period) / configurations[currConf][currMapping].alphas[j], BETA));
                        }

                        //it is impossible that R(N*period) is exactly equal to the randomly generated R value.
                        //in previous iteration we have computed the largest N such that we obtain the closest R(N*period) value greater than the randomly generated R value
                        //Now we have to compute the last deltaT (that is less than the given period) to obtain the randomly generated R value
                        if (cR > random) {
                            double lastElapsedT;
                            lastElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(random), (double) 1 / BETA); //elapsed time from 0 to obtain the new R value equal to random
                            eqT = configurations[currConf][currMapping].alphas[j] * pow(-log(cR), (double) 1 / BETA); //elapsed time from 0 to obtain the previous R value
                            //the difference between the two values represents the time period elapsed from the previous failure to the current failure
                            //(we will sum to the total time the minimum of such values)
                            t += (lastElapsedT - eqT);
                        }
                    }
                    //std::cerr << j << " R " << random << " " << t << std::endl;
                    if (minIndex == -1 || (minIndex != -1 && t < stepT)) {
                        minIndex = j;
                        stepT = t;
                    } //TODO ADD A CHECK ON MULTIPLE FAILURE IN THE SAME INSTANT OF TIME.
                }
            }
            if (minIndex == -1) {
                std::cerr << "Failing cores not found" << std::endl;
                return 1;
            }

            //std::cerr << "min " << minIndex << " " << stepT << std::endl;

            // update total time by using equivalent time according to the R for the core that is dying
            //stepT is the time starting from 0 to obtain the R value when the core is dead with the current load
            //eqT is the time starting from 0 to obtain the previous R value with the current load
            //thus the absolute totalTime when the core is dead is equal to the previous totalTime + the difference between stepT and eqT
            //geometrically we translate the R given the current load to right in order to intersect the previous R curve in the previous totalTime
            totalTime = totalTime + stepT;

            //if (period == 0) { //TODO code replaced by above instruction
            //    eqT = configurations[currConf][0][minIndex] * pow(-log(currR[minIndex]), (double) 1 / BETA);
            //    totalTime = totalTime + stepT; // - eqT;
            //} else {
            //    totalTime = totalTime + stepT;
            //}

            //std::cerr << "tt " << totalTime << " " << stepT << " " << eqT << std::endl;

            // update configuration
            alives[minIndex] = false;
            // compute remaining reliability for working cores
            for (j = 0; j < max_cores; j++) {
                if (alives[j]) {
                    if (period == 0) { //NON PERIODIC SCENARIO
                        //advance step by step for the overall duration of the interval from 0 to current time
                        int currMapping;
                        double elapsedT, tmpElapsedT;
                        double equivalentElapsedT;
                        double tmpR;

                        double accTime;
                        double currSparePeriod;

                        //scan the list of mapping in order to identify the current one (it is performed by considering the periods' durations)
                        //DO NOTE: last mapping is not considered in the iterations since it will be constant for the remaining lifetime
                        //therefore R^-1 and R formulas will be applied by using "random" as parameter
                        for (currMapping = 0, accTime = configurations[currConf][0].duration;
                                currMapping < configurations[currConf].size() - 1 && accTime <= totalTime - stepT; currMapping++, accTime +=
                                        configurations[currConf][currMapping].duration) {
                            //do nothing :)
                        }
                        //std::cerr << "currMapping " << currMapping << std::endl;
                        //std::cerr << "accTime " << accTime << std::endl;
                        //compute the time left on the current mapping. it will be used in the below loop (thus not considered for the last mapping in the list)
                        currSparePeriod = 0;
                        if (currMapping < configurations[currConf].size() - 1)
                            currSparePeriod = accTime - (totalTime - stepT);
                        //std::cerr << "currSparePeriod " << currSparePeriod << std::endl;

                        elapsedT = 0;

                        tmpElapsedT = currSparePeriod; //time elapsed from the previous time step
                        //It is necessary to compute first the equivalent time to obtain the previous R with the current alpha...
                        equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                        //and then compute the new R
                        tmpR = exp(-pow((equivalentElapsedT + currSparePeriod) / configurations[currConf][currMapping].alphas[j], BETA));

                        while (tmpElapsedT < stepT && currMapping < configurations[currConf].size() - 1) {
                            //update actual values with tested ones since the condition is satisfied
                            currR[j] = tmpR;
                            elapsedT = tmpElapsedT;

                            //compute values for the next period
                            tmpElapsedT += period;
                            currMapping = currMapping + 1;
                            //It is necessary to compute first the equivalent time to obtain the previous R with the current alpha ..
                            equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                            //and then compute the new R
                            tmpR = exp(-pow((equivalentElapsedT + period) / configurations[currConf][currMapping].alphas[j], BETA));
                        }

                        //add the last time delta
                        double lastDeltaT = stepT - elapsedT;
                        equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                        tmpR = exp(-pow((equivalentElapsedT + lastDeltaT) / configurations[currConf][currMapping].alphas[j], BETA));
                        if (tmpR > currR[j]) {
                            std::cerr << "Non-monotonic R function: " << tmpR << " > " << currR[j];
                            return 1;
                        }
                        currR[j] = tmpR;
                    } else { //PERIODIC SCENARIO - TODO: at the present moment the period is specified as argument and the values annotated on the tree are ignored!
                        //advance step by step for the overall duration of the interval from 0 to current time
                        int currMapping;
                        double elapsedT, tmpElapsedT;
                        double equivalentElapsedT;
                        double tmpR;

                        //computation of current mapping according to the period and the current totalTime
                        //identify current period and related mapping
                        double currPeriod = (totalTime - stepT) / period; // we need to restart from previous step...
                        currMapping = ((int) currPeriod) % configurations[currConf].size();

                        //std::cerr << "currMapping " << currMapping << std::endl;

                        //compute the spare time to complete current period
                        double currSparePeriod = (((int) currPeriod) + 1 - currPeriod) * period;  //TODO: fixed a bug

                        //std::cerr << "currSparePeriod " << currSparePeriod << std::endl;

                        //DO NOTE: this block of code is equivalent to the above instruction
                        //double currSparePeriod = (ceil(currPeriod) - currPeriod) * period;
                        ////do consider that in each period the lower bound is included and the upper bound is not included [lowB;highB)
                        //if (currSparePeriod == 0) {
                        //    currSparePeriod = period;
                        //}

                        elapsedT = 0;

                        tmpElapsedT = currSparePeriod; //time elapsed from the previous time step
                        //It is necessary to compute first the equivalent time to obtain the previous R with the current alpha...
                        equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                        //and then compute the new R
                        tmpR = exp(-pow((equivalentElapsedT + currSparePeriod) / configurations[currConf][currMapping].alphas[j], BETA));

                        while (tmpElapsedT < stepT) {
                            //update actual values with tested ones since the condition is satisfied 
                            if (tmpR > currR[j]) {
                                std::cerr << "Non-monotonic R function: " << tmpR << " > " << currR[j] << std::endl;
                                return 1;
                            }
                            currR[j] = tmpR;
                            elapsedT = tmpElapsedT;

                            //compute values for the next period
                            tmpElapsedT += period;
                            currMapping = (currMapping + 1) % configurations[currConf].size();
                            //It is necessary to compute first the equivalent time to obtain the previous R with the current alpha ..
                            equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                            //and then compute the new R
                            tmpR = exp(-pow((equivalentElapsedT + period) / configurations[currConf][currMapping].alphas[j], BETA));
                        }

                        //add the last time delta
                        double lastDeltaT = stepT - elapsedT;
                        equivalentElapsedT = configurations[currConf][currMapping].alphas[j] * pow(-log(currR[j]), (double) 1 / BETA);
                        tmpR = exp(-pow((equivalentElapsedT + lastDeltaT) / configurations[currConf][currMapping].alphas[j], BETA));
                        if (tmpR > currR[j]) {
                            std::cerr << "Non-monotonic R function: " << tmpR << " > " << currR[j];
                            return 1;
                        }
                        currR[j] = tmpR;
                    }
                    //std::cerr << j << " " << currR[j] << std::endl;
                }
            }
            //compute new configuration
            if (currConf == EMPTY_SET)
                currConf = MAKE_STRING(minIndex);
            else
                currConf = MAKE_STRING(currConf << "," << minIndex);

            left_cores--;
        }
        // updates stats
        results[totalTime]++;
        sumTTF += totalTime;
        sumTTFX2 += totalTime * totalTime;
        mean = sumTTF / (double) (i + 1); //do consider that i is incremented later
        var = sumTTFX2 / (double) (i) - mean * mean;
        ciSize = Zinv * sqrt(var / (double) (i + 1));
    }

    ////////////////////////////////////////////////////////////////////////////////
    //display results
    ////////////////////////////////////////////////////////////////////////////////
    if (!numTest)
        num_of_tests = i;
    double curr_alives = num_of_tests;
    double prec_time = 0;
    double mttf_int = (sumTTF / num_of_tests);
    double mttf_int1 = 0;
    if (outputfilename) {
        std::ofstream outfile(outputfilename);
        if (results.count(0) == 0) {
            results[0] = 0;
        }

        for (std::map<double, double>::iterator mapIt = results.begin(); mapIt != results.end(); mapIt++) {
            curr_alives = curr_alives - mapIt->second;
            mttf_int1 = mttf_int1 + curr_alives / num_of_tests * (mapIt->first - prec_time);
            prec_time = mapIt->first;
            outfile << mapIt->first << " " << (curr_alives / num_of_tests) << std::endl;
        }
        outfile.close();
    }

    t_end = time(NULL);

    std::cout << "MTTF: " << mttf_int << " (years: " << (mttf_int / (24 * 365)) << ") " << mttf_int1 << std::endl;
    std::cout << "Exec time: " << ((double) t_setup - t_start) << " " << ((double) t_end - t_setup) << std::endl;
    std::cout << "Number of tests performed: " << num_of_tests << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Variance: " << var << std::endl;
    std::cout << "Standard Deviation: " << sqrt(var) << std::endl;
    std::cout << "Coefficient of variation: " << (sqrt(var) / mean) << std::endl;
    std::cout << "Confidence interval: " << mean - ciSize << " " << mean + ciSize << std::endl;

    for (std::map<int, double>::iterator mapIt = sumTTnF.begin(); mapIt != sumTTnF.end(); mapIt++) {
        std::cout << "MTT" << mapIt->first << "F: " << (mapIt->second / num_of_tests) << " (years: " << (mapIt->second / num_of_tests / (24 * 365)) << ")"
                << std::endl;
    }

    return 0;
}
