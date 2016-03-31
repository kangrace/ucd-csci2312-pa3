#include <sstream>
#include <limits>
#include <cassert>
#include <algorithm>
#include "Cluster.h"
#include "Exceptions.h"


using namespace std;

namespace Clustering
{
    // LNode constructor
    LNode::LNode(const Point &p, LNodePtr n) : point(p), next(n)
    {
    }

    unsigned int Cluster::__idGenerator = 0;

    void Cluster::__del()
    {
        // until point's next is != null
        while (__size > 0)
        {
            LNodePtr current = __points;
            for (int i = 0; i < __size; ++i)
            {
                current = current -> next;
            }
            --__size;
            delete current;
        }
        __points = nullptr;
        centroid.setValid(false);
    }

    void Cluster::__cpy(LNodePtr pts)
    {
        __del();
        LNodePtr current = pts;
        while (current != nullptr)
        {
            add(current -> point);
            current = current -> next;
        }
    }

    bool Cluster::__in(const Point &p) const
    {
        LNodePtr current = __points;
        for (int i = 0; i < __size; ++i)
        {
            if (current -> point == p)
                return true; // point is in cluster
            current = current -> next;
        }
        return false; // not in cluster
    }

// centroid
    Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) : __dimensions(c.getDimensionality()), __p(d), __c(c)
    {
       __valid == false;
    }

// getters and setters
    const Point Cluster::Centroid::get() const
    {
        return __p;
    }

    void Cluster::Centroid::set(const Point &p)
    {
        __p = p;
        __valid == true;
    }

    bool Cluster::Centroid::isValid() const
    {
        return __valid;
    }

    void Cluster::Centroid::setValid(bool valid)
    {
        __valid = valid;
    }

// functions
    void Cluster::Centroid::compute()
    {
        if (!__valid)
        {
            __valid = true;
            if (__c.__points == nullptr)
            {
                toInfinity();
                return;
            }


            LNodePtr current = __c.__points;
            int size = 0;
            __p = Point(__c.getDimensionality());
            while (current != nullptr)
            {
                __p += current->point / __c.__size;
                current = current->next;
                size++;
            }

            assert(size == __c.__size);
        }
        return;

    }

    bool Cluster::Centroid::equal(const Point &point) const
    {
        if (point.getDims() != __p.getDims())
        {
            throw (DimensionalityMismatchEx(__p.getDims(), point.getDims()));
        }
        return (point == __p);
    }

    void Cluster::Centroid::toInfinity()
    {
        for (int i = 0; i < __p.getDims(); ++i)
        {
            __p[i] = std::numeric_limits<double>::max();
        }
    }

    Cluster::Cluster(unsigned int d) : centroid(d, *this)
    {
        __size = 0;
        __points = nullptr;
        __dimensionality = d;
    }

// the big three: copy constructor, overloaded =, destructor
    Cluster::Cluster(const Cluster &source) : centroid(source.__dimensionality, source)
    {
        __id = source.__id;
        __size = 0;
        __points = nullptr;
        __dimensionality = source.__dimensionality;
        __cpy(source.__points);
    }

    Cluster &Cluster::operator=(const Cluster &source)
    {
       __id = source.__id;
        __cpy(source.__points);
        __size = source.__size;
        return *this;
    }

    Cluster::~Cluster()
    {
        LNodePtr current;
        while(__points != nullptr)
        {
            current = __points;
            __points = current -> next;

            delete current;
        }
    }

// getters and setters
    unsigned int Cluster::getSize() const
    {
        return __size;
    }

    unsigned int Cluster::getDimensionality() const
    {
        return __dimensionality;
    }
    unsigned int Cluster::getId() const
    {
        return __id;
    }

// set functions
    void Cluster::add(const Point &point)
    {
        if (point.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, point.getDims());
        }

        if (!contains(point))
        {
            LNodePtr insert;
            insert = new LNode(point, __points);
            centroid.setValid(false);
            __size++;

            if (__points == nullptr)
            {
                __points = insert;
            }

            else if (point < __points->point)
            {
                insert -> next = __points;
                __points = insert;
            }

            else
            {
                LNodePtr left = __points;
                LNodePtr right = __points->next;
                while (true)
                {
                    if (right == nullptr || right -> point > point)
                    {
                        left -> next = insert;
                        insert -> next = right;
                        break;
                    }
                    else
                    {
                        left = right;
                        right = right -> next;
                    }
                }

            }

        }
        return;
    }

    const Point &Cluster::remove(const Point &point)
    {
        if (point.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, point.getDims());
        }

        if (contains(point))
        {
            LNodePtr prev;
            LNodePtr next;

            prev = nullptr;
            next = __points;

            while (next != nullptr)
            {
                if (next->point == point)
                {
                    if (prev == nullptr)
                    {
                        __points = next -> next;
                        delete next;
                        --__size;
                        break;
                    }
                    else
                    {
                        prev -> next = next -> next;
                        delete next;
                        --__size;
                        break;
                    }

                }
                prev = next;
                next = next -> next;
            }

        }
        return point;
    }

    bool Cluster::contains(const Point &p) const
    {
        if (p.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, p.getDims());
        }

        LNodePtr next = __points;
        while (next != nullptr)
        {
            // use id to see if point is int he list
            if (next->point.getId() == p.getId())
                return true;
                // search the next ids
            else next = next->next;

        }
        //not in list
        return false;

    }

// centroid function
    void Cluster::pickCentroids(unsigned int k, Point **pointArray)
    {
        if (k >= __size)
        {
            for (int i =  0; i < k; ++i)
            {
                if (i < __size)
                {
                    *pointArray[i] = (*this)[i];
                }
                else
                {
                    for (int j = 0; j < pointArray[i] -> getDims(); ++j)
                    {
                        pointArray[i] -> setValue(j, std::numeric_limits<double>::max());
                    }
                }
            }
            return;
        }
        else
        {
            for (int i = 0; i < k; ++i)
            {
                if (i < __size)
                {
                    *pointArray[i] = (*this)[i];
                }
            }
        }
    }


// overloaded operators

    // Subscript
    const Point &Cluster::operator[](unsigned int index) const
    {
        if (__size == 0)
        {
            throw EmptyClusterEx();
        }

        if (index >= __size)
        {
            throw OutOfBoundsEx(__size, index);
        }

        if(__size > 0)
        {
            LNodePtr current = __points;

            for (int i = 0; i < index && current -> next != nullptr; ++i)
                current = current->next;
            return current->point;

        }

    }

    // compound assignment (point arguments)
    Cluster &Cluster::operator+=(const Point &point)
    {
        if (point.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, point.getDims());
        }

        add(point);
        return *this;
    }

    Cluster &Cluster::operator-=(const Point &point)
    {
        if (point.getDims() != __dimensionality)
        {
            throw DimensionalityMismatchEx(__dimensionality, point.getDims());
        }

        remove(point);
        return *this;
    }

    // compound assignment (cluster argument)
    // union
    Cluster &Cluster::operator+=(const Cluster &right)
    {
        for (int i = 0; i < right.getSize(); ++i)
        {
            // if it doesn't contain the point, add it
            if(!contains(right[i]))
                add(right[i]);
        }
        return *this;
    }

    // difference
    Cluster &Cluster::operator-=(const Cluster &right)
    {
        for (int i = 0; i < right.__size; ++i)
        {
            // if it does contain the point, remove it
            if (contains(right[i]))
                remove(right[i]);
        }
        return *this;
    }

    // friends : IO
    ostream &operator<<(ostream &output, const Cluster &cluster)
    {
        LNodePtr current = cluster.__points;
        if (cluster.__size == 0)
        {
            return output;
        }

        else
        {
            while (current != nullptr)
            {
                Point point = current -> point;
                output << point << ", " << cluster.getId() << std::endl;

                current = current -> next;
            }
        }
        return output;
    }


    istream &operator>>(istream &input, Cluster &cluster)
    {
        cluster.__del();
        std::string string;
        Point line(cluster.getDimensionality());
        while (getline(input, string))
        {
            std::istringstream ss(string);

            Point line(cluster.getDimensionality());
            try
            {
                ss >> line;
                cluster.add(line);
            }
            catch (DimensionalityMismatchEx ex)
            {
                line.rewindIdGen();
            }
        }
    }

    // Friends: Comparison
    bool operator==(const Cluster &left, const Cluster &right)
    {
        if (right.__dimensionality != left.__dimensionality)
        {
            throw DimensionalityMismatchEx(right.__dimensionality, left.__dimensionality);
        }
        // compare size
        if (left.__size != right.__size)
            return false;

        // compare values
        for (int i = 0; i < left.__size; ++i)
        {
            if (left[i] != right[i])
                return false;
        }
        // values are the same
        return true;
    }

    bool operator!=(const Cluster &left, const Cluster &right)
    {
        return !(left == right);
    }

    // Friends: Arithmetic (Cluster and Point)
    const Cluster operator+(const Cluster &left, const Point &right)
    {
        Cluster temp(left);
        temp += right;
        return temp;
    }

    const Cluster operator-(const Cluster &left, const Point &right)
    {
        Cluster temp(left);
        temp -= right;
        return temp;
    }

    // Friends: Arithmetic (two Clusters)
    // union
    const Cluster operator+(const Cluster &left, const Cluster &right)
    {
        Cluster temp(left.getDimensionality());
        temp += right;
        temp += left;
        return temp;
    }

    // (asymmetric) difference
    const Cluster operator-(const Cluster &left, const Cluster &right)
    {
        Cluster temp(left);
        temp -= right;
        return temp;
    }

// move
    Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to) : __p(p), __from(from), __to(to)
    {
    }

    void Cluster::Move::perform()
    {
        __to.add(__p);
        __from.remove(__p);
    }
}