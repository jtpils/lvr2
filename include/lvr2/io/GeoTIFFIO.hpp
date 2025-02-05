//
// Created by ndettmer on 07.02.19.
//

#ifndef GEOTIFFIO_HPP
#define GEOTIFFIO_HPP

#include <gdal_priv.h>
#include <opencv2/opencv.hpp>
#include <string>

namespace lvr2
{
/**
 * @brief class providing and encapsulating GDAL GeoTIFF I/O functions
 * @author Niklas Dettmer <ndettmer@uos.de>
 */
class GeoTIFFIO
{
public:
    /**
     * @param filename filename of output GeoTIFF file
     * @param cols number of columns / width of the image
     * @param rows number of rows / length of the image
     * @param bands number of bands
     */
    GeoTIFFIO(std::string filename, int cols, int rows, int bands);

    /**
     * @param filename
     */
    GeoTIFFIO(std::string filename);

    /**
     * @brief Writing given band into open GeoTIFF file
     * @param mat cv::Mat containing the band data
     * @param band number of band to be written
     * @return standard C++ return value
     */
    int writeBand(cv::Mat *mat, int band);

    /**
     * @return width of dataset in number of pixels
     */
    int getRasterWidth();

    /**
     * @return height of dataset in number of pixels
     */
    int getRasterHeight();

    /**
     * @return number of bands of dataset
     */
    int getNumBands();

    /**
     * @param band_index index of the band to be read
     * @return indexed band of the dataset as cv::Mat *
     */
    cv::Mat *readBand(int band_index);

    ~GeoTIFFIO();

private:
    GDALDataset *m_gtif_dataset;
    GDALDriver *m_gtif_driver;
    int m_cols, m_rows, m_bands;
};
}


#endif //GEOTIFFIO_HPP
