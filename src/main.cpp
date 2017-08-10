#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <dsp/signal.h>
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))
using namespace dsp;
using namespace std::chrono;
std::ostream & operator<<(std::ostream & os, const std::complex<float> & num)
{
    #define EPSILON (1e-5)
    os << num.real();
    if( (num.imag()>0?num.imag():-num.imag()) > EPSILON )
    {
        os << " + " << num.imag() << "i";
    }
    return os;
}

void printConvExpResultsHeader()
{
    std::cout << "Input Size,"
              << "CPU mean time (us),"
              << "GPU mean time (us),"
              << "CPU StdDev (us),"
              << "GPU StdDev (us),"
              << std::endl;
}

struct ConvExpResults
{
    int dataSize;
    double cpuTime;
    double cpuSD;
    double gpuTime;
    double gpuSD;
};

std::ostream & operator<<(std::ostream & os, const ConvExpResults & res)
{
    os << res.dataSize << ","
       << res.cpuTime << "," << res.gpuTime << ","
       << res.cpuSD   << "," << res.gpuSD   << ","
       << std::endl;
    return os;
}

// using sqrt(E[x^2] - E[x]^2)
// to reduce number of subtracts
void calculateStats(int len, const double results[], double * meanOut, double * sdOut)
{
    // calculate E[x] and E[x^2]
    double eX  = 0.0;
    double eX2 = 0.0;
    for(int i = len-1; i >= 0; --i)
    {
        eX += results[i];
        eX2 += (results[i]*results[i]);
    }
    eX /= len;
    eX2 /= len;

    double var = eX2 - eX*eX;
    double sd  = sqrt(var);

    *meanOut = eX;
    *sdOut   = sd;
}

void generateSignalBuffer(int len, std::complex<float> * buffer)
{
    srand(static_cast<unsigned>(time(0)));
    for(int i = 0; i < len; ++i)
    {
        float real, imag;
        real = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        imag = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        buffer[i] = std::complex<float>(real,imag);
    }
}

ConvExpResults performExperiment(int size)
{
    constexpr int REPETITIONS = 30;
    constexpr int SCALING     = 20;  // scale for timing
    static double trials[REPETITIONS];

    ConvExpResults results;
    results.dataSize = size*2;  // x and h are same size

    std::complex<float> * x_raw = new std::complex<float>[size];
    std::complex<float> * h_raw = new std::complex<float>[size];

    // cpu
    for(int i = 0; i < REPETITIONS; ++i)
    {
        generateSignalBuffer(size,x_raw);
        generateSignalBuffer(size,h_raw);
        Signal x(size,x_raw);
        Signal h(size,h_raw);

        steady_clock::time_point start =  steady_clock::now();
        for(volatile int i = 0; i < SCALING; i++)
        {
            Signal temp = Signal::convolve_cpu(x,h);
        }
        steady_clock::time_point end   =  steady_clock::now();
        duration<double> time = duration_cast<duration<double>>(end - start);
        // mark result
        trials[i] = time.count()*1000000/SCALING;
    }
    calculateStats(REPETITIONS,trials,&(results.cpuTime),&(results.cpuSD));

    // gpu
    for(int i = 0; i < REPETITIONS; ++i)
    {
        generateSignalBuffer(size,x_raw);
        generateSignalBuffer(size,h_raw);
        Signal x(size,x_raw);
        Signal h(size,h_raw);

        steady_clock::time_point start =  steady_clock::now();
        for(volatile int i = 0; i < SCALING; i++)
        {
            Signal temp = Signal::convolve_gpu(x,h);
        }
        steady_clock::time_point end   =  steady_clock::now();
        duration<double> time = duration_cast<duration<double>>(end - start);
        // mark result
        trials[i] = time.count()*1000000/SCALING;
    }
    calculateStats(REPETITIONS,trials,&(results.gpuTime),&(results.gpuSD));


    delete[] x_raw;
    delete[] h_raw;

    return results;
}

// for scaling
static int dataSizes[] = {5, 10, 25, 50, 75, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

// for threshold finding
/*
static int dataSizes[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100,
105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200,
205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255, 260, 265, 270, 275, 280, 285, 290, 295, 300,
};
*/
void experiment()
{
    ConvExpResults results[ARRAY_LEN(dataSizes)];

    printConvExpResultsHeader();
    for(int i = 0; i < ARRAY_LEN(dataSizes); ++i)
    {
        results[i] = performExperiment(dataSizes[i]);
        std::cout << results[i];
    }
}

void printSignal(const Signal & signal, const char * name)
{
    std::cout << name << ": { ";
    for(int i = 0; i < signal.length()-1; ++i)
    {
        std::cout << signal[i] << ", ";
    }
    int i = signal.length()-1;
    std::cout << signal[i] << " }" << std::endl;
}
int main(int argc, char * argv[])
{
    //experiment();
    //float x_buf[8] = {0,1,2,3,4,5,6,7};
    float x_buf[8] = {0,1,0,1,0,1,0,1};
    //float x_buf[5] = {0,1,2,3,4};
    Signal x = Signal(sizeof(x_buf)/sizeof(x_buf[0]),x_buf);
    printSignal(x, "x");
    Signal X = Signal::fft_cpu(x);
    printSignal(X, "fft(x)");
    printSignal(Signal::ifft_cpu(X), "ifft(fft(x))");

    return 0;
};
