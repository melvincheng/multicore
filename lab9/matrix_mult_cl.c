//
// File:       hello.c
//
// Abstract:   A simple "Hello World" compute example showing basic usage of OpenCL which
//             calculates the mathematical square (X[i] = pow(X[i],2)) for a buffer of
//             floating point values.
//
//
// Version:    <1.0>
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2008 Apple Inc. All Rights Reserved.
//

////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/cl.h>

////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//
#define DATA_SIZE (1024)

////////////////////////////////////////////////////////////////////////////////

// Simple compute kernel which computes the square of an input array
//
const char *KernelSource = "\n" \
"__kernel void square(                                                  \n" \
"   __global float* a,                                                  \n" \
"   __global float* b,                                                  \n" \
"   __global float* ab,                                                 \n" \
"   const int m_dim,                                                    \n" \
"   const int k_dim,                                                    \n" \
"   const int n_dim)                                                    \n" \
"{                                                                      \n" \
"   int m = get_global_id(0);                                           \n" \
"   int n = get_global_id(1);                                           \n" \
"   float temp = 0;                                                     \n" \
"   for (int k=0; k< k_dim; k++) {                                      \n" \
"       temp +=  a[(m*k_dim+k)] * b[(k*n_dim+n)];                       \n" \
"   }                                                                   \n" \
"   if(n < n_dim && m < m_dim){                                         \n" \
"       ab[(m*n_dim+n)] = temp;                                         \n" \
"   }                                                                   \n" \
"}                                                                      \n" \
"\n";



////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    int err;                            // error code returned from api calls
    int md = 4;
    int kd = 2;
    int nd = 3;
    int k, n, m;

    float matrix_a[md*kd];
    float matrix_b[kd*nd];
    float matrix_result[md*nd];

    float data[DATA_SIZE];              // original data set given to device
    float results[DATA_SIZE];           // results returned from device
    unsigned int correct;               // number of correct results returned

    size_t global[2];                      // global domain size for our calculation
    size_t local[2];                       // local domain size for our calculation

    cl_platform_id platform_id;
    cl_uint num_platforms;
    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel

    cl_mem a;                       // device memory used for the input array
    cl_mem b;
    cl_mem ab;                      // device memory used for the output array

    // Fill our data set with random float values
    //
    printf("MATRIX A\n");
    for (m=0; m < md; m++) {
        for (k=0; k < kd; k++) {
            matrix_a[m*kd+k] = m*k+1;
            printf("%f\t", matrix_a[m*kd+k]);
        }
        printf("\n");
    }

    printf("MATRIX B\n");
    for (k=0; k < kd; k++) {
        for (n=0; n < nd; n++) {
            matrix_b[k*nd+n] = k*n+1;
            printf("%f\t", matrix_b[k*nd+n]);
        }
        printf("\n");
    }

    err = clGetPlatformIDs(1,&platform_id,&num_platforms);

    // Connect to a compute device
    //
    int gpu = 0;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }

    // Create a compute context
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context)
    {
        printf("Error: Failed to create a compute context!\n");
        return EXIT_FAILURE;
    }

    // Create a command commands
    //
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (!commands)
    {
        printf("Error: Failed to create a command commands!\n");
        return EXIT_FAILURE;
    }

    // Create the compute program from the source buffer
    //
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }

    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }

    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "square", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }

    // Create the input and output arrays in device memory for our calculation
    //
    a = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * (md * kd), NULL, NULL);
    b = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * (kd * nd), NULL, NULL);
    ab = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * (md * nd), NULL, NULL);
    if (!a || !b || !ab)
    {
        printf("Error: Failed to allocate device memory!\n");
        exit(1);
    }

    // Write our data set into the input array in device memory
    //
    err = clEnqueueWriteBuffer(commands, a, CL_TRUE, 0, sizeof(float) * md * kd, matrix_a, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(commands, b, CL_TRUE, 0, sizeof(float) * kd * nd, matrix_b, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to write to source array!\n");
        exit(1);
    }

    // Set the arguments to our compute kernel
    //
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &ab);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &md);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &kd);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &nd);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }

    // Get the maximum work group size for executing the kernel on the device
    //
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }

    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    //
    local[0] = 16;
    local[1] = 16;
    global[0] = DATA_SIZE;
    global[1] = DATA_SIZE;
    err = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, global, local, 0, NULL, NULL);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }

    // Wait for the command commands to get serviced before reading back results
    //
    clFinish(commands);

    // Read back the results from the device to verify the output
    //
    err = clEnqueueReadBuffer( commands, ab, CL_TRUE, 0, sizeof(float) * md * nd, matrix_result, 0, NULL, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }

    // Validate our results
    //
    printf("MATRIX AB (RESULT)\n");
    for(int i = 0; i < md; i++){
        for(int j = 0; j < nd; j++){
            printf("%f\t", matrix_result[i * nd + j]);
        }
        printf("\n");
    }

    // Print a brief summary detailing the results
    //
    // printf("Computed '%d/%d' correct values!\n", correct, count);

    // Shutdown and cleanup
    //
    clReleaseMemObject(a);
    clReleaseMemObject(b);
    clReleaseMemObject(ab);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    return 0;
}
