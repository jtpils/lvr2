#include <lvr2/io/PointBuffer2.hpp>
#include <lvr2/io/Timestamp.hpp>

#include <iostream>

namespace lvr2
{

PointBuffer2::PointBuffer2(floatArr points, size_t n)
{
    // Generate channel object pointer and add it
    // to channel map
    FloatChannelPtr point_data(new FloatChannel(n, 3, points));
    addFloatChannel(point_data, "points");

    // Save pointers
    m_points = point_data;
    m_numPoints = n;
}

PointBuffer2::PointBuffer2(floatArr points, floatArr normals, size_t n) : PointBuffer2(points, n)
{
    // Add normal data
    FloatChannelPtr normal_data(new FloatChannel(n, 3, points));
    addFloatChannel(normal_data, "normals");
}

void PointBuffer2::createFloatChannel(FloatChannelPtr data, std::string name, size_t n, unsigned width)
{

}

void PointBuffer2::createUCharChannel(UCharChannelPtr data, std::string name, size_t n, unsigned width)
{

}

void PointBuffer2::addFloatChannel(FloatChannelPtr data, std::string name)
{
    auto ret = m_floatChannels.insert(std::pair<std::string, FloatChannelPtr>(name, data));
    if(!ret.second )
    {
        std::cout << timestamp << "PointBuffer: Float channel '"
                  << name << "' already exist. Will not add data."
                  << std::endl;
    }
}

void PointBuffer2::addUCharChannel(UCharChannelPtr data, std::string name)
{
    auto ret = m_ucharChannels.insert(std::pair<std::string, UCharChannelPtr>(name, data));
    if(!ret.second)
    {
        std::cout << timestamp << "PointBuffer: UChar channel '"
                  << name << "' already exist. Will not add data."
                  << std::endl;
    }
}

bool PointBuffer2::hasUCharChannel(std::string name)
{
    auto it = m_ucharChannels.find(name);
    return !(it == m_ucharChannels.end());
}

bool PointBuffer2::hasFloatChannel(std::string name)
{
    auto it = m_floatChannels.find(name);
    return !(it == m_floatChannels.end());
}

unsigned PointBuffer2::ucharChannelWidth(std::string name)
{
    auto it = m_ucharChannels.find(name);
    if(it == m_ucharChannels.end())
    {
        return 0;
    }
    else
    {
        return it->second->width();
    }
}

unsigned PointBuffer2::floatChannelWidth(std::string name)
{
    auto it = m_floatChannels.find(name);
    if(it == m_floatChannels.end())
    {
        return 0;
    }
    else
    {
        return it->second->width();
    }
}

PointBuffer2::FloatProxy PointBuffer2::getFloatHandle(int idx, unsigned w)
{

}

PointBuffer2::UCharProxy PointBuffer2::getUCharHandle(int idx, unsigned w)
{

}

PointBuffer2::FloatProxy PointBuffer2::point(int idx)
{

}

PointBuffer2::FloatProxy PointBuffer2::normal(int idx)
{

}

}
