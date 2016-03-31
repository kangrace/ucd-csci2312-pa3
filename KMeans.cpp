#include <iostream>
#include <fstream>
#include <cassert>
#include <limits>
#include "KMeans.h"

#include "Exceptions.h"

namespace Clustering
{
// constructor
    KMeans::KMeans(unsigned int dim, unsigned int k, std::string filename, unsigned int maxIter)
    {
        __dimensionality = dim;
        __numIter = 0;
        __numNonempty = 0;
        __maxIter = maxIter;
        __numIter = 0;

        __k = k;
        if (k == 0)
        {
            throw ZeroClustersEx();
        }

        __clusters = new Cluster *[__k];
        for (int i = 0; i < __k; ++i)
        {
            __clusters[i] = new Cluster(__dimensionality);
        }

        __initCentroids = new Point *[__k];
        for (int i = 0; i < __k; ++i)
        {
            __initCentroids[i] = new Point(__dimensionality);
        }

        __iFileName = filename;
        std::ifstream inFile(__iFileName);
        if (inFile.is_open())
        {
            inFile >> *__clusters[0];
            inFile.close();
        }
        else
        {
            delete[] __clusters;
            delete[] __initCentroids;
            throw (DataFileOpenEx(__iFileName));
        }

        (*__clusters[0]).pickCentroids(k, __initCentroids);

        for (int i = 0; i < __k; ++i)
        {
            __clusters[i]->centroid.set((*__initCentroids[i]));
            __clusters[i]->centroid.setValid(true);
            if (__clusters[i]->centroid.get().getValue(0) != std::numeric_limits<double>::max())
                __numNonempty++;
        }
    }

// destructor
    KMeans::~KMeans()
    {
        for (int i = 0; i < __k; ++i)
        {
            delete __initCentroids[i];
        }
        delete[] __initCentroids;

        for (int i = 0; i < __k; ++i)
        {
            delete __clusters[i];
        }
        delete[] __clusters;
    }

// accessors

    unsigned int KMeans::getMaxIter()
    {
        return __maxIter;
    }

    unsigned int KMeans::getNumIters()
    {
        return __numIter;
    }

    unsigned int KMeans::getNumNonemptyClusters()
    {
        return __numNonempty;
    }

    unsigned int KMeans::getNumMovesLastIter()
    {
        return __numMovesLastIter;
    }

// elemenet access
    Cluster &KMeans::operator[](unsigned int u)
    {
        return (*__clusters[u]);
    }

    const Cluster &KMeans::operator[](unsigned int u) const
    {
        return (*__clusters[u]);
    }

 //write  out results
    std::ostream &operator<<(std::ostream &os, const KMeans &kmeans)
    {
        // gave up
        return os;
    }


//clustering function

    void KMeans::run()
    {
        assert(__clusters[0]->getSize() > 0);

        unsigned int moves = 100;
        unsigned int iter = 0;
        unsigned int nonempty = 0;
        unsigned int closest = 0;

        while(moves > 0 && iter < __maxIter)
        {
            moves = 0; //restart count for the new iteration

            //find closest centroid(i) of c
            for (int i = 0; i < __k; ++i)
            {
                // point (j)
                for (int j = 0; j < __clusters[i] -> getSize(); ++j)
                {
                   double distance = (*__clusters[i])[j].distanceTo(__clusters[closest] -> centroid.get());

                    for (unsigned int k = 0; k < __k; ++k)
                    {
                        if (distance > (*__clusters[i])[j].distanceTo(__clusters[k] -> centroid.get()))
                        {
                            distance = (*(__clusters[i]))[j].distanceTo(__clusters[k]->centroid.get());
                            closest = k;
                        }
                    }

                    if (i != closest)
                    {
                        Cluster::Move move((*__clusters[i])[j], (*__clusters[i]), *(__clusters[closest]));
                        move.perform();
                        --j;
                        moves ++;
                    }
                }
            }

            //recompute all invalidated centroids
            // iter++
            for (int i = 0; i < __k; ++i)
            {
                if(!__clusters[i]->centroid.isValid())
                    __clusters[i]->centroid.compute();
            }

            __numIter = iter;
            __numMovesLastIter = moves;

            for (int i = 0; i < __k; ++i)
            {
                if ((*__clusters[i]).getSize() > 0)
               {
                  ++nonempty;
              }
            }

            __numNonempty = nonempty;

        }
    }
}
