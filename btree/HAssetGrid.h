#ifndef HASSETGRID_H
#define HASSETGRID_H

/*
 *  HAssetGrid.h
 *  julia
 *
 *  Created by jian zhang on 3/31/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  grid contains a number of element asset
 *  bounded by a cube
 *  /asset_1
 *  /asset_2
 *  ...
 *  /.tree
 */

#include <HBase.h>
#include <HElemAsset.h>
#include <KdEngine.h>
#include <VoxelGrid.h>
#include <HNTree.h>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

namespace aphid {
namespace sdb {

template <typename T, typename Tv>
class HAssetGrid : public HBase, public Entity {
	
	boost::scoped_ptr<T> m_activeAsset;
	boost::scoped_ptr<HNTree<Voxel, KdNode4 > > m_tree;
	
public:
	HAssetGrid(const std::string & name, Entity * parent);
	virtual ~HAssetGrid();
	
	bool insert(const Tv * v);
	bool insert(const std::string & name);
	bool flush();
	int numElements();
	void remove(const std::string & name);
	void buildTree(const BoundingBox & worldBox);
	bool isEmpty();
	HNTree<Voxel, KdNode4 > * loadTree();
	void getBBox(BoundingBox * dst);
	
protected:

private:

};

template <typename T, typename Tv>
HAssetGrid<T, Tv>::HAssetGrid(const std::string & name, Entity * parent) :
HBase(name), Entity(parent),
m_activeAsset(0),
m_tree(0)
{}

template <typename T, typename Tv>
HAssetGrid<T, Tv>::~HAssetGrid()
{}

template <typename T, typename Tv>
int HAssetGrid<T, Tv>::numElements()
{
	int sum = 0;
	int an;
/// count all asset n element
	std::vector<std::string > assetNames;
	lsTypedChildWithIntAttrVal<HElemBase>(assetNames,
											".elemtyp", Tv::ShapeTypeId );
	std::vector<std::string >::const_iterator it = assetNames.begin();
	for(;it != assetNames.end();++it) {
		T ass(*it);
		an = 0;
		ass.readIntAttr(".nelem", &an );
		sum += an;
		ass.close();
	}
	return sum;
}

template <typename T, typename Tv>
bool HAssetGrid<T, Tv>::insert(const std::string & name)
{
/// open named asset
	std::cout<<"\n hasset insert "<<(childPath(name) );
	m_activeAsset.reset(new T(childPath(name) ) );
	return true;
}

template <typename T, typename Tv>
bool HAssetGrid<T, Tv>::insert(const Tv * v)
{
	m_activeAsset->insert(*v);
	return true;
}

template <typename T, typename Tv>
bool HAssetGrid<T, Tv>::flush()
{
	if(m_activeAsset) {
		m_activeAsset->save();
		m_activeAsset->close();
	}
	return true;
}

template <typename T, typename Tv>
void HAssetGrid<T, Tv>::remove(const std::string & name)
{
	if(hasTypedChildWithIntAttrVal<T>(name, ".elemtyp", Tv::ShapeTypeId ) ) {
		std::cout<<"\n hasset remove "<<(childPath(name) );
		T c(childPath(name) );
		c.clear();
		c.close();
	}
}

template <typename T, typename Tv>
void HAssetGrid<T, Tv>::buildTree(const BoundingBox & worldBox)
{
	int numVoxels = 0;
	if(!hasNamedAttr(".nvx") )
	    addIntAttr(".nvx", 1);
	writeIntAttr(".nvx", &numVoxels );
	
	sdb::VectorArray<Tv> src;
	std::vector<std::string > assetNames;
	lsTypedChildWithIntAttrVal<HElemBase>(assetNames,
											".elemtyp", Tv::ShapeTypeId );
	std::vector<std::string >::const_iterator it = assetNames.begin();
	for(;it != assetNames.end();++it) {
		T ass(*it);
		if(ass.load() ) {
			ass.extract(&src);
		}
		ass.close();
	}
	
	if(src.size() < 1) return;
			
	std::cout<<"\n hassetgrid extract "<<src.size()<<" "<<Tv::GetTypeStr()
			<<" in world bbox "<<worldBox;
	BoundingBox rootBox(0.f, 0.f, 0.f,
						worldBox.distance(0), 
						worldBox.distance(1), 
						worldBox.distance(2));
	
	KdNTree<Tv, KdNode4 > ptree;
	
	TreeProperty::BuildProfile bf;
	bf._maxLeafPrims = 64;
	bf._doTightBox = false;
	
	KdEngine engine;
	engine.buildTree<Tv, KdNode4, 4>(&ptree, &src, rootBox, &bf);
	
	VoxelGrid<KdNTree<Tv, KdNode4 >, Tv > vgd;
	vgd.create(&ptree, rootBox, 8);
	
	numVoxels = vgd.numVoxels();
	
	if(numVoxels < 1) {
		std::cout<<"\n  warning hassetgrid "<<worldBox<<" is empty";
		return;
	}
	
	writeIntAttr(".nvx", &numVoxels );
	
	HNTree<Voxel, KdNode4 > vtree(boost::str(boost::format("%1%/.tree") % pathToObject() ) );
	bf._maxLeafPrims = 32;
	bf._doTightBox = true;
    
	BoundingBox vxBox(0.f, 0.f, 0.f,
						1024.f, 1024.f, 1024.f);
						
	engine.buildTree<Voxel, KdNode4, 4>(&vtree, vgd.voxelsR(), vxBox, &bf);
	vtree.setRelativeTransform(worldBox);
	vtree.save();
	vtree.close();
	
	BoundingBox tb;
	vtree.getWorldTightBox(&tb);
	if(!hasNamedAttr(".bbx") )
	    addFloatAttr(".bbx", 6);
	writeFloatAttr(".bbx", (float *)&tb );
	std::cout<<"\n hassetgrid world tight bbox "<<tb;
	// H5Fflush(HObject::FileIO.fFileId, H5F_SCOPE_LOCAL);
}

template <typename T, typename Tv>
bool HAssetGrid<T, Tv>::isEmpty()
{
	int nv = 0;
	readIntAttr(".nvx", &nv );
	return nv < 1; 
}

template <typename T, typename Tv>
HNTree<Voxel, KdNode4 > * HAssetGrid<T, Tv>::loadTree()
{
	if(!m_tree.get() ) {
		m_tree.reset(new HNTree<Voxel, KdNode4 >(boost::str(boost::format("%1%/.tree") % pathToObject() ) ) );
		m_tree->load();
		m_tree->close();
	}
	return m_tree.get();
}

template <typename T, typename Tv>
void HAssetGrid<T, Tv>::getBBox(BoundingBox * dst)
{ readFloatAttr(".bbx", (float *)dst ); }

}
}
#endif        //  #ifndef HASSETGRID_H
