#include <dsp/signal.h>

#ifndef GPU_EN
using namespace dsp;
Signal Signal::convolve( const Signal & x, const Signal & h )
{
    int xLen    = x.m_length;
    int hLen    = h.m_length;
    int convLen = xLen + hLen - 1;

    const std::complex<float> * x_buffer = x.m_samples;
    const std::complex<float> * h_buffer = h.m_samples;
    std::complex<float> * y_buffer = new std::complex<float>[convLen];

    // very mathematical way of doing it
    // for each point x[n] and h[-n] are in contact,
    // do the sum of products between overlapping x[n] and h[n]
    for(int n = 0; n < convLen; ++n)
    {
        std::complex<float> sum = 0;
        for(int hI = 0, xI = n; hI < hLen; ++hI, --xI)
        {
            std::complex<float> hVal = h_buffer[hI];
            std::complex<float> xVal = {0.0f,0.0f};
            if( 0 <= xI && xI < xLen )
            {
                xVal = x_buffer[xI];
            }
            sum += xVal * hVal;
        }
        y_buffer[n] = sum;
    }

    Signal y;
    y.m_samples  = y_buffer;
    y.m_length   = convLen;
    y.m_position = x.m_position - hLen + 1; // since x stays in place
    return y;
}
#endif
