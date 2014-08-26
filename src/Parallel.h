//
//  Parallel.h
//  sim
//
//  Created by Lucas Wolf on 7/16/14.
//
//

#ifndef __sim__Parallel__
#define __sim__Parallel__

#include <OpenCL/OpenCL.h>
#include "Simulator.h"

#define NUM_CONSTANTS 5

class Parallel : public Simulator {
public:
    Parallel();
    
    virtual int setup();
    virtual int computeAccelerations();
    virtual int cleanup();
    
    std::unique_ptr<float[]> data;
    std::unique_ptr<float[]> constants;
    std::unique_ptr<float[]> results;
    
    cl_context context;                 /**< CL context */
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernel;                   /**< CL kernel */

    size_t kernelWorkGroupSize;         /**< Group Size returned by kernel */
    size_t global;
    size_t local;
   
    cl_mem inputBuffer;
    cl_mem constantsBuffer;
    cl_mem outputBuffer;
};

#endif /* defined(__sim__Parallel__) */
