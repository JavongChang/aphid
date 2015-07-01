/*
 *  HesperisIO.h
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <AllMath.h>
class HBase;
class HesperisFile;
class CurveGroup;
class HesperisIO {
public:
	static bool WriteTransforms(const MDagPathArray & paths, HesperisFile * file, const std::string & beheadName = "");
    static bool AddTransform(const MDagPath & path, HesperisFile * file, const std::string & beheadName = "");
    static bool WriteCurves(MDagPathArray & paths, HesperisFile * file, const std::string & parentName = "");
	static bool IsCurveValid(const MDagPath & path);
	static bool WriteMeshes(MDagPathArray & paths, HesperisFile * file);
    static MMatrix GetParentTransform(const MDagPath & path);
    static MMatrix GetWorldTransform(const MDagPath & path);
    static bool GetCurves(const MDagPath &root, MDagPathArray & dst);
    static bool ReadCurves(HesperisFile * file, MObject &target = MObject::kNullObj);
    static bool ReadTransforms(HBase * parent, MObject &target = MObject::kNullObj);
    static bool ReadCurves(HBase * parent, MObject &target = MObject::kNullObj);
    static bool CreateCurveGeos(CurveGroup * geos, MObject &target = MObject::kNullObj);
    static bool CreateACurve(Vector3F * pos, unsigned nv, MObject &target = MObject::kNullObj);
    static bool CheckExistingCurves(CurveGroup * geos, MObject &target = MObject::kNullObj);
    static bool FindNamedChild(MObject & dst, const std::string & name, MObject & oparent = MObject::kNullObj);
};