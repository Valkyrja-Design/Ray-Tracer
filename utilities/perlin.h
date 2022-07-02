#ifndef PERLIN_H
#define PERLIN_H

#include "general.h"

class perlin{

    public:
        perlin(){
            randvec = new vec3[point_count];
            for (int i=0;i<point_count;i++)
                randvec[i] = unit_vector(vec3 :: random(-1, 1));
            
            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        ~perlin(){
            delete[] randvec;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        // double noise(const point3& p) const {
        //     int i = static_cast<int>(4*p.x()) & 255;
        //     int j = static_cast<int>(4*p.y()) & 255;
        //     int k = static_cast<int>(4*p.z()) & 255;

        //     return (randvec[perm_x[i] ^ perm_y[j] ^ perm_z[k]].length_squared());
        // }

        double noise(const point3& p) const {
            auto u = p.x() - floor(p.x());
            auto v = p.y() - floor(p.y());
            auto w = p.z() - floor(p.z());

            int i = static_cast<int>(floor(p.x()));
            int j = static_cast<int>(floor(p.y()));
            int k = static_cast<int>(floor(p.z()));

            vec3 c[2][2][2];

            for (int di=0;di<2;i++){
                for (int dj=0;dj<2;dj++){
                    for (int dk=0;dk<2;dk++){
                        c[di][dj][dk] = randvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];
                    }
                }
            }

            return perlin_interpolation(c, u, v, w);
        }

        double turbulance(const point3& p, int depth = 7) const {
            auto sum = 0.0;
            auto temp_p = p;
            auto weight = 1.0;

            for (int i=0;i<depth;i++){
                sum += weight*noise(p);
                weight *= 0.5;
                temp_p *= 2;
            }

            return fabs(sum);
        }

    private:
        static const int point_count = 256;
        vec3* randvec;
        int* perm_x;
        int* perm_y;
        int* perm_z;

        static int* perlin_generate_perm(){
            auto p = new int[point_count];

            for (int i=0;i<point_count;i++)
                p[i] = i;
            
            permute(p, point_count);

            return p;
        }

        static void permute(int* p, int n){
            for (int i=n-1;i>0;i--){
                int target = random_int(0, i);
                int temp = p[i];
                p[i] = p[target];
                p[target] = temp;
            }

        }

        static double perlin_interpolation(vec3 c[2][2][2], double u, double v, double w){
            auto uu = u*u*(3-2*u);
            auto vv = v*v*(3-2*v);
            auto ww = w*w*(3-2*w);
            auto accum = 0.0;

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        vec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))
                               * (j*vv + (1-j)*(1-vv))
                               * (k*ww + (1-k)*(1-ww))
                               * dot(c[i][j][k], weight_v);
                    }

            return accum;
        }
};

#endif