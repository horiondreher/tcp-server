// create a simple hello word
#include <iostream>
#include <boost/array.hpp>

int main()
{
    boost::array<int, 4> arr = {{1,2,3,4}};

    for(const int& i : arr)
    {
        std::cout << i << std::endl;
    }

    return 0;
}