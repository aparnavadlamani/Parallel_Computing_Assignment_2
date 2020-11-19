//To compile: g++ -fopenmp cs17b005.cpp -o cs17b005

#include<iostream>
#include<chrono>
#include<random>
#include<cmath>
#include "omp.h"

using namespace std::chrono;

int main(int argc, char* argv[])
{
    int n = atoi(argv[1]);
    float th = atof(argv[2]);
    int n1 = n+2;
    
    float* mat = (float*) malloc((n1)*(n1)*sizeof(float));
    float* mat1 = (float*) malloc((n1)*(n1)*sizeof(float));

    for(int i=0;i<n1;i++)
    {
        for(int j=0;j<n1;j++)
        {
            mat[i*n1+j] = 0;
            mat1[i*n1+j] = 0;
        }
    }
    
    float lbound = 0;
    float ubound = 10;
    std::uniform_real_distribution<float> urd(lbound, ubound);
    std::default_random_engine re;
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
        {
            mat[i*n1+j] = urd(re);
        }
    }
    /*for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
        {
            std::cout<<mat[i*n1+j]<<" ";
        }
        std::cout<<std::endl;
    }*/
    bool check = true;
    int num_threads = 4, iter = 0;

    auto start = high_resolution_clock::now();

    omp_set_num_threads(num_threads);
    while(check)
    {
        double diff = 0.0;
        #pragma omp parallel
        {
        #pragma omp for collapse(2) reduction(max:diff)
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                float num = 5.0;
                float avg = mat[i*n1+j]+mat[i*n1+j+1]+mat[i*n1+j-1]+mat[i*n1+n1+j]+mat[i*n1-n1+j];
                
                if(i==1||i==n)
                {
                    num-=1;
                }
                if(j==1||j==n)
                {
                    num-=1;
                }
                avg = avg/num;
                diff = std::max(diff, fabs(avg-mat[i*n1+j]));
                if(diff<th)
                {
                    check = false;
                }
                mat1[i*n1+j] = avg;
            }
        }
        #pragma omp for collapse(2)
        for(int i=1;i<=n;i++)
        {
            for(int j=0;j<=n;j++)
            {
                mat[i*n1 + j] = mat1[i*n1+j];
            }
        }
        }
        iter+=1;
    }
        
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout <<"Duration: "<<duration.count() << std::endl;

    std::cout<<"Iterations: "<<iter<<" Threads: "<<num_threads<<std::endl;
    return 0;
}