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

int main(int argc, char * argv[])
{
    std::cout << "Hello world!" << std::endl;

    float x_raw[] = {1, 2, 3, 4, 5};
    float h_raw[] = {0, 1, 2, 3, 1};
    Signal x(ARRAY_LEN(x_raw),x_raw);
    Signal h(ARRAY_LEN(h_raw),h_raw);
    Signal y = Signal::convolve(x,h);

    printSignal(x,"x");
    printSignal(h,"h");
    printSignal(y,"y");

    return 0;
}
