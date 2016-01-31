/*
 *  ModifyForest.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 2/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "ModifyForest.h"
#include <TriangleRaster.h>
#include <BarycentricCoordinate.h>

namespace sdb {

ModifyForest::ModifyForest() 
{ 
	m_raster = new TriangleRaster;
	m_bary = new BarycentricCoordinate;
	m_seed = rand() % 999999; 
}

ModifyForest::~ModifyForest() 
{
	delete m_raster;
	delete m_bary;
}

void ModifyForest::growOnGround(GrowOption & option)
{
	if(numActiveGroundFaces() < 1) return;
	std::map<Geometry *, Sequence<unsigned> * >::iterator it = activeGround()->geometryBegin();
	for(; it != activeGround()->geometryEnd(); ++it) {
		growOnFaces(it->first, it->second, geomertyId(it->first), option);
	}
}

void ModifyForest::growOnFaces(Geometry * geo, Sequence<unsigned> * components, 
						int geoId,
						GrowOption & option)
{
	GroundBind bind;
	ATriangleMesh * mesh = static_cast<ATriangleMesh *>(geo);
	Vector3F * p = mesh->points();
	components->begin();
	while(!components->end()) {
		if(option.m_alongNormal)
			option.m_upDirection = mesh->triangleNormal(components->key() );
		
		unsigned * tri = mesh->triangleIndices(components->key() );
		
		if(m_raster->create(p[tri[0]], p[tri[1]], p[tri[2]] ) ) {
			m_bary->create(p[tri[0]], p[tri[1]], p[tri[2]] );
			
			bind.setGeomComp(geoId, components->key() );
			growOnTriangle(m_raster, m_bary, bind, option);
		}
		components->next();
	}
}

void ModifyForest::growOnTriangle(TriangleRaster * tri, 
							BarycentricCoordinate * bar,
							GroundBind & bind,
							GrowOption & option)
{
	float sampleSize = plantSize(option.m_plantId) * .7f;
	int grid_x, grid_y;
	tri->gridSize(sampleSize, grid_x, grid_y);
	int numSamples = grid_x * grid_y;
	
	Vector3F *samples = new Vector3F[numSamples];
	char *hits = new char[numSamples];
	tri->genSamples(sampleSize, grid_x, grid_y, samples, hits);
	float scale;
	Matrix44F tm;
	for(int s = 0; s < numSamples; s++) {
		if(!hits[s]) continue;	
	
		Vector3F & pos = samples[s];
			
		randomSpaceAt(pos, option, tm, scale);
		float delta = option.m_marginSize + sampleSize * 1.4f * scale;
		if(closeToOccupiedPosition(pos, delta)) continue;
		
		bar->project(pos);
		bar->compute();
		if(!bar->insideTriangle()) continue;
		
		bind.m_w0 = bar->getV(0);
		bind.m_w1 = bar->getV(1);
		bind.m_w2 = bar->getV(2);
		
		addPlant(tm, bind, option.m_plantId);
	}
	delete[] samples;
	delete[] hits;
}

void ModifyForest::growAt(const Ray & ray, GrowOption & option)
{
	if(!intersectGround(ray) ) return;
	
	IntersectionContext * ctx = intersection();
	
	ATriangleMesh * mesh = static_cast<ATriangleMesh *>(ctx->m_geometry);
	unsigned component = ctx->m_componentIdx;
	if(option.m_alongNormal)
		option.m_upDirection = mesh->triangleNormal(component );
		
	Vector3F * p = mesh->points();
	unsigned * tri = mesh->triangleIndices(component );
	if(!m_raster->create(p[tri[0]], p[tri[1]], p[tri[2]] ) ) return;
	
	m_bary->create(p[tri[0]], p[tri[1]], p[tri[2]] );
	
	GroundBind bind;
	bind.setGeomComp(geomertyId(mesh), component );
	
	if(option.m_multiGrow) growOnTriangle(m_raster, m_bary, bind, option);
	else {
		Matrix44F tm;
		float scale;
		randomSpaceAt(ctx->m_hitP, option, tm, scale);
		float delta = option.m_marginSize + plantSize(option.m_plantId) * scale;
		if(closeToOccupiedPosition(ctx->m_hitP, delta)) return;
		
		m_bary->project(ctx->m_hitP);
		m_bary->compute();
		
		bind.m_w0 = m_bary->getV(0);
		bind.m_w1 = m_bary->getV(1);
		bind.m_w2 = m_bary->getV(2);
		
		addPlant(tm, bind, option.m_plantId);
	}
}

void ModifyForest::clearAt(const Ray & ray, float weight)
{
	if(numActivePlants() < 1 ) return;
	if(!intersectGround(ray) ) return;
	
	IntersectionContext * ctx = intersection();
	
	selection()->set(ctx->m_hitP, ctx->m_hitN, 4.f);
	selection()->calculateWeight();
	
	std::vector<int> idToClear;
	Array<int, PlantInstance> * arr = activePlants();
	arr->begin();
	while(!arr->end() ) {
		float wei = arr->value()->m_weight;
		if(wei > 1e-4f) { 
			if(m_pnoise.rfloat(m_seed) < weight) {
				idToClear.push_back(arr->key() );
				Plant * pl = arr->value()->m_reference;
				Vector3F pos = pl->index->t1->getTranslation();
				Coord3 c0 = grid()->gridCoord((const float *)&pos);
				Array<int, Plant> * cell = grid()->findCell(c0 );
				if(cell) {
					cell->remove(arr->key() );
					if(cell->isEmpty() )
						grid()->remove(c0);
				}
			}
			m_seed++;
		}
		arr->next();
	}
	
	std::vector<int>::const_iterator it = idToClear.begin();
	for(;it!=idToClear.end();++it) {
		arr->remove(*it);
	}
	idToClear.clear();
}

void ModifyForest::movePlant(const Ray & ray,
						const Vector3F & displaceNear, const Vector3F & displaceFar,
						const float & clipNear, const float & clipFar)
{
	if(numActivePlants() < 1 ) return;
	if(!intersectGround(ray) ) return;
	
	IntersectionContext * ctx = intersection();
	
	const float depth = ctx->m_hitP.distanceTo(ray.m_origin);
	const Vector3F disp = displaceNear 
					+ (displaceFar - displaceNear) * depth / (clipFar-clipNear);
	
	selection()->set(ctx->m_hitP, ctx->m_hitN, 4.f);
	selection()->calculateWeight();
	
	Vector3F pos, bindPos;
	Array<int, PlantInstance> * arr = activePlants();
	arr->begin();
	while(!arr->end() ) {
		float wei = arr->value()->m_weight;
		if(wei > 1e-4f) { 
			PlantData * plantd = arr->value()->m_reference->index;
			pos = plantd->t1->getTranslation() + disp * wei;
			
			bindToGround(plantd, pos, bindPos);
			
			plantd->t1->setTranslation(bindPos );
			displacePlantInGrid(arr->value() );
			
		}
		arr->next();
	}
}

void ModifyForest::moveWithGround()
{
	selection()->deselect();
	if(numPlants() < 1) return;
	if(ground()->isEmpty() ) return;
	
	grid()->begin();
	while(!grid()->end() ) {
		movePlantsWithGround(grid()->value() );
		grid()->next();
	}
	
	sdb::Array<int, sdb::PlantInstance> * arr = activePlants();
	if(arr->size() < 1) return;
	
	arr->begin();
	while(!arr->end() ) {
		displacePlantInGrid(arr->value() );
		arr->next();
	}
	selection()->deselect();
	updateGrid();
}

void ModifyForest::movePlantsWithGround(Array<int, Plant> * arr)
{
	Vector3F bindP, curP;
	arr->begin();
	while(!arr->end() ) {
		Plant * pl = arr->value();
		curP = pl->index->t1->getTranslation();
		GroundBind * bind = pl->index->t2;
		if(!getBindPoint(bindP, bind) ) {
			bindToGround(pl->index, curP, bindP);
		}
		
		if(curP.distance2To(bindP) > 1e-5f) {
/// select for later displace
			selection()->select(pl);
/// relocate
			pl->index->t1->setTranslation(bindP);
		}
			
		arr->next();
	}
}

void ModifyForest::randomSpaceAt(const Vector3F & pos, const GrowOption & option,
							Matrix44F & space, float & scale)
{
	*space.m(3, 0) =  pos.x;
	*space.m(3, 1) =  pos.y;
	*space.m(3, 2) =  pos.z;
	
	Vector3F up = option.m_upDirection;
	
	Vector3F side(1.f, 0.f, 0.f);
	if(up.x > 0.9f || up.x < -0.9f)
		side = Vector3F::YAxis;
		
	Vector3F front = side.cross(up);
	front.normalize();
	
	side = up.cross(front);
	side.normalize();
	
	scale = (m_pnoise.rfloat(m_seed)) * (option.m_maxScale - option.m_minScale)
				+ option.m_minScale;
	m_seed++;
	
	Vector3F vside(side.x, side.y, side.z);
	Vector3F vaxis(up.x, up.y, up.z);
	float ang = (m_pnoise.rfloat(m_seed) - 0.5f) * option.m_rotateNoise * 2.f * 3.14f;
	m_seed++;
	
	vside.rotateAroundAxis(vaxis, ang);
	
	side = vside;
	front = side.cross(up);
	front.normalize();

	*space.m(0, 0) = side.x * scale;
	*space.m(0, 1) = side.y * scale;
	*space.m(0, 2) = side.z * scale;
	*space.m(1, 0) = up.x * scale;
	*space.m(1, 1) = up.y * scale;
	*space.m(1, 2) = up.z * scale;
	*space.m(2, 0) = front.x * scale;
	*space.m(2, 1) = front.y * scale;
	*space.m(2, 2) = front.z * scale;
}

}