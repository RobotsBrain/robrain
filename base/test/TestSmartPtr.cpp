#include <iostream>

#include "SmartPtr.h"



using namespace std;

int main(int argc, char **argv)
{
    int *i = new int(2);

    {
        Base::SmartPtr<int> ptr1(i);

        {
            Base::SmartPtr<int> ptr2(ptr1);

            {
                Base::SmartPtr<int> ptr3 = ptr2;

                cout << *ptr1 << endl;
                *ptr1 = 20;
                cout << *ptr2 << endl;

            }
        }
    }

    return 0;
}


