//
//  main.c
//  sim
//
//  Created by Lucas Wolf on 11/29/13.
//
//

#include <memory>
#include <stdio.h>
#include "Parallel.h"
#include "Sequential.h"

int main(int argc, const char * argv[]) {
    std::unique_ptr<Sequential> sim(new Sequential);
    
    if (sim->setup() != MD_SUCCESS)
        return MD_FAILURE;
    
    if (sim->run() != MD_SUCCESS)
        return MD_FAILURE;

    if (sim->cleanup() != MD_SUCCESS)
        return MD_FAILURE;
    
    return MD_SUCCESS;
}
