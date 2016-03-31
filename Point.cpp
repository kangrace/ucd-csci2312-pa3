#include <iostream>
#include <cmath>     // sqrt, pow
#include <algorithm> //count
#include <cassert>
#include <sstream>
#include "Point.h"
#include "Exceptions.h"

using namespace std;
using namespace Clustering;

namespace Clustering
{
// static id generator
    unsigned int Point::__idGen = 0;
    const char Point::POINT_VALUE_DELIM = ',';

    void Point::rewindIdGen()
    {
        __idGen--;
    }

//constructors
    Point::Point(unsigned int dim)
    {
        if (dim == 0)
        {
            throw ZeroDimensionsEx();
        }

        //assign id and increase for next point
        __id = __idGen;
        ++__idGen;

        //assign dimension and create array
        __dim = dim;
        __values = new double[__dim];

        for (int i = 0; i < __dim; ++i)
        {
            __values[i] = 0;
        }
    }

    Point::Point(unsigned int dim, double *array)
    {
        // assign id and increase for next point
        __id = __idGen;
        ++__idGen;

        __dim = dim;
        __values = new double[__dim];
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] = array[i];
        }
    }

// the big three: copy constructor
    Point::Point(const Point &source)
    {
        if (source.__dim == 0)
        {
            throw ZeroDimensionsEx();
        }
        __dim = source.__dim;
        __id = source.__id;
        __values = new double[__dim];
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] = source.__values[i];
        }

    }
// 2: overloaded operator =
    Point &Point::operator=(const Point &right)
    {
        if (__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(__dim, right.__dim);
        }

        __dim = right.__dim;
        __id = right.__id;

        if (__values != nullptr)
            delete[] __values;
        __values = new double[__dim];
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] = right.__values[i];
        }

        return *this;
    }
// 3: destructor
    Point::~Point()
    {
        delete [] __values;
    }

//accessors and mutators
    int Point::getId() const
    {
        return __id;
    }

    unsigned int Point::getDims() const
    {
        return __dim;
    }


    void Point::setValue(unsigned int dim, double val)
    {
        if (dim >= __dim)
        {
            throw OutOfBoundsEx(__dim, dim);
        }
        __values[dim] = val;
    }

//get value of the dim
    double Point::getValue(unsigned int dim) const
    {
        if (dim >= __dim)
        {
            throw OutOfBoundsEx(__dim, dim);
        }
        return __values[dim];
    }


    double Point::distanceTo(const Point &point) const
    {
        if (__dim != point.__dim)
        {
            throw DimensionalityMismatchEx(__dim, point.__dim);
        }

        // make sure the dimension are the same to calculate distance
        if(__dim != point.__dim)
            return false;

        double answer = 0.0;
        for (unsigned int i = 0; i < __dim; ++i)
        {
            answer += pow(point.getValue(i) - getValue(i), 2);
        }
        return sqrt(answer);
    }

//overloaded operators

//members
    Point &Point::operator*=(double value)// p *= 6; p.operator*=(6);
    {
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] *= value;
        }
        return *this;
    }
    Point &Point::operator/=(double value)
    {
        for (int i = 0; i < __dim; ++i)
        {
            __values[i] /= value;
        }
        return *this;
    }

    const Point Point::operator*(double value) const // prevent (p1 * 2) = p2;
    {
        Point temp(*this);
        temp *= value;
        return temp;
    }
    const Point Point::operator/(double value) const // p3 = p2 / 2;
    {
        // can't divide by 0
        assert(value != 0);

        Point temp(*this);
        temp /= value;
        return temp;

    }

    double &Point::operator[](unsigned int index)
    {
        if (index >= __dim)
        {
            throw OutOfBoundsEx(__dim, index);
        }
        return __values[index];
    }

// friends

    Point &operator+=(Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        //lecture
        if(&left == &right)
        {
            return left *= 2;
        }
        else if (left.__dim == right.__dim)
        {
            for (int i = 0; i < left.__dim; ++i)
                left.__values[i] += right.__values[i];
        }
        return left;

    }

    Point &operator-=(Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        if(&left == &right)
        {
            return left -= left;
        }
        else if (left.__dim == right.__dim)
        {
            for (int i = 0; i < left.__dim; ++i)
                left.__values[i] -= right.__values[i];
        }
        return left;
    }

// +
    const Point operator+(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        //lecture
        Point temp(left);
        return temp += right;
    }

// -
    const Point operator-(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        // set temp to left and subtract right
        Point temp(left);
        return temp -= right;
    }

// ==
    bool operator==(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        // check to see if ids are equal
        if(left.getId() != right.getId())
            return false; // id are not equal cannot be true

        // check to see if values are equal
        for (unsigned int i = 0; i < max(left.__dim, right.__dim); ++i)
        {
            if (left.getValue(i) != right.getValue(i))
                return false;
        }
        return true;
    }

// !=
    bool operator!=(const Point &right, const Point &left)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        return !(left == right);
    }

// <
    bool operator<(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        // check for each if left < right
        for (unsigned int i = 0; i < max(left.__dim, right.__dim); ++i)
        {
            // check to see if the values are different
            // return left < right of the values
            if (left.getValue(i) != right.getValue(i))
                return (left.getValue(i) < right.getValue(i));
        }
    }

// >
    bool operator>(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        return (right < left);
    }

// <=
    bool operator<=(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        return !(left > right);
    }

// >=
    bool operator>=(const Point &left, const Point &right)
    {
        if (left.__dim != right.__dim)
        {
            throw DimensionalityMismatchEx(left.__dim, right.__dim);
        }

        return !(left < right);
    }

// <<
    ostream &operator<<(ostream &output, const Point &point)
    {
        int i = 0;
        for( ; i < point.__dim - 1; ++i)
            output << point.__values[i] << ", ";

        output << point.__values[i];
    }

// >>
    istream &operator>>(istream &input, Point &point)
    {
        string line;
        double num;

        for(int i = 0; i < point.__dim; ++i)
        {
            getline(input, line, ',');
            stringstream string(line);
            string >> num;
            point.__values[i] = num;
        }

        return input;
    }

}
