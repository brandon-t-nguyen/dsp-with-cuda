#include <iostream>
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
