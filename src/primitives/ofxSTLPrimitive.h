/*
 * Copyright (c) 2014, Scott Kildall
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

#pragma once


#include "of3dPrimitives.h"
#include "ofxSTL.h"

class ofxSTLPrimitive : public of3dPrimitive {
    
public:
    
    void draw();
    void drawWireframe();

    void save(string filePath, bool asciiFormat = false);
    void save(ofxSTLExporter &stlExporter);
    
    // overridden
    void setPosition(float px, float py, float pz);
    
};

class ofxSTLBoxPrimitive : public ofxSTLPrimitive {
public:
    ofxSTLBoxPrimitive();
    
    // recalculates the vertices, should be called after setPostion()...or some other places
    void setVertices();
    
    void set( float _width, float _height, float _depth );
    void set( float size ); // all sides the same dimensions //
    
    //-- these each to reallocations, so if you need to change *all* dimensions, it's more efficeint to call set()
    void setWidth( float _width );
    void setHeight( float _height );
    void setDepth( float _depth );
    
protected:
    float width;
    float height;
    float depth;
    
    
    void addCubeFaceVertices(ofVec3f &v1, ofVec3f &v2, ofVec3f &v3, ofVec3f &v4);
};

#define DEFAULT_CYLINDER_RESOLUTION    (40)

class ofxSTLCylinderPrimitive : public ofxSTLPrimitive  {
public:
    ofxSTLCylinderPrimitive();
    
    void set( float _radius, float _length, int _resolution );
    
     //-- these each to reallocations, so if you need to change *all* dimensions, it's more efficeint to call set()
    void setResolution(int _resolution);
    void setDimensions( float _radius, float _length );
    void setRadius( float _radius );
    void setLength( float _length );

protected:
    void setVertices();
    void makeCapVertices(ofVec3f *vertices,float xPos, bool standardWinding);
    void makeLengths(ofVec3f *capVertices1, ofVec3f *capVertices2);
    ofVec3f *allocateCapVertices(float xPos);
    
    int resolution;
    float radius;
    float length;
};

class ofxSTLModelPrimitive : public ofxSTLPrimitive {
public:
    ofxSTLModelPrimitive(ofxSTLImporter &stlImporter);
};