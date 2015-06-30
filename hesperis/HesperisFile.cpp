/*
 *  HesperisFile.cpp
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "HesperisFile.h"
#include <AllHdf.h>
#include <HWorld.h>
#include <HCurveGroup.h>
#include <BaseBuffer.h>
#include <HTetrahedronMesh.h>
#include <HTriangleMesh.h>
#include <ATriangleMesh.h>
#include <ATetrahedronMesh.h>
#include <GeometryArray.h>
#include <SHelper.h>
#include <sstream>
#include <boost/format.hpp>
HesperisFile::HesperisFile() {}
HesperisFile::HesperisFile(const char * name) : HFile(name) 
{
	m_readComp = RNone;
	m_writeComp = WCurve;
}

HesperisFile::~HesperisFile() {}

void HesperisFile::setReadComponent(ReadComponent comp)
{ m_readComp = comp; }

void HesperisFile::setWriteComponent(WriteComponent comp)
{ m_writeComp = comp; }

bool HesperisFile::doWrite(const std::string & fileName)
{
	if(!HObject::FileIO.open(fileName.c_str(), HDocument::oReadAndWrite)) {
		setLatestError(BaseFile::FileNotWritable);
		return false;
	}
	
	HWorld grpWorld;
	grpWorld.save();
	
	switch (m_writeComp) {
		case WCurve:
			writeCurve();
			break;
		case WTetra:
			writeTetrahedron();
			break;
		case WTri:
			writeTriangle();
		default:
			break;
	}
	
	grpWorld.close();
	HObject::FileIO.close();
	
	std::cout<<" finished writing hesperis file at "<<grpWorld.modifiedTimeStr()<<"\n";
	
	return true;
}

bool HesperisFile::writeCurve()
{
	std::map<std::string, CurveGroup *>::iterator itcurve = m_curves.begin();
	for(; itcurve != m_curves.end(); ++itcurve) {
		openParents(itcurve->first);
        std::cout<<" write curve "<<worldPath(itcurve->first)
        <<"\n";
        
		HCurveGroup grpCurve(worldPath(itcurve->first));
		grpCurve.save(itcurve->second);
		grpCurve.close();
        closeParents();
	}
	return true;
}

bool HesperisFile::writeTetrahedron()
{
	std::stringstream sst;
	std::map<std::string, ATetrahedronMesh *>::iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		sst.str("");
		sst<<"/world/"<<it->first;
		std::cout<<" write tetrahedron mesh "<<sst.str()<<"\n";
		HTetrahedronMesh grp(sst.str());
		grp.save(it->second);
		grp.close();
	}
	return true;
}

bool HesperisFile::writeTriangle()
{
	std::stringstream sst;
	std::map<std::string, ATriangleMesh *>::iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		sst.str("");
		sst<<"/world/"<<it->first;
		std::cout<<" write triangle mesh "<<sst.str()<<"\n";
		HTriangleMesh grp(sst.str());
		grp.save(it->second);
		grp.close();
	}
	return true;
}

void HesperisFile::addCurve(const std::string & name, CurveGroup * data)
{ m_curves[name] = data; }

void HesperisFile::addTetrahedron(const std::string & name, ATetrahedronMesh * data)
{ m_terahedrons[name] = data; }

void HesperisFile::addTriangleMesh(const std::string & name, ATriangleMesh * data)
{ m_triangleMeshes[name] = data; }

bool HesperisFile::doRead(const std::string & fileName)
{
	if(!HFile::doRead(fileName)) return false;
	
	std::cout<<" reading curves file "<<fileName<<"\n";
	HWorld grpWorld;
	grpWorld.load();
	
	switch (m_readComp) {
		case RCurve:
			readCurve();
			break;
		case RTetra:
            listTetrahedron(&grpWorld);
			readTetrahedron();
			break;
		case RTri:
			listTriangle(&grpWorld);
			readTriangle();
		default:
			break;
	}
	
	grpWorld.close();
	
	std::cout<<" finished reading hesperis file modified at "<<grpWorld.modifiedTimeStr()<<"\n";
	
	return true;
}

bool HesperisFile::readCurve()
{
	bool allValid = true;
	std::stringstream sst;
	std::map<std::string, CurveGroup *>::iterator itcurve = m_curves.begin();
	for(; itcurve != m_curves.end(); ++itcurve) {
		sst.str("");
		sst<<"/world/"<<itcurve->first;
		std::cout<<" read curve "<<sst.str()<<"\n";
		HCurveGroup grpCurve(sst.str());
		if(!grpCurve.load(itcurve->second)) {
			std::cout<<" cannot load "<<sst.str();
			allValid = false;
		}
		
		grpCurve.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading curves.\n";

	return allValid;
}

bool HesperisFile::listTetrahedron(HBase * grp)
{
    std::vector<std::string > tetraNames;
	grp->lsTypedChild<HTetrahedronMesh>(tetraNames);
	
	std::vector<std::string>::const_iterator it = tetraNames.begin();
	for(;it!=tetraNames.end();++it) {
		addTetrahedron(*it, new ATetrahedronMesh);
	}
	return true;
}

bool HesperisFile::readTetrahedron()
{
    bool allValid = true;
	std::map<std::string, ATetrahedronMesh *>::iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		std::cout<<" read tetrahedron mesh "<<it->first<<"\n";
		HTetrahedronMesh grp(it->first);
		if(!grp.load(it->second)) {
			std::cout<<" cannot load "<<it->first;
			allValid = false;
		}
		grp.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading tetrahedrons.\n";

	return allValid;
}

bool HesperisFile::listTriangle(HBase * grp)
{
	std::vector<std::string > triNames;
	grp->lsTypedChild<HTriangleMesh>(triNames);
	
	std::vector<std::string>::const_iterator it = triNames.begin();
	for(;it!=triNames.end();++it) {
		addTriangleMesh(*it, new ATriangleMesh);
	}
	return true;
}

bool HesperisFile::readTriangle()
{
	bool allValid = true;
	std::map<std::string, ATriangleMesh *>::iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		std::cout<<" read triangle mesh "<<it->first<<"\n";
		HTriangleMesh grp(it->first);
		if(!grp.load(it->second)) {
			std::cout<<" cannot load "<<it->first;
			allValid = false;
		}
		grp.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading triangles.\n";

	return allValid;
}

void HesperisFile::extractTetrahedronMeshes(GeometryArray * dst)
{
    dst->create(m_terahedrons.size());
	unsigned i = 0;
	std::map<std::string, ATetrahedronMesh *>::const_iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		dst->setGeometry(it->second, i);
		i++;
	}
	dst->setNumGeometries(i);
}

void HesperisFile::extractTriangleMeshes(GeometryArray * dst)
{
	dst->create(m_triangleMeshes.size());
	unsigned i = 0;
	std::map<std::string, ATriangleMesh *>::const_iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		dst->setGeometry(it->second, i);
		i++;
	}
	dst->setNumGeometries(i);
}

void HesperisFile::openParents(const std::string & name)
{
    std::vector<std::string>  parents;
    SHelper::listParentNames(name, parents);
    if(parents.size() < 1) return;
    
    m_parentGroups.clear();
    
    std::vector<std::string>::const_iterator it = parents.begin();
    for(;it!=parents.end();++it) {
        std::string wp = worldPath(*it);
        std::cout<<"hes open "<<wp;
        m_parentGroups[wp] = new HBase(wp);

        
    }
}

void HesperisFile::closeParents()
{
    if(m_parentGroups.size() < 1) return;
    std::map<std::string, HBase * >::const_iterator it = m_parentGroups.begin();
    for(;it!=m_parentGroups.end(); ++it) {
        std::cout<<"hes close "<<it->first;
        it->second->close();
    }
    m_parentGroups.clear();
}

std::string HesperisFile::worldPath(const std::string & name)
{ return boost::str(boost::format("/world%1%") % name); }
//:~