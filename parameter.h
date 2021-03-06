// Copyright 2020 José María Castelo Ares

// This file is part of MorphogenCV.

// MorphogenCV is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// MorphogenCV is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with MorphogenCV.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PARAMETER_H
#define PARAMETER_H

#include <vector>
#include <string>

class BoolParameter
{
public:
    std::string name;
    bool value;
    BoolParameter(std::string theName, bool theValue): name(theName), value(theValue){}
};

template <class T>
class OptionsParameter
{
public:
    std::string name;
    std::vector<std::string> valueNames;
    std::vector<T> values;
    T value;
    OptionsParameter(std::string theName, std::vector<std::string> theValueNames, std::vector<T> theValues, T theValue):
        name(theName),
        valueNames(theValueNames),
        values(theValues),
        value(theValue)
    {}
};

class IntParameter
{
public:
    std::string name;
    int value, min, max;
    bool isOdd;
    IntParameter(std::string theName, int theValue, int theMin, int theMax, bool odd):
        name(theName),
        value(theValue),
        min(theMin),
        max(theMax),
        isOdd(odd)
    {}
};

class DoubleParameter
{
public:
    std::string name;
    double value, min, max, inf, sup;
    DoubleParameter(std::string theName, double theValue, double theMin, double theMax, double theInf, double theSup):
        name(theName),
        value(theValue),
        min(theMin),
        max(theMax),
        inf(theInf),
        sup(theSup)
    {}
};

class KernelParameter
{
public:
    std::string name;
    std::vector<float> values;
    float min, max;
    bool normalize;
    KernelParameter(std::string theName, std::vector<float> theValues, float theMin, float theMax, bool norm):
        name(theName),
        values(theValues),
        min(theMin),
        max(theMax),
        normalize(norm)
    {}
};

#endif // PARAMETER_H
