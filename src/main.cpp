#include <iostream>
/*
#include <vector>
#include <operations.h>

void printVector(const std::vector<double> & v, const char * name)
{
    std::cout << name << ": { ";
    for(int i = 0; i < v.size()-1; ++i)
    {
        std::cout << v[i] << ", ";
    }
    std::cout << v.back() << " }" << std::endl;
}

int main(int argc, char * argv[])
{
    std::cout << "Hello world!" << std::endl;

    double x_raw[] = {1, 2, 3, 4, 5};
    double h_raw[] = {0, 1, 2, 3, 1};

    std::vector<double> x (x_raw, x_raw + sizeof(x_raw)/sizeof(x_raw[0]));
    std::vector<double> h (h_raw, h_raw + sizeof(h_raw)/sizeof(h_raw[0]));

    std::vector<double> * py = Operations::convolve(&x, &h);

    printVector(x,"x");
    printVector(h,"h");
    printVector(*py,"y");

    return 0;
}
*/
#include <chrono>
#include <dsp/signal.h>
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))
using namespace dsp;

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
#include <cstdlib>
#include <ctime>
int main(int argc, char * argv[])
{
    std::cout << "Hello world!" << std::endl;

    /*
    float x_raw[] = {1, 2, 3, 4, 5};
    float h_raw[] = {0, 1, 2, 3, 1};
    Signal x(ARRAY_LEN(x_raw),x_raw);
    Signal h(ARRAY_LEN(h_raw),h_raw);
    */
    /*
    std::complex<float> x_raw[] = {{1,2}, {2,-1}, {3,0}, {4,0}, {5,3}};
    std::complex<float> h_raw[] = {{0,0}, {1,0}, {2,2}, {3,0}, {1,0}};
    Signal x(ARRAY_LEN(x_raw),x_raw);
    Signal h(ARRAY_LEN(h_raw),h_raw);
    */

    ///*
    int len = 500;
    srand(static_cast<unsigned>(time(0)));
    std::complex<float> * x_raw = new std::complex<float>[len];
    std::complex<float> * h_raw = new std::complex<float>[len];
    for(int i = 0; i < len; ++i)
    {
        float real, imag;
        real = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        imag = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        x_raw[i] = std::complex<float>(real,imag);
        real = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        imag = (static_cast<float>(rand())/static_cast<float>(RAND_MAX)) * 10 - 20;
        h_raw[i] = std::complex<float>(real,imag);
    }
    Signal x(len,x_raw);
    Signal h(len,h_raw);
    //*/

    int repeats = 1000;
    ///*
    using namespace std::chrono;
    steady_clock::time_point start =  steady_clock::now();
    for(volatile int i = 0; i < repeats; i++)
    {
        Signal temp = Signal::convolve(x,h);
    }
    steady_clock::time_point end   =  steady_clock::now();
    duration<double> time = duration_cast<duration<double>>(end - start);
    std::cout << "It took " << time.count() << " seconds to execute " << repeats << std::endl;
    //*/
    Signal y = Signal::convolve(x,h);
    printSignal(x,"x");
    printSignal(h,"h");
    printSignal(y,"y");
    std::cout << "It took " << time.count() << " seconds to execute " << repeats << std::endl;
    delete[] x_raw;
    delete[] h_raw;
    return 0;
}

struct Experiment
{

};

void experiment()
{

}
