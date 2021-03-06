/*
 * Copyright (c) Scott Kildall
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * http://creativecommons.org/licenses/LGPL/2.1/
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ofxSTLPrimitive.h"


void ofxSTLPrimitive::draw() {
    ofGetCurrentRenderer()->draw(*this, OF_MESH_FILL);
}

void ofxSTLPrimitive::drawWireframe() {
    ofGetCurrentRenderer()->draw(*this, OF_MESH_WIREFRAME);
}


void ofxSTLPrimitive::setPosition(float px, float py, float pz) {
    ofVec3f oldPos = getPosition();
    ofNode::setPosition(px,py,pz);
    ofVec3f newPos = getPosition();
    
    ofMesh *m = getMeshPtr();
    
    for( int i = 0; i < m->getNumVertices(); i++ ) {
        ofVec3f v = m->getVertex(i);
        v.set( newPos.x + (oldPos.x+v.x), newPos.y +(oldPos.y+v.y), newPos.z + (oldPos.z+v.z));
        
        m->setVertex(i, v );
    }
}

//-----------------------------------

ofxSTLBoxPrimitive::ofxSTLBoxPrimitive() {
    width = 1;
    height = 1;
    depth = 1;
    setVertices();
}


void ofxSTLBoxPrimitive::set( float _width, float _height, float _depth ) {
    width = _width;
    height = _height;
    depth = _depth;
    setVertices();
}

void ofxSTLBoxPrimitive::set( float size ) {
    set(size,size,size);
    
}

void ofxSTLBoxPrimitive::setWidth( float _width ) {
    set(_width,height,depth);
}

void ofxSTLBoxPrimitive::setHeight( float _height ) {
    set(width,_height,depth);
}

void ofxSTLBoxPrimitive::setDepth( float _depth ) {
    set(width,height,_depth);
}


void ofxSTLPrimitive::save(string filePath, bool asciiFormat) {
    ofxSTLExporter stlExporter;
    
    stlExporter.beginModel("stlOutput");
    
    save(stlExporter);
    
    stlExporter.useASCIIFormat(asciiFormat);
    stlExporter.saveModel(filePath);
}

void ofxSTLPrimitive::save(ofxSTLExporter &stlExporter) {
    vector<ofVec3f>& verts = mesh->getVertices();
	if( verts.size() %3 != 0 ) {
        cout << "ofxSTLPrimitive() error: vertices must be divisible by 3" << "\n";
        return;
    }
    
    ofVec3f emptyNormal(0,0,0);
    
    for(unsigned int i = 0; i < verts.size(); i+=3){
        stlExporter.addTriangle(verts[i], verts[i+1], verts[i+2], emptyNormal);
    }
}

//-----------------------------


// from stored width, height, depth, will set the vertices
void ofxSTLBoxPrimitive::setVertices() {
    ofVec3f p = getPosition();
    
    ofVec3f v1(p.x-width/2, p.y-height/2, p.z-depth/2);
    ofVec3f v2(p.x+width/2, p.y-height/2, p.z-depth/2);
    ofVec3f v3(p.x+width/2, p.y-height/2, p.z+depth/2);
    ofVec3f v4(p.x-width/2, p.y-height/2, p.z+depth/2);
    
    ofVec3f v5(p.x-width/2, p.y+height/2, p.z+depth/2);
    ofVec3f v6(p.x-width/2, p.y+height/2, p.z-depth/2);
    ofVec3f v7(p.x+width/2, p.y+height/2, p.z-depth/2);
    ofVec3f v8(p.x+width/2, p.y+height/2, p.z+depth/2);
    
    mesh->clearVertices();
    
    // each vertex shows up 3 times
    addCubeFaceVertices(v1,v2,v3,v4);     // next vertex = v3
    addCubeFaceVertices(v2,v7,v8,v3);     // next vertex = v8
    addCubeFaceVertices(v8,v7,v6,v5);     // next vertex = v8
    addCubeFaceVertices(v8,v5,v4,v3);     // next vertex = v4
    addCubeFaceVertices(v5,v6,v1,v4);     // next vertex = v1
    addCubeFaceVertices(v6,v7,v2,v1);     // next vertex = v1
}

//-- last vertex will always be at v3
void ofxSTLBoxPrimitive::addCubeFaceVertices(ofVec3f &v1, ofVec3f &v2, ofVec3f &v3, ofVec3f &v4) {
    mesh->addVertex(v1);
    mesh->addVertex(v2);
    mesh->addVertex(v3);
    
    mesh->addVertex(v4);
    mesh->addVertex(v1);
    mesh->addVertex(v3);
}

//-----------------------------

ofxSTLCylinderPrimitive::ofxSTLCylinderPrimitive() {
    resolution = DEFAULT_CYLINDER_RESOLUTION;
    radius = 6;
    length = 200;
    
    setVertices();
}

void ofxSTLCylinderPrimitive::set( float _radius, float _length, int _resolution ) {
    resolution = _resolution;
    radius = _radius;
    length = _length;
    setVertices();
}


void ofxSTLCylinderPrimitive::setResolution(int _resolution) {
    resolution = _resolution;
    setVertices();
}

//-- these each to reallocations, so if you need to change *all* dimensions, it's more efficeint to call set()
void ofxSTLCylinderPrimitive::setDimensions( float _radius, float _length ) {
    radius = _radius;
    length = _length;
    setVertices();
}

void ofxSTLCylinderPrimitive::setRadius( float _radius ) {
    radius = _radius;
    setVertices();
}

void ofxSTLCylinderPrimitive::setLength( float _length ) {
    length = _length;
    setVertices();
}

// move all vertices here
//-- (1) do offset calculations with 2D hexagons at -x/2 and x/2, use these formulas here:
//-- http://www.physicsforums.com/showthread.php?t=317274
//-- (2) try changing the resolution
//-- (3) add center point and do STL output
//--        -- using triangle-winding for 1 face
//--        -- using triangle-winding for 2 face
//-- (4) add triangle-winding for the rectangular face
//--        -- store temporary array of vertices for this
void ofxSTLCylinderPrimitive::setVertices() {
    mesh->clearVertices();
    ofVec3f p = getPosition();
    
    ofVec3f *capVertices1 = allocateCapVertices(p.x-length/2);
    ofVec3f *capVertices2 = allocateCapVertices(p.x+length/2);
    
    // Make the caps, such that the normals face the outside
    makeCapVertices(capVertices1, p.x-length/2, true);
    makeCapVertices(capVertices2, p.x+length/2, false);
    makeLengths(capVertices1,capVertices2);
    
    delete [] capVertices1;
    delete [] capVertices2;
}

void ofxSTLCylinderPrimitive::makeCapVertices(ofVec3f *vertices, float xPos, bool standardWinding) {
    ofVec3f p = getPosition();
    
    ofVec3f cv;     // center vertiex
    cv.x = xPos;
    cv.y = p.y;
    cv.z = p.z;
    
    // Add triangulated, from center point
    ofVec3f v1;
    ofVec3f v2;
    for( int i = 0; i < resolution; i++ ) {
        if( i == 0 )
            v1 = *(vertices+resolution-1);
        else
            v1 = *(vertices+i-1);
        
        v2 = *(vertices+i);
        
        mesh->addVertex(cv);
        
        if( standardWinding ) {
            mesh->addVertex(v1);
            mesh->addVertex(v2);
        }
        else {
            mesh->addVertex(v2);
            mesh->addVertex(v1);
        }
    }
}

void ofxSTLCylinderPrimitive::makeLengths(ofVec3f *capVertices1, ofVec3f *capVertices2) {
    
    ofVec3f v1;
    ofVec3f v2;
    ofVec3f v3;
    ofVec3f v4;
    for( int i = 0; i < resolution; i++ ) {
        if( i == 0 ) {
            v1 = *(capVertices1+resolution-1);
            v3 = *(capVertices2+resolution-1);
        }
        else {
            v1 = *(capVertices1+i-1);
            v3 = *(capVertices2+i-1);
        }
        v2 = *(capVertices1+i);
        v4 = *(capVertices2+i);
        
        // first tri
        mesh->addVertex(v1);
        mesh->addVertex(v3);
        mesh->addVertex(v2);
        
        // second tri
        mesh->addVertex(v3);
        mesh->addVertex(v4);
        mesh->addVertex(v2);
    }
}

//-- stored in
ofVec3f * ofxSTLCylinderPrimitive::allocateCapVertices(float xPos) {
    ofVec3f p = getPosition();
    ofVec3f *vertices = new ofVec3f[resolution];
    
    // Load up vertices
    for( int i = 0; i < resolution; i++ ) {
        (vertices+i)->z = p.z + (radius * cos( (2*PI/resolution) * i ));
        (vertices+i)->y = p.y + (radius * sin( (2*PI/resolution) * i ));
        (vertices+i)->x = xPos;
    }
    
    return vertices;
}

//---------------------------------
ofxSTLModelPrimitive::ofxSTLModelPrimitive(ofxSTLImporter &stlImporter) {
    vector<ofxSTLFacet>& facets = stlImporter.getFacets();
    int numFacets = facets.size();
    for(int i=0; i<numFacets; i++){
        getMeshPtr()->addVertex(facets[i].vert1);
        getMeshPtr()->addVertex(facets[i].vert2);
        getMeshPtr()->addVertex(facets[i].vert3);
    }
}

//-----------------------------

//-- unimplemented / old draw code

/*
 if( bWireframe ) {
 mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
 
 }
 else {
 mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
 ofFill();
 ofSetColor(225);
 
 // Load into the GPU
 //        vbo.draw(GL_TRIANGLES, 0, vbo.getNumVertices());
 
 }
 
 
 mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
 ofFill();
 ofSetColor(225);
 
 
 //mesh.drawFaces();
 mesh.draw();
 */

//-- unimplemented / old tetrahedron code

/*
 
 // init
 tetraFacets = new ofxSTLFacet[4];

 
 // delete

 if(tetraFacets)
 delete [] tetraFacets;

 
// other
 void datum::initTetrahedron() {
 float offset = 50;
 
 float x1 = -s/2 + x;
 float x2 = s/2 + x;
 float y1 = -s/2 + y;
 float y2 = s/2 + y;
 float z1 = -((s/2) / 1.41421) + z; // square root of 2
 float z2 = (s/2) / 1.41421 + z;
 
 ofVec3f v1(x1,y,z1);
 ofVec3f v2(x2,y,z1);
 ofVec3f v3(x,y1,z2);
 ofVec3f v4(x,y2,z2);
 
 ofVec3f n(0,0,0);
 
 tetraFacets[0].normal = n;
 tetraFacets[0].vert1 = v1;
 tetraFacets[0].vert2 = v2;
 tetraFacets[0].vert3 = v3;
 
 tetraFacets[1].normal = n;
 tetraFacets[1].vert1 = v2;
 tetraFacets[1].vert2 = v4;
 tetraFacets[1].vert3 = v3;
 
 tetraFacets[2].normal = n;
 tetraFacets[2].vert1 = v3;
 tetraFacets[2].vert2 = v4;
 tetraFacets[2].vert3 = v1;
 
 tetraFacets[3].normal = n;
 tetraFacets[3].vert1 = v4;
 tetraFacets[3].vert2 = v2;
 tetraFacets[3].vert3 = v1;
 
 mesh.addVertex(v1);
 mesh.addVertex(v2);
 mesh.addVertex(v3);
 mesh.addVertex(v1);
 mesh.addVertex(v4);
 mesh.addVertex(v2);
 mesh.addVertex(v3);
 mesh.addVertex(v4);
 
 }
 */

/*
 
 void datum::stlOutTetrahedron(ofxSTLExporter &stlExporter) {
 int numFacets = 4;
 for(int i=0; i<numFacets; i++)
 stlExporter.addTriangle(tetraFacets[i].vert1, tetraFacets[i].vert2, tetraFacets[i].vert3, tetraFacets[i].normal);
 }
 */

//-- Tetra-out
/*

void ofApp::initTetrahedron() {
     float offset = 50;
     
     float edgeLength = 200;
     float x1 = -edgeLength/2;
     float x2 = edgeLength/2;
     float y1 = -edgeLength/2;;
     float y2 = edgeLength/2;
     float z1 = -((edgeLength/2) / 1.41421); // square root of 2
     float z2 = (edgeLength/2) / 1.41421;
     
     ofVec3f v1(x1,0,z1);
     ofVec3f v2(x2,0,z1);
     ofVec3f v3(0,y1,z2);
     ofVec3f v4(0,y2,z2);
     ofVec3f n(0,0,0);
     
     tetraFacets = new ofxSTLFacet[4];
     tetraFacets[0].normal = n;
     tetraFacets[0].vert1 = v1;
     tetraFacets[0].vert2 = v2;
     tetraFacets[0].vert3 = v3;
     
     tetraFacets[1].normal = n;
     tetraFacets[1].vert1 = v2;
     tetraFacets[1].vert2 = v4;
     tetraFacets[1].vert3 = v3;
     
     tetraFacets[2].normal = n;
     tetraFacets[2].vert1 = v3;
     tetraFacets[2].vert2 = v4;
     tetraFacets[2].vert3 = v1;
     
     tetraFacets[3].normal = n;
     tetraFacets[3].vert1 = v4;
     tetraFacets[3].vert2 = v2;
     tetraFacets[3].vert3 = v1;
     
     tetraMesh.addVertex(v1);
     tetraMesh.addVertex(v2);
     tetraMesh.addVertex(v3);
     tetraMesh.addVertex(v1);
     tetraMesh.addVertex(v4);
     tetraMesh.addVertex(v2);
     tetraMesh.addVertex(v3);
     tetraMesh.addVertex(v4);
     */




