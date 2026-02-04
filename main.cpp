#include <bits/stdc++.h>


void bitonicSort(std::vector<int> &a, bool dir);

int main ()
{
    std::vector<int> a = {3, 7, 4, 8, 6, 2, 1, 5};
    
    bitonicSort(a, true);

    for (int x : a)
        std::cout << x << " ";
    std::cout << "\n";
    
        return 0;
}

void bitonicSort(std::vector<int> &a, bool dir)
{
    int n = (int)a.size();

    for (int k = 2; k <= n; k <<= 1)
    {
        for (int j = k >> 1; j > 0; j >>= 1)
        {
            for (int i = 0; i < n; i++)
            {
                int ixj = i ^ j;
                if (ixj > i)
                {
                    bool ascending = ((i & k) == 0) ? dir : !dir;

                    if (ascending)
                        if (a[i] > a[ixj]) std::swap(a[i], a[ixj]);                 
                    else 
                        if (a[i] < a[ixj]) std::swap(a[i], a[ixj]);
                }
            }
        }
    }
}
