//
//  MDSim.h
//  sim
//
//  Created by Lucas Wolf on 3/30/14.
//
//

#ifndef __sim__MDSim__
#define __sim__MDSim__

#include "Molecule.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <OpenCL/OpenCL.h>
#include <vector>

#define SDK_SUCCESS 0
#define SDK_FAILURE -1

class MDSim {
public:
    int cubeSide;
    int particleCount;
    
    double latticeOffset;
    double timestep;
    
    double wallStiffness;
    double dimensions[3] = { 50.0, 50.0, 50.0 };
    double forceCutoff;
    
    std::vector<std::unique_ptr<Molecule>> molecules;
    
    double* data;
    double* results;
    
    cl_mem inputBuffer;
    cl_mem outputBuffer;
    
    cl_context context;                 /**< CL context */
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernel;                   /**< CL kernel */
    
    size_t kernelWorkGroupSize;         /**< Group Size returned by kernel */
    size_t blockSizeX;                  /**< Work-group size in x-direction */
    size_t blockSizeY;                  /**< Work-group size in y-direction */
    
    MDSim();
    
    int setupCL();
    int runCLKernels();
    int cleanup();
    
    void computeAccelerations();
};

#endif /* defined(__sim__MDSim__) */
