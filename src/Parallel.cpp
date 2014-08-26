//
//  Parallel.cpp
//  sim
//
//  Created by Lucas Wolf on 7/16/14.
//
//

#include <algorithm>
#include <stdlib.h>
#include "Parallel.h"
#include "Util.h"

/**
 * Debug-related macro/functions.
 */
#define CheckConditional(conditional, message) { if (!(conditional)) { std::cout << "ERROR: " << message << std::endl; return -1; } }
#define CheckStatus(status, message) { if (status != CL_SUCCESS) { std::cout << "ERROR: " << print_cl_errstring(status) << ", " << message << std::endl; return -1; } }
char *print_cl_errstring(cl_int err) { switch (err) { case CL_SUCCESS: return strdup("Success!"); case CL_DEVICE_NOT_FOUND: return strdup("Device not found."); case CL_DEVICE_NOT_AVAILABLE: return strdup("Device not available"); case CL_COMPILER_NOT_AVAILABLE: return strdup("Compiler not available"); case CL_MEM_OBJECT_ALLOCATION_FAILURE: return strdup("Memory object allocation failure"); case CL_OUT_OF_RESOURCES: return strdup("Out of resources"); case CL_OUT_OF_HOST_MEMORY: return strdup("Out of host memory"); case CL_PROFILING_INFO_NOT_AVAILABLE: return strdup("Profiling information not available"); case CL_MEM_COPY_OVERLAP: return strdup("Memory copy overlap"); case CL_IMAGE_FORMAT_MISMATCH: return strdup("Image format mismatch"); case CL_IMAGE_FORMAT_NOT_SUPPORTED: return strdup("Image format not supported"); case CL_BUILD_PROGRAM_FAILURE: return strdup("Program build failure"); case CL_MAP_FAILURE: return strdup("Map failure"); case CL_INVALID_VALUE: return strdup("Invalid value"); case CL_INVALID_DEVICE_TYPE: return strdup("Invalid device type"); case CL_INVALID_PLATFORM: return strdup("Invalid platform"); case CL_INVALID_DEVICE: return strdup("Invalid device"); case CL_INVALID_CONTEXT: return strdup("Invalid context"); case CL_INVALID_QUEUE_PROPERTIES: return strdup("Invalid queue properties"); case CL_INVALID_COMMAND_QUEUE: return strdup("Invalid command queue"); case CL_INVALID_HOST_PTR: return strdup("Invalid host pointer"); case CL_INVALID_MEM_OBJECT: return strdup("Invalid memory object"); case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return strdup("Invalid image format descriptor"); case CL_INVALID_IMAGE_SIZE: return strdup("Invalid image size"); case CL_INVALID_SAMPLER: return strdup("Invalid sampler"); case CL_INVALID_BINARY: return strdup("Invalid binary"); case CL_INVALID_BUILD_OPTIONS: return strdup("Invalid build options"); case CL_INVALID_PROGRAM: return strdup("Invalid program"); case CL_INVALID_PROGRAM_EXECUTABLE: return strdup("Invalid program executable"); case CL_INVALID_KERNEL_NAME: return strdup("Invalid kernel name"); case CL_INVALID_KERNEL_DEFINITION: return strdup("Invalid kernel definition"); case CL_INVALID_KERNEL: return strdup("Invalid kernel"); case CL_INVALID_ARG_INDEX: return strdup("Invalid argument index"); case CL_INVALID_ARG_VALUE: return strdup("Invalid argument value"); case CL_INVALID_ARG_SIZE: return strdup("Invalid argument size"); case CL_INVALID_KERNEL_ARGS: return strdup("Invalid kernel arguments"); case CL_INVALID_WORK_DIMENSION: return strdup("Invalid work dimension"); case CL_INVALID_WORK_GROUP_SIZE: return strdup("Invalid work group size"); case CL_INVALID_WORK_ITEM_SIZE: return strdup("Invalid work item size"); case CL_INVALID_GLOBAL_OFFSET: return strdup("Invalid global offset"); case CL_INVALID_EVENT_WAIT_LIST: return strdup("Invalid event wait list"); case CL_INVALID_EVENT: return strdup("Invalid event"); case CL_INVALID_OPERATION: return strdup("Invalid operation"); case CL_INVALID_GL_OBJECT: return strdup("Invalid OpenGL object"); case CL_INVALID_BUFFER_SIZE: return strdup("Invalid buffer size"); case CL_INVALID_MIP_LEVEL: return strdup("Invalid mip-map level"); default: return strdup("Unknown"); } }

#ifdef DEBUG
#define DEBUG_STDERR(x) do { std::cerr << x << std::endl; } while (0)
#define DEBUG_STDOUT(x) do { std::cout << x << std::endl; } while (0)
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif

/**
 * CL simulator class
 */
Parallel::Parallel() {}

int Parallel::setup() {
    /**
     * OpenCL initialization.
     */
    cl_int status = Simulator::setup();
    CheckStatus(status, "Simulator::setup() failed.");
    
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CheckStatus(status, "clGetPlatformIDs, fetching number");
    DEBUG_STDOUT("Number of platforms: " << numPlatforms);
    
    cl_platform_id platform = NULL;
    if (numPlatforms > 0) {
        std::unique_ptr<cl_platform_id[]> platforms (new cl_platform_id[numPlatforms]);
        status = clGetPlatformIDs(numPlatforms, platforms.get(), NULL);
        CheckStatus(status, "clGetPlatformIDs, fetching platforms");
        
        for (unsigned i = 0; i < numPlatforms; ++i) {
            char pbuf[100];
            status = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);
            CheckStatus(status, "clGetPlatformInfo");
        }
        
        // Just grab the first platform.
        platform = platforms[0];
    }
    CheckConditional(platform != NULL, "platform == NULL");
    
    cl_uint numDevices;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    CheckStatus(status, "clGetDeviceIDs: fetching number");
    DEBUG_STDOUT("Number of devices: " << numDevices);
    
    cl_device_id *devices = new cl_device_id[numDevices];
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    CheckStatus(status, "clGetDeviceIDs: fetching devices");
    
    int deviceIndex = 0;
    for (unsigned i = 0; i < numDevices; ++i) {
        char pbuf[100];
        status = clGetDeviceInfo(devices[i],
                                 CL_DEVICE_NAME,
                                 sizeof(pbuf),
                                 pbuf,
                                 NULL);
        if (!strncmp(pbuf, "ATI", 3)) {
            deviceIndex = i;
        }
    }
    
    /* Create the context. */
    context = clCreateContext(0, numDevices, devices, NULL, NULL, &status);
    CheckConditional(context != NULL, "clCreateContextFromType");
    
    /* Create command queue */
    cl_command_queue_properties prop = CL_QUEUE_PROFILING_ENABLE;
    commandQueue = clCreateCommandQueue(context, devices[deviceIndex], prop, &status);
    CheckStatus(status, "clCreateCommandQueue");
    
    /* Create a CL program using the kernel source */
    SDKFile kernelFile;
    std::string kernelPath = getenv("HOME") + std::string("/md-simulator/src/TestKernel.cl");
    if(!kernelFile.open(kernelPath.c_str())) {
        DEBUG_STDERR("Failed to load kernel file : " << kernelPath);
        return MD_FAILURE;
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
            std::unique_ptr<char[]> buildLog (nullptr);
            //char *buildLog = NULL;
            size_t buildLogSize = 0;
            logStatus = clGetProgramBuildInfo(program,
                                              devices[deviceIndex],
                                              CL_PROGRAM_BUILD_LOG,
                                              buildLogSize,
                                              buildLog.get(),
                                              &buildLogSize);
            CheckStatus(logStatus, "clGetProgramBuildInfo");
            
            buildLog = std::unique_ptr<char[]>(new char[buildLogSize]);
            if(!buildLog) {
                return MD_FAILURE;
            }
            std::fill_n(buildLog.get(), buildLogSize, 0);
            
            logStatus = clGetProgramBuildInfo(program,
                                              devices[deviceIndex],
                                              CL_PROGRAM_BUILD_LOG,
                                              buildLogSize,
                                              buildLog.get(),
                                              NULL);
            CheckStatus(logStatus, "clGetProgramBuildInfo (2)");
            
            DEBUG_STDERR("\n\t\t\tBUILD LOG\n");
            DEBUG_STDERR("************************************************\n");
            DEBUG_STDERR(buildLog.get());
            DEBUG_STDERR("************************************************\n");
        }
    }
    CheckStatus(status, "clBuildProgram");
    
    /* Get a kernel object handle for a kernel with the given name */
    kernel = clCreateKernel(program, "computeAccelerations", &status);
    CheckStatus(status, "clCreateKernel");
    
    /* Check group size against group size returned by kernel */
    status = clGetKernelWorkGroupInfo(kernel,
                                      devices[deviceIndex],
                                      CL_KERNEL_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &kernelWorkGroupSize,
                                      0);
    CheckStatus(status, "clGetKernelWorkGroupInfo");
    DEBUG_STDOUT("kernelWorkGroupSize: " << kernelWorkGroupSize);
    
    /**
     * Initialize some simulator data structures.
     */
    global = particleCount * particleCount;
    local = particleCount;
    
    if (global * local > kernelWorkGroupSize) {
        DEBUG_STDERR("WARNING - global * local > kernelWorkGroupSize; global: " << global << ", local: " << local << ", kernelWorkGroupSize: " << kernelWorkGroupSize);
        return MD_FAILURE;
    }
    
    // Data holds the molecule positions.
    data = std::unique_ptr<float[]> (new float[particleCount * 3]);
    
    // Constants holds simulator constants.
    constants = std::unique_ptr<float[]> (new float[NUM_CONSTANTS]);
    
    // Copy constants to buffer;
    constants[0] = epsilon;
    constants[1] = sigma;
    constants[2] = negForceCutoffMinusHalf;
    constants[3] = forceCutoffMinusHalf;
    constants[4] = wallStiffness;
    
    // Results holds pairwise forces.
    results = std::unique_ptr<float[]> (new float[particleCount * particleCount * 3]);
    
    return MD_SUCCESS;
}

int Parallel::computeAccelerations() {
    cl_int status = MD_SUCCESS;
    
    // Copy positions to buffer.
    for (int i = 0, j = 0; i < particleCount; i++, j+= 3) {
        std::array<double, 3>::const_pointer p = molecules[i]->getPosition();
        data[j] = p[0];
        data[j+1] = p[1];
        data[j+2] = p[2];
    }
    
    // Allocate input and output buffers, and fill the input with data
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * particleCount * 3, NULL, NULL);
    constantsBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * NUM_CONSTANTS, NULL, NULL);
    // Create an output memory buffer for our results
    outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * particleCount * particleCount * 3, NULL, NULL);
    
    // Copy our host buffer of random values to the input device buffer
    status = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(float) * particleCount * 3, data.get(), 0, NULL, NULL);
    CheckStatus(status, "clEnqueueWriteBuffer: inputs");
    status = clEnqueueWriteBuffer(commandQueue, constantsBuffer, CL_TRUE, 0, sizeof(float) * NUM_CONSTANTS, constants.get(), 0, NULL, NULL);
    CheckStatus(status, "clEnqueueWriteBuffer: constants");

    // Set the arguments to our kernel, and enqueue it for execution
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &constantsBuffer);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 3, sizeof(unsigned int), &particleCount);
    
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    CheckStatus(status, "clEnqueueNDRangeKernel");
    
    // Force the command queue to get processed, wait until all commands are complete
    status = clFinish(commandQueue);
    CheckStatus(status, "commandQueue");
    
    // Read back the results
    status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, sizeof(float) * particleCount * particleCount * 3, results.get(), 0, NULL, NULL);
    CheckStatus(status, "clEnqueueReadBuffer");

    for (int i = 0, j = 0; j < global; i++, j+= local) {
        molecules[i]->setAcceleration(results[j*3], results[j*3+1], results[j*3+2]);
    }
    
    status = clReleaseMemObject(outputBuffer);
    CheckStatus(status, "clReleaseMemObject outputBuffer");
    
    status = clReleaseMemObject(constantsBuffer);
    CheckStatus(status, "clReleaseMemObject constantsBuffer");
    
    status = clReleaseMemObject(inputBuffer);
    CheckStatus(status, "clReleaseMemObject inputBuffer");
    
    return status;
}

int Parallel::cleanup() {
    /* Releases OpenCL resources (Context, Memory etc.) */
    cl_int status;
    
    status = clReleaseKernel(kernel);
    CheckStatus(status, "clReleaseKernel");
    
    status = clReleaseProgram(program);
    CheckStatus(status, "clReleaseProgram");
    
    status = clReleaseCommandQueue(commandQueue);
    CheckStatus(status, "clReleaseCommandQueue");
    
    status = clReleaseContext(context);
    CheckStatus(status, "clReleaseContext");
    
    return MD_SUCCESS;
}