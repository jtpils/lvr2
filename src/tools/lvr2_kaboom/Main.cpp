/**
 * Copyright (c) 2018, University Osnabrück
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University Osnabrück nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL University Osnabrück BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Main.cpp
 *
 *  Created on: Aug 9, 2013
 *      Author: Thomas Wiemann
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <cstdio>
#include <fstream>
#include <utility>
#include <iterator>
using namespace std;

#include <boost/filesystem.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <Eigen/Dense>

#include "Options.hpp"
#include <lvr2/io/Timestamp.hpp>
#include <lvr2/io/ModelFactory.hpp>
#include <lvr2/io/IOUtils.hpp>

#ifdef LVR2_USE_PCL
#include <lvr2/reconstruction/PCLFiltering.hpp>
#endif

#define BUF_SIZE 1024

using namespace lvr2;

namespace qi = boost::spirit::qi;

const kaboom::Options* options;

// This is dirty
bool lastScan = false;
ofstream scanPosesOut("scanpositions.txt");

// ModelPtr filterModel(ModelPtr p, int k, float sigma)
// {
//     if(p)
//     {
//         if(p->m_pointCloud)
//         {
// #ifdef LVR2_USE_PCL
//             PCLFiltering filter(p->m_pointCloud);
//             cout << timestamp << "Filtering outliers with k=" << k << " and sigma=" << sigma << "." << endl;
//             size_t original_size = p->m_pointCloud->getNumPoints();
//             filter.applyOutlierRemoval(k, sigma);
//             PointBufferPtr pb( filter.getPointBuffer() );
//             ModelPtr out_model( new Model( pb ) );
//             cout << timestamp << "Filtered out " << original_size - out_model->m_pointCloud->getNumPoints() << " points." << endl;
//             return out_model;
// #else
//             cout << timestamp << "Can't create a PCL Filter without PCL installed." << endl;
//             return NULL;
// #endif

//         }
//     }
//     return NULL;
// }

size_t writePly(ModelPtr model, std::fstream& out)
{
    size_t n_ip = model->m_pointCloud->numPoints();
    size_t n_colors = n_ip;
    unsigned w_color;

    floatArr arr = model->m_pointCloud->getPointArray();

    ucharArr colors = model->m_pointCloud->getColorArray(w_color);

    if(colors)
    {
        if(n_colors != n_ip)
        {
            std::cout << timestamp << "Numbers of points and colors needs to be identical" << std::endl;
            return 0;
        }

        for(int a = 0; a < n_ip; a++)
        {
            // x y z
            out.write((char*) (arr.get() + (3 * a)), sizeof(float) * 3);

            // r g b
            out.write((char*) (colors.get() + (w_color * a)), sizeof(unsigned char) * 3);
        }
    }
    else
    {
        // simply write whole points array
        out.write((char*) arr.get(), sizeof(float) * n_ip * 3);
    }

    return n_ip;

}

size_t writePlyHeader(std::ofstream& out, size_t n_points, bool colors)
{
    out << "ply" << std::endl;
    out << "format binary_little_endian 1.0" << std::endl;

    out << "element point " << n_points << std::endl;
    out << "property float32 x" << std::endl;
    out << "property float32 y" << std::endl;
    out << "property float32 z" << std::endl;

    if(colors)
    {
        out << "property uchar red" << std::endl;
        out << "property uchar green" << std::endl;
        out << "property uchar blue" << std::endl;
    }
    out << "end_header" << std::endl;
}

void addScanPosition(Eigen::Matrix4d& transform)
{
    Eigen::Vector4d translation = transform.rightCols<1>();
    if(scanPosesOut.good())
    {
        std::cout << timestamp << "Exporting scan position @ "
                  << translation[0] << " "
                  << translation[1] << " "
                  << translation[2] << std::endl;

        scanPosesOut << translation[0] << " "
                     << translation[1] << " "
                     << translation[2] << std::endl;
    }
}


void processSingleFile(boost::filesystem::path& inFile)
{
    cout << timestamp << "Processing " << inFile << endl;

    cout << timestamp << "Reading point cloud data from file " << inFile.filename().string() << "." << endl;

    // TODO: Make explicit call to ASCII-IO of necessery to account for
    // attribute column ordering!
    ModelPtr model = ModelFactory::readModel(inFile.string());

    if(0 == model)
    {
        throw "ERROR: Could not create Model for: ";
    }

    if(options->getOutputFile() != "")
    {
        char frames[1024];
        char pose[1024];
        char dat[1024];
        sprintf(frames, "%s/%s.frames", inFile.parent_path().c_str(), inFile.stem().c_str());
        sprintf(pose, "%s/%s.pose", inFile.parent_path().c_str(), inFile.stem().c_str());
        sprintf(dat, "%s/%s.dat", inFile.parent_path().c_str(), inFile.stem().c_str());


        boost::filesystem::path framesPath(frames);
        boost::filesystem::path posePath(pose);
        boost::filesystem::path datPath(dat);

        size_t reductionFactor = lvr2::getReductionFactor(inFile, options->getTargetSize());

        if(options->transformBefore())
        {
            transformAndReducePointCloud(model, reductionFactor, options->coordinateTransform());
        }


        if(boost::filesystem::exists(datPath))
        {
            std::cout << timestamp << "Getting transformation from dat: " << datPath << std::endl;
            Eigen::Matrix4d transform = getTransformationFromDat(datPath);
            transformPointCloud(model, transform);
            addScanPosition(transform);
        }
        else if(boost::filesystem::exists(framesPath))
        {
            std::cout << timestamp << "Getting transformation from frame: " << framesPath << std::endl;
            Eigen::Matrix4d transform = getTransformationFromFrames(framesPath);
            transformPointCloud(model, transform);
            addScanPosition(transform);
        }
        else if(boost::filesystem::exists(posePath))
        {

            std::cout << timestamp << "Getting transformation from pose: " << posePath << std::endl;
            Eigen::Matrix4d transform = getTransformationFromPose(posePath);
            transformPointCloud(model, transform);
            addScanPosition(transform);
        }

        if(!options->transformBefore())
        {
            transformAndReducePointCloud(model, reductionFactor, options->coordinateTransform());
        }

        static size_t points_written = 0;


        if(options->getOutputFormat() == "ASCII" || options->getOutputFormat() == "")
        {
            // Merge (only ASCII)
            std::ofstream out;

            /* If points were written we want to append the next scans, otherwise we want an empty file */
            if(points_written != 0)
            {
                out.open(options->getOutputFile().c_str(), std::ofstream::out | std::ofstream::app);
            }
            else
            {
                out.open(options->getOutputFile().c_str(), std::ofstream::out | std::ofstream::trunc);
            }

            points_written += writePointsToStream(model, out);

            out.close();
        }
        else if(options->getOutputFormat() == "PLY")
        {
            char tmp_file[1024];

            sprintf(tmp_file, "%s/tmp.ply", inFile.parent_path().c_str());

            std::fstream tmp;

            if(points_written != 0)
            {
                tmp.open(tmp_file, std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);
            }
            else
            {
                tmp.open(tmp_file, std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);
            }

            if(tmp.is_open())
            {
                points_written += writePly(model, tmp);
            }
            else
            {
                std::cout << "could not open " << tmp_file << std::endl;
            }

            if(true == lastScan)
            {
                std::ofstream out;

                // write the header -> open in text_mode
                out.open(options->getOutputFile().c_str(), std::ofstream::out | std::ofstream::trunc);
                // check if we have color information
                if(model->m_pointCloud->hasColors())
                {
                    writePlyHeader(out, points_written, true);
                }
                else
                {
                    writePlyHeader(out, points_written, false);
                }

                out.close();

                // determine size of the complete binary blob
                tmp.seekg(0, std::fstream::end);
                size_t blob_size = tmp.tellg();
                tmp.seekg(0, std::fstream::beg);

                // open the actual output file for binary blob write
                out.open(options->getOutputFile(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);

                char buffer[BUF_SIZE];

                while(blob_size)
                {
                    if(blob_size < BUF_SIZE)
                    {
                        // read the rest from tmp file (binary blob)
                        tmp.read(buffer, blob_size);
                        // write the rest to actual ply file
                        out.write(buffer, blob_size);

                        blob_size -= blob_size;
                    }
                    else
                    {
                        // reading from tmp file (binary blob)
                        tmp.read(buffer, BUF_SIZE);
                        // write to actual ply file
                        out.write(buffer, BUF_SIZE);

                        blob_size -= BUF_SIZE;
                    }
                }

                out.close();
                tmp.close();

                std::remove(tmp_file);

                std::cout << timestamp << "Wrote " << points_written << " points." << std::endl;
            }
            else
            {
                tmp.close();
            }

        }
    }
    else
    {
        if(options->getOutputFormat() == "")
        {
            // Infer format from file extension, convert and write out
            char name[1024];
            char frames[1024];
            char pose[1024];
            char framesOut[1024];
            char poseOut[1024];

            model = ModelFactory::readModel(inFile.string());
            sprintf(frames, "%s/%s.frames", inFile.parent_path().c_str(), inFile.stem().c_str());
            sprintf(pose, "%s/%s.pose", inFile.parent_path().c_str(), inFile.stem().c_str());
            sprintf(framesOut, "%s/%s.frames", options->getOutputDir().c_str(), inFile.stem().c_str());
            sprintf(poseOut, "%s/%s.pose", options->getOutputDir().c_str(), inFile.stem().c_str());
            sprintf(name, "%s/%s", options->getOutputDir().c_str(), inFile.filename().c_str());

            boost::filesystem::path framesPath(frames);
            boost::filesystem::path posePath(pose);

            // Transform the frames
            if(boost::filesystem::exists(framesPath))
            {
                std::cout << timestamp << "Transforming frame: " << framesPath << std::endl;
                Eigen::Matrix4d transformed = transformFrame(getTransformationFromFrames(framesPath), options->coordinateTransform());
                writeFrame(transformed, framesOut);
            }

            ofstream out(name);
            transformAndReducePointCloud(model, getReductionFactor(inFile, options->getTargetSize()), options->coordinateTransform());
            size_t points_written = writePointsToStream(model, out);

            out.close();

            cout << "Wrote " << points_written << " points to file " << name << endl;
         }
        else if(options->getOutputFormat() == "SLAM")
        {
            std::cerr << "I am sorry! This is not implemented yet" << std::endl;
        }
        else
        {
            std::cerr << "I am sorry! This is not implemented yet" << std::endl;
        }
    }
}

template <typename Iterator>
bool parse_filename(Iterator first, Iterator last, int& i)
{

    using qi::lit;
    using qi::uint_parser;
    using qi::parse;
    using boost::spirit::qi::_1;
    using boost::phoenix::ref;

    uint_parser<unsigned, 10, 3, 3> uint_3_d;

    bool r = parse(
            first,                          /*< start iterator >*/
            last,                           /*< end iterator >*/
            ((lit("scan")|lit("Scan")) >> uint_3_d[ref(i) = _1])   /*< the parser >*/
            );

    if (first != last) // fail if we did not get a full match
        return false;
    return r;
}

bool sortScans(boost::filesystem::path firstScan, boost::filesystem::path secScan)
{
    std::string firstStem = firstScan.stem().string();
    std::string secStem   = secScan.stem().string();

    int i = 0;
    int j = 0;

    bool first = parse_filename(firstStem.begin(), firstStem.end(), i);
    bool sec = parse_filename(secStem.begin(), secStem.end(), j);

    if(first && sec)
    {
        return (i < j);
    }
    else
    {
        // this causes non valid files being at the beginning of the vector.
        if(sec)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}



int main(int argc, char** argv) {

    // Parse command line arguments
    options = new kaboom::Options(argc, argv);

    // Check if a specific input file was given. If so, convert the single
    // file according to .pose or .frame information
    if(options->getInputFile() != "")
    {
        boost::filesystem::path inputFile(options->getInputFile());
        if(boost::filesystem::exists((inputFile)))
        {
            processSingleFile(inputFile);
            exit(0);
        }
        else
        {
            cout << timestamp << "File '" << options->getInputFile() << "' does not exist." << endl;
            exit(-1);
        }
    }

    // If an input directory is given, enter directory parsing mode
    boost::filesystem::path inputDir(options->getInputDir());
    boost::filesystem::path outputDir(options->getOutputDir());

    // Check input directory
    if(!boost::filesystem::exists(inputDir))
    {
        cout << timestamp << "Error: Directory " << options->getInputDir() << " does not exist" << endl;
        exit(-1);
    }

    // Check if output dir exists
    if(!boost::filesystem::exists(outputDir))
    {
        cout << timestamp << "Creating directory " << options->getOutputDir() << endl;
        if(!boost::filesystem::create_directory(outputDir))
        {
            cout << timestamp << "Error: Unable to create " << options->getOutputDir() << endl;
            exit(-1);
        }
    }

    boost::filesystem::path abs_in = boost::filesystem::canonical(inputDir);
    boost::filesystem::path abs_out = boost::filesystem::canonical(outputDir);

    if(abs_in == abs_out)
    {
        cout << timestamp << "Error: We think it is not a good idea to write into the same directory. " << endl;
        exit(-1);
    }

    // Create director iterator and parse supported file formats
    boost::filesystem::directory_iterator end;
    vector<boost::filesystem::path> v;
    for(boost::filesystem::directory_iterator it(inputDir); it != end; ++it)
    {
        std::string ext =	it->path().extension().string();
        if(ext == ".3d" || ext == ".ply" || ext == ".txt" )
        {
            v.push_back(it->path());
        }
    }

    // Sort entries
    sort(v.begin(), v.end(), sortScans);

    vector<float>	 		merge_points;
    vector<unsigned char>	merge_colors;

    int j = -1;
    for(vector<boost::filesystem::path>::iterator it = v.begin(); it != v.end(); ++it)
    {
        int i = 0;

        std::string currFile = (it->stem()).string();
        bool p = parse_filename(currFile.begin(), currFile.end(), i);

        //if parsing failed terminate, this should never happen.
        if(!p)
        {
            std::cerr << timestamp << "ERROR " << " " << *it << " does not match the naming convention" << std::endl;
            break;
        }

        // check if the current scan has the same numbering like the previous, this should not happen.
        if(i == j)
        {
            std::cerr << timestamp << "ERROR " << *std::prev(it) << " & " << *it << " have identical numbering" << std::endl;
            break;
        }

        // check if the scan is in the range which should be processed
        if(i >= options->getStart()){
            // when end is default(=0) process the complete vector
            if(0 == options->getEnd() || i <= options->getEnd())
            {
                try
                {
                    // This is dirty and bad designed.
                    // We need to know when we advanced to the last scan
                    // for ply merging. Which originally was not planned.
                    // Two cases end option set or not.
                    if((i  == options->getEnd()) || std::next(it, 1) == v.end())
                    {
                        lastScan = true;
                    }
                    processSingleFile(*it);
                    std::cout << " finished" << std::endl;
                }
                catch(const char* msg)
                {
                    std::cerr << timestamp << msg << *it << std::endl;
                    break;
                }
                j = i;
            }
            else
            {
                break;
            }
        }

    }

    cout << timestamp << "Program end." << endl;
    delete options;
    return 0;
}
