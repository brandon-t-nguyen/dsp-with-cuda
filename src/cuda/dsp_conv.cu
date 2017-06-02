#ifdef GPU_EN

#include <dsp/signal.h>
#include <cuda.h>
#include <cuComplex.h>

/**
 * multiply-accumulate for each slide
 *
 * Since processing y[n] has a sequential dependency
 * on h[n'], h[n'+1], h[n'+2] etc due to the MAC summation,
 * process one MAC across all shifts at once. Thus, for y of size N,
 * we launch N threads.
 *
 * @param[in]   x
 * @param[in]   h
 * @param[in]   y
 * @param[in]   xLen
 * @param[in]   hLen
 * @param[in]   yLen
 * @param[in]   hI      position in h[m]
 * @param[in]   offset  current offset in our processing,
 *                      since there's a block dimension limit
 */
// TODO: we can optimize the parameters: what do we really need?
__global__
void mac_slide( const cuFloatComplex * x, const cuFloatComplex * h, cuFloatComplex *y,
                int                 xLen, int                 hLen, int          yLen,
                int hI, int offset)
{
    // TODO: have calculations for offset. Thus we're limited to
    //       1024*1024 threads for the time being
    int n = blockIdx.x * blockDim.x + threadIdx.x; // TODO: fix the calculation
    int xI = n - hI;
    cuFloatComplex sum  = y[n];  // pick up the last sum
    cuFloatComplex xVal = make_cuFloatComplex(0.0f,0.0f);
    if( 0 <= xI && xI < xLen )
    {
        
    }
    cuFloatComplex hVal = h[hI]; // TODO: optimization: provide hVal from host
                                 //       don't need to memcpy h[n]
    cuFloatComplex product = cuCmulf(xVal, hVal);   // multiply
    y[n] = cuCaddf(sum, product);                   // accumulate
}

void complex2cuFloatComplex( int len, const std::complex<float> cpp[], cuFloatComplex cu[] )
{
    for(int i = len-1; i >= 0; --i)
    {
        cu[i] = make_cuFloatComplex( cpp[i].real(), cpp[i].imag() );
    }
}

void cuFloatComplex2complex( int len, const cuFloatComplex cu[], std::complex<float> cpp[])
{
    for(int i = len-1; i >= 0; --i)
    {
        cpp[i] = std::complex<float>(cuCrealf(cu[i]),cuCimagf(cu[i]));
    }
}

using namespace dsp;
Signal Signal::convolve( const Signal & x, const Signal & h )
{
    int xLen    = x.m_length;
    int hLen    = h.m_length;
    int yLen    = xLen + hLen - 1;

    const std::complex<float> * x_buffer = x.m_samples;
    const std::complex<float> * h_buffer = h.m_samples;
    std::complex<float> * y_buffer = new std::complex<float>[yLen];

    cuFloatComplex * h_x, * h_h, * h_y, * d_x, * d_h, * d_y;
    h_x = new cuFloatComplex[xLen];
    h_h = new cuFloatComplex[hLen];
    h_y = new cuFloatComplex[yLen];
    for(int i = yLen-1; i >= 0; --i)
    {
        y_buffer[i] = std::complex<float>(0,0);    // zero-init
    }
    // convert std::complex to cuFloatComplex
    complex2cuFloatComplex( xLen, x_buffer, h_x );
    complex2cuFloatComplex( hLen, h_buffer, h_h );
    
    cudaMalloc(&d_x, sizeof(cuFloatComplex)*xLen);
    cudaMalloc(&d_h, sizeof(cuFloatComplex)*hLen);
    cudaMalloc(&d_y, sizeof(cuFloatComplex)*yLen);

    cudaMemcpy(d_x, h_x, sizeof(cuFloatComplex)*xLen, cudaMemcpyHostToDevice);
    cudaMemcpy(d_h, h_h, sizeof(cuFloatComplex)*hLen, cudaMemcpyHostToDevice);
    // launch our things in parallel,
    // then lockstep the MAC
    for(int hI = 0; hI < hLen; ++hI)
    {
        mac_slide<<<yLen,yLen/1024>>>( d_x,  d_h,  d_y,
                                       xLen, hLen, yLen,
                                       hI, 0 );
    }
    cudaMemcpy(d_y, h_y, sizeof(cuFloatComplex)*hLen, cudaMemcpyDeviceToHost);

    cuFloatComplex2complex( yLen, h_y, y_buffer );
    Signal y;
    y.m_samples  = y_buffer;
    y.m_length   = yLen;
    y.m_position = x.m_position - hLen + 1; // since x stays in place
    
    cudaFree(d_x);
    cudaFree(d_h);
    cudaFree(d_y);
    delete[] h_x;
    delete[] h_h;
    delete[] h_y;
    return y;
}
#endif
