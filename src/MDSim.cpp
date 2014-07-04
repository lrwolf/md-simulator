//
//  MDSim.cpp
//  sim
//
//  Created by Lucas Wolf on 3/30/14.
//
//

#include "MDSim.h"

#include "Molecule.h"
#include "Util.h"

#include <ctime>
#include <cmath>
#include <functional>
#include <memory>
#include <random>

/*
 Debug-related macro/functions.
 */

#define CheckConditional(conditional, message) { if (!(conditional)) { std::cout << "ERROR: " << message << std::endl; return -1; } }
#define CheckStatus(status, message) { if (status != CL_SUCCESS) { std::cout << "ERROR: " << print_cl_errstring(status) << ", " << message << std::endl; return -1; } }
char *print_cl_errstring(cl_int err) { switch (err) { case CL_SUCCESS: return strdup("Success!"); case CL_DEVICE_NOT_FOUND: return strdup("Device not found."); case CL_DEVICE_NOT_AVAILABLE: return strdup("Device not available"); case CL_COMPILER_NOT_AVAILABLE: return strdup("Compiler not available"); case CL_MEM_OBJECT_ALLOCATION_FAILURE: return strdup("Memory object allocation failure"); case CL_OUT_OF_RESOURCES: return strdup("Out of resources"); case CL_OUT_OF_HOST_MEMORY: return strdup("Out of host memory"); case CL_PROFILING_INFO_NOT_AVAILABLE: return strdup("Profiling information not available"); case CL_MEM_COPY_OVERLAP: return strdup("Memory copy overlap"); case CL_IMAGE_FORMAT_MISMATCH: return strdup("Image format mismatch"); case CL_IMAGE_FORMAT_NOT_SUPPORTED: return strdup("Image format not supported"); case CL_BUILD_PROGRAM_FAILURE: return strdup("Program build failure"); case CL_MAP_FAILURE: return strdup("Map failure"); case CL_INVALID_VALUE: return strdup("Invalid value"); case CL_INVALID_DEVICE_TYPE: return strdup("Invalid device type"); case CL_INVALID_PLATFORM: return strdup("Invalid platform"); case CL_INVALID_DEVICE: return strdup("Invalid device"); case CL_INVALID_CONTEXT: return strdup("Invalid context"); case CL_INVALID_QUEUE_PROPERTIES: return strdup("Invalid queue properties"); case CL_INVALID_COMMAND_QUEUE: return strdup("Invalid command queue"); case CL_INVALID_HOST_PTR: return strdup("Invalid host pointer"); case CL_INVALID_MEM_OBJECT: return strdup("Invalid memory object"); case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return strdup("Invalid image format descriptor"); case CL_INVALID_IMAGE_SIZE: return strdup("Invalid image size"); case CL_INVALID_SAMPLER: return strdup("Invalid sampler"); case CL_INVALID_BINARY: return strdup("Invalid binary"); case CL_INVALID_BUILD_OPTIONS: return strdup("Invalid build options"); case CL_INVALID_PROGRAM: return strdup("Invalid program"); case CL_INVALID_PROGRAM_EXECUTABLE: return strdup("Invalid program executable"); case CL_INVALID_KERNEL_NAME: return strdup("Invalid kernel name"); case CL_INVALID_KERNEL_DEFINITION: return strdup("Invalid kernel definition"); case CL_INVALID_KERNEL: return strdup("Invalid kernel"); case CL_INVALID_ARG_INDEX: return strdup("Invalid argument index"); case CL_INVALID_ARG_VALUE: return strdup("Invalid argument value"); case CL_INVALID_ARG_SIZE: return strdup("Invalid argument size"); case CL_INVALID_KERNEL_ARGS: return strdup("Invalid kernel arguments"); case CL_INVALID_WORK_DIMENSION: return strdup("Invalid work dimension"); case CL_INVALID_WORK_GROUP_SIZE: return strdup("Invalid work group size"); case CL_INVALID_WORK_ITEM_SIZE: return strdup("Invalid work item size"); case CL_INVALID_GLOBAL_OFFSET: return strdup("Invalid global offset"); case CL_INVALID_EVENT_WAIT_LIST: return strdup("Invalid event wait list"); case CL_INVALID_EVENT: return strdup("Invalid event"); case CL_INVALID_OPERATION: return strdup("Invalid operation"); case CL_INVALID_GL_OBJECT: return strdup("Invalid OpenGL object"); case CL_INVALID_BUFFER_SIZE: return strdup("Invalid buffer size"); case CL_INVALID_MIP_LEVEL: return strdup("Invalid mip-map level"); default: return strdup("Unknown"); } }

#ifdef DEBUG
    #define DEBUG_STDERR(x) do { std::cerr << x << endl; } while (0)
    #define DEBUG_STDOUT(x) do { std::cout << x << endl; } while (0)
#else
    #define DEBUG_STDERR(x)
    #define DEBUG_STDOUT(x)
#endif

const int Default_CubeSide = 2;
const double Default_LatticeOffset = 1.0; //sqrtf(3.f)/2;
const double Default_Timestep = 0.025;
const double Default_WallStiffness = 50.0;
const double Default_ForceCutoff = 3.0;

MDSim::MDSim() {
    cubeSide = Default_CubeSide;
    particleCount = cubeSide*cubeSide*cubeSide;
    
    latticeOffset = Default_LatticeOffset;
    timestep = Default_Timestep;
    
    wallStiffness = Default_WallStiffness;
    forceCutoff = Default_ForceCutoff;
    
    blockSizeX = 1;
    blockSizeY = 1;
}


int MDSim::setupCL() {
    cl_int status = CL_SUCCESS;
    
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CheckStatus(status, "clGetPlatformIDs, fetching number");
    DEBUG_STDOUT("Number of platforms: " << numPlatforms);
    
    cl_platform_id platform = NULL;
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        CheckStatus(status, "clGetPlatformIDs, fetching platforms");
        
        for (unsigned i = 0; i < numPlatforms; ++i) {
            char pbuf[100];
            status = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);
            CheckStatus(status, "clGetPlatformInfo");
            
            platform = platforms[i];
            if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) {
                break;
            }
        }
        delete[] platforms;
    }
    CheckConditional(platform != NULL, "platform == NULL");
    
    cl_uint numDevices;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    CheckStatus(status, "clGetDeviceIDs: fetching number");
    DEBUG_STDOUT("Number of devices: " << numDevices);
    
    cl_device_id *devices = new cl_device_id[numDevices];
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    CheckStatus(status, "clGetDeviceIDs: fetching devices");
    
    /* Create the context. */
    context = clCreateContext(0, numDevices, devices, NULL, NULL, &status);
    CheckConditional(context != NULL, "clCreateContextFromType");
    
    /* Create command queue */
    cl_command_queue_properties prop = CL_QUEUE_PROFILING_ENABLE;
    commandQueue = clCreateCommandQueue(context, devices[0], prop, &status);
    CheckStatus(status, "clCreateCommandQueue");
    
    /* Create a CL program using the kernel source */
    SDKFile kernelFile;
    std::string kernelPath = std::string("/Users/lwolf/Dropbox/School/sim/sim/VolumeRendering_Kernels.cl");
    if(!kernelFile.open(kernelPath.c_str())) {
        DEBUG_STDERR("Failed to load kernel file : " << kernelPath);
        return SDK_FAILURE;
    }
    
    const char *source = kernelFile.source().c_str();
    size_t sourceSize[] = {strlen(source)};
    program = clCreateProgramWithSource(context,
                                        1,
                                        &source,
                                        sourceSize,
                                        &status);
    CheckStatus(status, "clCreateProgramWithSource");
    
    /* Create a cl program executable for all the devices specified */
    status = clBuildProgram(program,
                            numDevices,
                            devices,
                            NULL,
                            NULL,
                            NULL);
    
    if (status != CL_SUCCESS) {
        if (status == CL_BUILD_PROGRAM_FAILURE) {
            cl_int logStatus;
            char *buildLog = NULL;
            size_t buildLogSize = 0;
            logStatus = clGetProgramBuildInfo(program,
                                              devices[0],
                                              CL_PROGRAM_BUILD_LOG,
                                              buildLogSize,
                                              buildLog,
                                              &buildLogSize);
            CheckStatus(logStatus, "clGetProgramBuildInfo");
            
            buildLog = (char*)malloc(buildLogSize);
            if(buildLog == NULL) {
                return SDK_FAILURE;
            }
            memset(buildLog, 0, buildLogSize);
            
            logStatus = clGetProgramBuildInfo(program,
                                              devices[0],
                                              CL_PROGRAM_BUILD_LOG,
                                              buildLogSize,
                                              buildLog,
                                              NULL);
            CheckStatus(logStatus, "clGetProgramBuildInfo (2)");
            
            DEBUG_STDERR("\n\t\t\tBUILD LOG\n");
            DEBUG_STDERR("************************************************\n");
            DEBUG_STDERR(buildLog);
            DEBUG_STDERR("************************************************\n");

            free(buildLog);
        }
    }
    CheckStatus(status, "clBuildProgram");
    
    /* Get a kernel object handle for a kernel with the given name */
    kernel = clCreateKernel(program, "square", &status);
    CheckStatus(status, "clCreateKernel");
    
    /* Check group size against group size returned by kernel */
    status = clGetKernelWorkGroupInfo(kernel,
                                      devices[0],
                                      CL_KERNEL_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &kernelWorkGroupSize,
                                      0);
    CheckStatus(status, "clGetKernelWorkGroupInfo");
    
    while ((blockSizeX * blockSizeY) < kernelWorkGroupSize) {
        if (2 * blockSizeX * blockSizeY <= kernelWorkGroupSize) {
            blockSizeX <<= 1;
        }
        if (2 * blockSizeX * blockSizeY <= kernelWorkGroupSize) {
            blockSizeY <<= 1;
        }
    }
    
    DEBUG_STDOUT("blockSizeX: " << blockSizeX);
    DEBUG_STDOUT("blockSizeY: " << blockSizeY);
    
    return SDK_SUCCESS;
}

int MDSim::runCLKernels() {
    for (int i = 0; i < cubeSide; i++) {
        for (int j = 0; j < cubeSide; j++) {
            for (int k = 0; k < cubeSide; k++) {
                // Set initial particle position
                molecules.emplace_back(new Molecule(i*latticeOffset, j*latticeOffset, k*latticeOffset));
            }
        }
    }

    std::function<void(MDSim*)> simAccel = [](MDSim* sim) {
        for (int i = 0; i < sim->particleCount; i++) {
            Molecule* m1 = sim->molecules[i].get();
            m1->addSpringForce(sim->wallStiffness, sim->dimensions);
        }
        
        for (int i = 0; i < sim->particleCount; i++) {
            Molecule* m1 = sim->molecules[i].get();
            for (int j = i+1; j < sim->particleCount; j++) {
                Molecule* m2 = sim->molecules[j].get();
                m1->positionDifference(m2);
            }
        }
    };
    
    for (int j = 0; j < 40000; j++) {
        // Single-step
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updatePosition(timestep);
        }

        simAccel(this);
    
        for (int i = 0; i < particleCount; i++) {
            molecules[i]->updateVelocityHalf(timestep);
        }
        
        if (j%40 == 0) {
            molecules[0]->printPosition();
          //  molecules[0]->printVelocity();
          //  molecules[0]->printAcceleration();
          //  std::cout << "-------------------------" << std::endl;
          //  molecules[7]->printPosition();
          //  molecules[7]->printVelocity();
          //  molecules[7]->printAcceleration();
          //  std::cout << "-------------------------" << std::endl;
        }
    }
    
    
    data = new double[particleCount];
    results = new double[particleCount];
    
        /**
         With liquid argon atoms, a pairwise force exists between each two atoms derived from the Lennard-Jones potential,
         ULJ(r) = 4ε[(σ/r)12 − (σ/r)6], where r is the distance between the two atoms, and ε and σ are constants specific to
         liquid argon, here being 9.977104 × 10−5  ̊A2· amu/fs2 and 3.405 ̊A , respectively. This is a repulsive force when atoms
         are near one another but acts as an attractive force when the distance is greater. It is calculated by taking the
         negative gradient of the Lennard-Jones potential energy function.
         **/
        
    cl_int status = CL_SUCCESS;
    
    // Allocate input and output buffers, and fill the input with data
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(double) * particleCount, NULL, NULL);
    
    // Create an output memory buffer for our results
    outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * particleCount, NULL, NULL);
    
    // Copy our host buffer of random values to the input device buffer
    status = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(double) * particleCount, data, 0, NULL, NULL);
    CheckStatus(status, "clEnqueueWriteBuffer");
    
    size_t global = particleCount;
    
    // Set the arguments to our kernel, and enqueue it for execution
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 2, sizeof(unsigned int), &particleCount);
    if (kernelWorkGroupSize > global) kernelWorkGroupSize = global;
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &global, &kernelWorkGroupSize, 0, NULL, NULL);
    CheckStatus(status, "clEnqueueNDRangeKernel");
    
    // Force the command queue to get processed, wait until all commands are complete
    status = clFinish(commandQueue);
    CheckStatus(status, "commandQueue");
    
    // Read back the results
    status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, sizeof(double) * particleCount, results, 0, NULL, NULL);
    CheckStatus(status, "clEnqueueReadBuffer");
    
    // Validate our results
    //int correct = 0;
    //for (int i = 0; i < count; i++) {
    //    correct += (results[i] == data[i] * data[i]) ? 1 : 0;
    //}
    
    // Print a brief summary detailing the results
    //printf("Computed '%d/%d' correct values!\n", correct, count);
    
    return SDK_SUCCESS;
}

int MDSim::cleanup() {
    /* Releases OpenCL resources (Context, Memory etc.) */
    cl_int status;
    
    status = clReleaseKernel(kernel);
    CheckStatus(status, "clReleaseKernel");
    
    status = clReleaseProgram(program);
    CheckStatus(status, "clReleaseProgram");
    
    status = clReleaseMemObject(inputBuffer);
    CheckStatus(status, "clReleaseMemObject inputBuffer");
    
    status = clReleaseMemObject(outputBuffer);
    CheckStatus(status, "clReleaseMemObject outputBuffer");
    
    status = clReleaseCommandQueue(commandQueue);
    CheckStatus(status, "clReleaseCommandQueue");
    
    status = clReleaseContext(context);
    CheckStatus(status, "clReleaseContext");
    
    /** Release program resources (input memory etc.) **/
    if (data) {
        delete[] data;
    }
    
    if (results) {
        delete[] results;
    }
    
    return SDK_SUCCESS;
}

void MDSim::computeAccelerations() {
    for (int i = 0; i < particleCount; i++) {
        Molecule* m1 = molecules[i].get();
        for (int j = i+1; j < particleCount; j++) {
            Molecule* m2 = molecules[j].get();
            m1->positionDifference(m2);            
        }
    }
}