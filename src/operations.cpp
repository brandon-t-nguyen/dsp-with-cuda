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
    int convLen  = x->size() + h->size() - 1;  // total length of the convolution
    int hL = h->size();  // let x stay in place
    int xL = x->size();  // let x stay in place
    std::vector<double> * y = new std::vector<double>(convLen);

    for(int n = 0; n < convLen; ++n)
    {
        double sum = 0;
        for(int hI = 0, xI = n; hI < hL; ++hI, --xI)
        {
            double hVal = (*h)[hI];
            double xVal = 0.0;
            if( 0 <= xI && xI < xL )
            {
                xVal = (*x)[xI];
            }
            sum += xVal * hVal;

        }
        (*y)[n] = sum;
    }

    return y;
}
