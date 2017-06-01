#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#include <vector>

class Operations
{
public:
    /**
     * Perform a convolution: y = x (*) h
     * @param[in]   x   input vector 1
     * @param[in]   h   input vector 2
     * @return      y   output vector
     */
    static std::vector<double> * convolve( const std::vector<double> * x,
                                           const std::vector<double> * h );

    /**
     * Perform a CPU convolution: y = x (*) h
     */
    static std::vector<double> * convolve_cpu( const std::vector<double> * x,
                                               const std::vector<double> * h );

    /**
     * Perform a GPU convolution: y = x (*) h
     */
    static std::vector<double> * convolve_gpu( const std::vector<double> * x,
                                               const std::vector<double> * h );
private:
    static bool useGpu;
};

#endif//__OPERATIONS_H__
