#include "box.h"

Box::Box(){
  for(int i = 0; i < 8; i++) configuration[i] = false;
  for(int i = 0; i < 12; i++) indices[i] = -1;
  for(int i = 0; i < 27; i++) nb[i] = 0;
  voxelsize = 0.0;
  current_color[0] = current_color[1] = current_color[2] = 200;
  use_it = true;
}

Box::Box(const Box &o){
  for(int i = 0; i < 8; i++) configuration[i] = o.configuration[i];
  for(int i = 0; i < 12; i++) indices[i] = o.indices[i];
  for(int i = 0; i < 27; i++) nb[i] = 0;
  baseVertex = o.baseVertex;
  voxelsize = o.voxelsize;
  current_color[0] = current_color[1] = current_color[2] = 200;
  use_it = true;
}

Box::Box(Vertex v, float vs){
  for(int i = 0; i < 8; i++) configuration[i] = false;
  for(int i = 0; i < 12; i++) indices[i] = -1;
  for(int i = 0; i < 27; i++) nb[i] = 0;
  uchar r, g, b;
  r = b = 0; g = 200;
  baseVertex = ColorVertex(v, r, g, b);
  voxelsize = vs;
  current_color[0] = current_color[1] = current_color[2] = 200;
  use_it = true;
}

int Box::getIndex() const{
  int index = 0;
  for(int i = 0; i < 8; i++){
    if(configuration[i] > 0) index |= (1 << i);
  }
  return index;
}

void Box::getCorners(ColorVertex corners[]){

  uchar r, g, b;
  r = b = 0; g = 200;
  
  corners[0] = ColorVertex(baseVertex);

  corners[1] = ColorVertex(baseVertex.x + voxelsize,
					  baseVertex.y,
					  baseVertex.z,
					  r, g, b);

  corners[2] = ColorVertex(baseVertex.x + voxelsize,
					  baseVertex.y + voxelsize,
					  baseVertex.z,
					  r, g, b);

  corners[3] = ColorVertex(baseVertex.x,
					  baseVertex.y + voxelsize,
					  baseVertex.z,
					  r, g, b);

  corners[4] = ColorVertex(baseVertex.x,
					  baseVertex.y,
					  baseVertex.z + voxelsize,
					  r, g, b);

  corners[5] = ColorVertex(baseVertex.x + voxelsize,
					  baseVertex.y,
					  baseVertex.z + voxelsize,
					  r, g, b);

  corners[6] = ColorVertex(baseVertex.x + voxelsize,
					  baseVertex.y + voxelsize,
					  baseVertex.z + voxelsize,
					  r, g, b);

  corners[7] = ColorVertex(baseVertex.x,
					  baseVertex.y + voxelsize,
					  baseVertex.z + voxelsize,
					  r, g, b);
  
}

void Box::getIntersections(ColorVertex corners[],
					  DistanceFunction* df,
					  ColorVertex intersections[]){

  use_it = true;
  
  //bool interpolate = (df != 0);
  bool interpolate = true;
  float d1, d2;
  d1 = d2 = 0.0;

  float intersection;

  //Calc distances
  float distance[8];
  for(int i = 0; i < 8; i++){
    distance[i] = df->distance(corners[i]);
    if(configuration[i]) distance[i] = -distance[i];
  }

  //Front Quad
  intersection = calcIntersection(corners[0].x, corners[1].x, distance[0], distance[1], interpolate);
  intersections[0] = ColorVertex(intersection, corners[0].y, corners[0].z,
						   current_color[0], current_color[1], current_color[2]);

  intersection = calcIntersection(corners[1].y, corners[2].y, distance[1], distance[2], interpolate);
  intersections[1] = ColorVertex(corners[1].x, intersection, corners[1].z,
						   current_color[0], current_color[1], current_color[2]);

  intersection = calcIntersection(corners[3].x, corners[2].x, distance[3], distance[2], interpolate);
  intersections[2] = ColorVertex(intersection, corners[2].y, corners[2].z,
						   current_color[0], current_color[1], current_color[2]);
  
  intersection = calcIntersection(corners[0].y, corners[3].y, distance[0], distance[3], interpolate);
  intersections[3] = ColorVertex(corners[3].x, intersection, corners[3].z,
						   current_color[0], current_color[1], current_color[2]); 

  //Back Quad
  intersection = calcIntersection(corners[4].x, corners[5].x, distance[4], distance[5], interpolate);
  intersections[4] = ColorVertex(intersection, corners[4].y, corners[4].z,
						   current_color[0], current_color[1], current_color[2]);

  intersection = calcIntersection(corners[5].y, corners[6].y, distance[5], distance[6], interpolate);
  intersections[5] = ColorVertex(corners[5].x, intersection, corners[5].z,
						   current_color[0], current_color[1], current_color[2]);

  intersection = calcIntersection(corners[7].x, corners[6].x, distance[7], distance[6], interpolate);
  intersections[6] = ColorVertex(intersection, corners[6].y, corners[6].z,
						   current_color[0], current_color[1], current_color[2]);
  
  intersection = calcIntersection(corners[4].y, corners[7].y, distance[4], distance[7], interpolate);
  intersections[7] = ColorVertex(corners[7].x, intersection, corners[7].z,
						   current_color[0], current_color[1], current_color[2]);

  //Sides
  intersection = calcIntersection(corners[0].z, corners[4].z, distance[0], distance[4], interpolate);
  intersections[8] = ColorVertex(corners[0].x, corners[0].y, intersection,
						   current_color[0], current_color[1], current_color[2]); 

  intersection = calcIntersection(corners[1].z, corners[5].z, distance[1], distance[5], interpolate);
  intersections[9] = ColorVertex(corners[1].x, corners[1].y, intersection,
						   current_color[0], current_color[1], current_color[2]); 

  intersection = calcIntersection(corners[3].z, corners[7].z, distance[3], distance[7], interpolate);
  intersections[10] = ColorVertex(corners[3].x, corners[3].y, intersection,
						    current_color[0], current_color[1], current_color[2]); 
  
  intersection = calcIntersection(corners[2].z, corners[6].z, distance[2], distance[6], interpolate);
  intersections[11] = ColorVertex(corners[2].x, corners[2].y, intersection,
						    current_color[0], current_color[1], current_color[2]); 
								
  
}

float Box::calcIntersection(float x1, float x2,
					   float d1, float d2, bool interpolate){

  if(fabs(d1 - d2) < voxelsize)
    setColor(200, 200, 200);
  else{
    setColor(200, 0, 0);
  }

  (!interpolate){
    return x1 + 0.5 * (x2 - x1);
  } else {
    return x1 + (0.0 - d1) * (x2 - x1) / (d2-d1);
  }
  
}

int Box::getApproximation(int globalIndex, StaticMesh &mesh,
					 DistanceFunction* dst_func){

  if(use_it){
  ColorVertex corners[8];
  ColorVertex intersections[12];
  ColorVertex tmp_vertices[12];

  getCorners(corners);
  getIntersections(corners, dst_func, intersections);

  int index = getIndex();
  int edge_index = 0;
  int current_index;
  int vertex_count = 0;
  int tmp_indices[12];
  
  BaseVertex diff1, diff2;
  Normal normal;

  for(int a = 0; MCTable[index][a] != -1; a+= 3){
    for(int b = 0; b < 3; b++){

	 edge_index = MCTable[index][a + b];
	 current_index = -1;

	 //If current vertex hasn't got an vertex index yet,
	 //search the neighbour boxes using the NB-Table
	 if(indices[edge_index] == -1){
	   for(int i = 0; i < 3; i++){
		Box* current_neighbour = nb[NBTable[edge_index][i]];
		if(current_neighbour != 0){
		  //If index exists, use it
		  if(current_neighbour->indices[NBVertTable[edge_index][i]] != -1){
		    current_index = current_neighbour->indices[NBVertTable[edge_index][i]];
		  }
		}
	   }
	 }

	 //If no vertex index has been found, create a new one
	 //and add it to the mesh's vertex buffer
	 if(current_index == -1){
	   indices[edge_index] = globalIndex;
	   mesh.addVertex(intersections[edge_index]);
	   mesh.addNormal(Normal(0.0, 0.0, 0.0));

	   //Store new index on the correct locations of the neighbours
	   //index arrays
	   for(int i = 0; i < 3; i++){
		Box* current_neighbour = nb[NBTable[edge_index][i]];
		if(current_neighbour != 0){
		  if(current_neighbour->indices[NBVertTable[edge_index][i]] != -1){
		    //cerr << "##### Warning: Found already used index." << endl;
		  }
		  current_neighbour->indices[NBVertTable[edge_index][i]] = globalIndex;
		}
	   }
	   globalIndex++;
	 } else{
	   //save found index
	   indices[edge_index] = current_index;
	 }

	 mesh.addIndex(indices[edge_index]);

	 //Count and tmp-save generated vertices
	 tmp_vertices[vertex_count] = intersections[edge_index];
	 tmp_indices[vertex_count] = indices[edge_index];
	 vertex_count++;
    }

    //Calculate surface normal
    for(int i = 0; i < vertex_count - 2; i+= 3){
	 diff1 = tmp_vertices[i] - tmp_vertices[i+1];
	 diff2 = tmp_vertices[i+1] - tmp_vertices[i+2];
	 normal = diff1.cross(diff2);

	 //Interpolate with normals in mesh
	 for(int j = 0; j < 3; j++){
	   mesh.interpolateNormal(tmp_indices[i+j], normal);
	 }
    }
    
  }

 
  }
   return globalIndex;
}

