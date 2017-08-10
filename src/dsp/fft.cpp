#include <dsp/signal.h>
#include <brandon/log.h>

#define __USE_MATH_DEFINES
#include <math.h>

using namespace dsp;
Signal Signal::fft( const Signal & x )
{
    // decide to call cpu or gpu implementation
    // based on input size
    if(x.length() < 130)
    {
        return fft_cpu(x);
    }
    else
    {
        return fft_gpu(x);
    }
}

#ifndef GPU_EN
Signal Signal::fft_gpu( const Signal & x )
{
    return fft_cpu(x);
}
#endif

// does bit reversal, given width
#define MOVE_BIT(val,src,dst) (((val >> src) & 0x1) << dst)
static int reverse( int x, int w )
{
    int tmp = 0x0;
    for (int i = 0; i < w/2+1; ++i)
    {
        tmp |= (MOVE_BIT(x,i,w-1-i) | MOVE_BIT(x,w-1-i,i));
    }
    return tmp;
}

/**
 * Calculates a single N-point butterfly
 * @param   length  how big the total fft is
 * @param   base    what is the index offset of the butterfly is
 * @param   stride  how long the butterfly is
 */
static inline
void rad2_dit_butterfly( int length, int base, int stride,
                         std::complex<float> * src,
                         std::complex<float> * dst,
                         std::complex<float> * twiddle )
{
    int twiddleOff = length / stride;
    int i;
    for (i = 0; i < stride/2; ++i)
    {
        dst[base + i] = src[base + i] +
                        src[base + i + stride/2] * twiddle[twiddleOff * i];
    }
    for (; i < stride; ++i)
    {
        dst[base + i] = src[base + i - stride/2] +
                        src[base + i] * twiddle[twiddleOff * i];
    }
}

static
void printBuffer( int length, std::complex<float> * buffer )
{
    for (int i = 0; i < length; ++i)
    {
        dprintf("[%d] %g + %gi\n", i, buffer[i].real(), buffer[i].imag());
    }
}

static Signal fft_rad2_dit( const Signal & x )
{
    // normalize x to a certain length
    // determine if x's length is a power of 2
    int origLength = x.length();
    bool power2 = false;
    int length = 1;
    int stages = 0;
    while (length < origLength && length > 0)
    {
        ++stages;
        length <<= 1;
        if (length == origLength)
        {
            power2 = true;
            break;
        }
    }

    // calculate twiddle factors
    std::complex<float> * twiddle = new std::complex<float>[length];
    for (int i = 0; i < length; ++i)
    {
        twiddle[i] = std::exp(std::complex<float>(0.0f,-2*M_PI*i/length));
        dprintf("W %d/%d: %f + %fi\n", i, length, twiddle[i].real(), twiddle[i].imag());
    }

    // two buffers; a source to work with, and one to write to
    // alternate between the two
    int curr = 0;
    #define SRC(s) (s)
    #define DST(s) ((s+1)&0x1)
    std::complex<float> * buffer[2];
    buffer[0] = new std::complex<float>[length];
    buffer[1] = new std::complex<float>[length];
    // lets put into bit reverse order for dit, starting from 0
    for (int i = 0; i < length; ++i)
    {
        int srcInd = reverse(i, stages);
        dprintf("[%d] -> [%d]\n", i, srcInd);
        buffer[SRC(curr)][i] = srcInd < origLength ?
                                        x[srcInd] :
                                        std::complex<float>(0.0f, 0.0f);
    }

    // inital stride is 2 for the starting 2-point dft
    int stride = 2;
    for (int i = 0; i < stages; ++i)
    {
        dprintf("Stage %d:\n", i);
        printBuffer( length, buffer[SRC(curr)] );
        for (int j = length - stride; j >= 0; j -= stride)
        {
            rad2_dit_butterfly( length, j, stride,
                                buffer[SRC(curr)],
                                buffer[DST(curr)],
                                twiddle );
        }
        dprintf("====\n");

        stride <<= 1;
        curr = DST(curr);  // switch buffer
    }
    dprintf("Stage %d:\n", stages);
    printBuffer( length, buffer[SRC(curr)] );
    dprintf("====\n\n");

    Signal X = Signal(length,buffer[SRC(curr)]);
    delete[] buffer[0];
    delete[] buffer[1];
    delete[] twiddle;
    return X;
    #undef SRC
    #undef DST
}

Signal Signal::fft_cpu( const Signal & x )
{
    Signal X = fft_rad2_dit( x );
    return X;
}
