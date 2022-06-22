#ifndef PERLIN_H
#define PERLIN_H

#include "general.h"

class perlin{

    public:
        perlin(){
            randfloat = new double[point_count];
            for (int i=0;i<point_count;i++)
                randfloat[i] = random_double();
            
            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        ~perlin(){
            delete[] randfloat;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        double noise(const point3& p) const {
            int i = static_cast<int>(4*p.x()) & 256;
            int j = static_cast<int>(4*p.y()) & 256;
            int k = static_cast<int>(4*p.z()) & 256;

            return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        }

    private:
        static const int point_count = 256;
        double* randfloat;
        double* perm_x;
        double* perm_y;
        double* perm_z;

        static int* perlin_generate_perm(){
            auto p = new int[point_count];

            for (int i=0;i<point_count;i++)
                p[i] = i;
            
            permute(p, point_count);

            return p;
        }

        static void permute(int* p, int n){
            for (int i=n-1;i>=0;i--){
                int target = random_int(0, i);
                int temp = p[i];
                p[i] = p[target];
                p[target] = temp;
            }

        }
};

#endif