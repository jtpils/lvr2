/*
 * PolygonFusion.hpp
 *
 *  Created on: 05.03.2014
 *      Author: dofeldsc
 */

#ifndef POLYGONFUSION_HPP_
#define POLYGONFUSION_HPP_

// Boost includes for the Fusion
#include <boost148/geometry.hpp>
#include <boost148/geometry/geometries/point_xy.hpp>
#include <boost148/geometry/geometries/polygon.hpp>
#include <boost148/geometry/domains/gis/io/wkt/wkt.hpp>

#include "geometry/Vertex.hpp"
#include "geometry/Normal.hpp"
#include "geometry/PolygonRegion.hpp"
#include "geometry/PolygonMesh.hpp"
#include <vector>
#include <map>

// lvr includes
#include "io/Timestamp.hpp"

namespace lvr
{

/**
 * @brief Class for Polygonfusion
 *
 *	0.5) Wait and store all given meshes
 *	 1) put polyregions into bins according to labels
 *	 2) in these bins, find "co-planar" polyregions -> same plane (Δ)
 *	 3) transform these polygons into 2D space (see spuetz fusion)
 *	 4) apply boost::geometry::union_ for these polygons
 *	 5) transform resulting 2D polygon back into 3d space (inverse of step 3)
 *	 6) place resulting 3D polygon in response mesh
 *	 7) insert all left overs into response.mesh
 *
 */

template<typename VertexT, typename NormalT>
class PolygonFusion {
public:
	typedef PolygonRegion<VertexT, NormalT> PolyRegion;
	typedef std::map<std::string, std::vector<PolyRegion> > PolyRegionMap;
	typedef std::vector<PolygonMesh<VertexT, NormalT> > PolyMesh;

	/**
	 * @brief standard constructor
	 */
	PolygonFusion();


	/**
	 * destructor
	 */
	~PolygonFusion();


	/**
	 * @brief add a new PolygonMesh to the Fusion (store it in the container)
	 */
	void addFusionMesh(PolygonMesh<VertexT, NormalT> mesh);


	/**
	 * @brief Fuse all the Meshes (Polygons) in the container
	 *
	 * 		At first, only the Polygons with the same label
	 *
	 * 	 1) put polyregions into bins according to labels
	 *	 2) in these bins, find "co-planar" polyregions -> same plane (Δ)
	 *	 3) transform these polygons into 2D space (see spuetz fusion)
	 *	 4) apply boost::geometry::union_ for these polygons
	 *	 5) transform resulting 2D polygon back into 3d space (inverse of step 3)
	 *	 6) place resulting 3D polygon in response mesh
	 *	 7) insert all left overs into response.mesh
	 *
	 * @return returns false, if something went wrong
	 */
	bool doFusion();


private:
	/**
	 * @brief This function tests if these two Polygons are planar
	 *
	 * @return true, if these Polygons are planar
	 */
	bool isPlanar(PolyRegion a, PolyRegion b);

	// Vector for all data (Polygonmesh)
	PolyRegionMap	m_polyregionmap;

	PolyMesh 		m_meshes;

	double			m_distance_threshold;



	Timestamp ts;
};

} // End of namespace lvr
#include "PolygonFusion.tcc"
#endif /* POLYGONFUSION_HPP_ */
