
#pragma once

#include <string>
#include <string.h>
#include <stdio.h>
#include "Util.h"

class TiXmlElement;

class XmlGetAttribute
{
public:
    XmlGetAttribute(){}
    ~XmlGetAttribute(){}
public:
    static std::string  GetAttribute(TiXmlElement * element, std::string name, std::string defaultValue);
    static int  GetAttribute(TiXmlElement * element, std::string name, int defaultValue);

};

