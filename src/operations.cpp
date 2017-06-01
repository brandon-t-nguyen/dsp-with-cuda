#include <operations.h>

bool Operations::useGpu = false;

std::vector<double> * Operations::convolve( const std::vector<double> * x,
                                            const std::vector<double> * h )
{
    if( Operations::useGpu )
    {
        return convolve_gpu(x,h);
    }
    else
    {
        return convolve_cpu(x,h);
    }
}


std::vector<double> * Operations::convolve_cpu( const std::vector<double> * x,
                                                const std::vector<double> * h )
{
    int N = x->size() + h->size() - 1;  // total length of the convolution
    int M = h->size();  // let x stay in place
    std::vector<double> * y = new std::vector<double>(N);

    // point by point convolution, mathematical definition
    for(int n = 0; n < N; ++n)
    {
        double sum = 0.0;
        for(int m = 0; m < M; ++m)
        {
            int xIndex = n - m;
            double xVal = xIndex < 0? 0.0 : (*x)[xIndex];
            sum += (*h)[m] * xVal;
        }
        (*y)[n] = sum;
    }

    return y;
}
