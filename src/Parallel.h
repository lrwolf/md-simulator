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

class Parallel : public Simulator {
public:
    Parallel();
    
    virtual int setup();
    virtual int run();
    virtual int cleanup();
    
    cl_context context;                 /**< CL context */
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernel;                   /**< CL kernel */

    size_t kernelWorkGroupSize;         /**< Group Size returned by kernel */
    size_t blockSizeX;                  /**< Work-group size in x-direction */
    size_t blockSizeY;                  /**< Work-group size in y-direction */
    
    cl_mem inputBuffer;
    cl_mem outputBuffer;
};

#endif /* defined(__sim__Parallel__) */
