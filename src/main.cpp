//
//  main.c
//  sim
//
//  Created by Lucas Wolf on 11/29/13.
//
//

#include "MDSim.h"
#include <memory>
#include <stdio.h>

int main(int argc, const char * argv[]) {
    std::unique_ptr<MDSim> sim(new MDSim);
    
    if (sim->setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if (sim->runCLKernels() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if (sim->cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}

