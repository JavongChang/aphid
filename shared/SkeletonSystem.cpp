#include "SkeletonSystem.h"
#include <SkeletonJoint.h>
#include <SkeletonPose.h>
SkeletonSystem::SkeletonSystem() 
{
}

SkeletonSystem::~SkeletonSystem() 
{
    clear();
}

void SkeletonSystem::clear()
{
    std::vector<SkeletonJoint *>::iterator it = m_joints.begin();
    for(; it != m_joints.end(); ++it) delete (*it); 
    m_joints.clear();
}

void SkeletonSystem::addJoint(SkeletonJoint * j)
{
    m_joints.push_back(j);
    j->setIndex(numJoints() - 1);
}

unsigned SkeletonSystem::numJoints() const
{
    return m_joints.size();
}
   
SkeletonJoint * SkeletonSystem::joint(unsigned idx) const
{
    return m_joints[idx];
}

SkeletonJoint * SkeletonSystem::jointByIndex(unsigned idx) const
{
	std::vector<SkeletonJoint *>::const_iterator it = m_joints.begin();
	for(; it != m_joints.end(); ++it) {
		if((*it)->index() == idx) return *it;
	}
	return 0;
}

SkeletonJoint * SkeletonSystem::selectJoint(const Ray & ray) const
{
    std::vector<SkeletonJoint *>::const_iterator it = m_joints.begin();
	for(; it != m_joints.end(); ++it) {
		if((*it)->intersect(ray)) return *it;
	}
	return m_joints[0];
}

unsigned SkeletonSystem::degreeOfFreedom() const
{
	std::vector<Float3> dofs;
	degreeOfFreedom(m_joints[0], dofs);
	unsigned ndof = 0;
	std::vector<Float3>::iterator it = dofs.begin();
	for(; it != dofs.end(); ++it) {
		if((*it).x > 0.f) ndof++;
		if((*it).y > 0.f) ndof++;
		if((*it).z > 0.f) ndof++;
	}
	return ndof;
}

void SkeletonSystem::degreeOfFreedom(BaseTransform * j, std::vector<Float3> & dof) const
{
	dof.push_back(j->rotateDOF());
	
	for(unsigned i = 0; i < j->numChildren(); i++) degreeOfFreedom(j->child(i), dof);
}

void SkeletonSystem::rotationAngles(BaseTransform * j, std::vector<Vector3F> & angles) const
{
	angles.push_back(j->rotationAngles());
	for(unsigned i = 0; i < j->numChildren(); i++) rotationAngles(j->child(i), angles);
}

void SkeletonSystem::addPose()
{
	SkeletonPose *pose = new SkeletonPose;
	pose->setName("pose", maxPoseIndex() + 1);
	pose->setIndex(maxPoseIndex() + 1);
	pose->setNumJoints(numJoints());
	std::vector<Float3> dofs;
	degreeOfFreedom(m_joints[0], dofs);
	pose->setDegreeOfFreedom(dofs);
	std::vector<Vector3F> angles;
	rotationAngles(m_joints[0], angles);
	pose->setValues(dofs, angles);
	
	m_poses.push_back(pose);
	m_activePose = pose;
}

void SkeletonSystem::selectPose(unsigned i)
{
	m_activePose = m_poses[i];
}

void SkeletonSystem::selectPose(const std::string & name)
{
	m_activePose = 0;
	std::vector<SkeletonPose *>::const_iterator it = m_poses.begin();
	for(; it != m_poses.end(); ++it) {
		if((*it)->name() == name) m_activePose = *it;
	}
}
	
void SkeletonSystem::updatePose()
{
	if(!m_activePose) return;
	std::vector<Float3> dofs;
	degreeOfFreedom(m_joints[0], dofs);
	std::vector<Vector3F> angles;
	rotationAngles(m_joints[0], angles);
	m_activePose->setValues(dofs, angles);
}

void SkeletonSystem::recoverPose()
{
	if(!m_activePose) return;
	m_activePose->recoverValues(m_joints);
}

void SkeletonSystem::renamePose(const std::string & fromName, const std::string & toName)
{
	selectPose(fromName);
	if(!m_activePose) return;
	m_activePose->setName(toName);
}

unsigned SkeletonSystem::numPoses() const
{
	return m_poses.size();
}

SkeletonPose * SkeletonSystem::pose(unsigned idx) const
{
	return m_poses[idx];
}

SkeletonPose * SkeletonSystem::currentPose() const
{
	return m_activePose;
}

unsigned SkeletonSystem::maxPoseIndex() const
{
	unsigned mx = 0;
	std::vector<SkeletonPose *>::const_iterator it = m_poses.begin();
    for(; it != m_poses.end(); ++it) {
		if((*it)->index() > mx) mx = (*it)->index();
	}
	return mx;
}

unsigned SkeletonSystem::closestJointIndex(const Vector3F & pt) const
{
	unsigned res = m_joints[0]->index();
	float curD, minD = 10e8;
	std::vector<SkeletonJoint *>::const_iterator it = m_joints.begin();
	for(; it != m_joints.end(); ++it) {
		curD = (*it)->worldSpace().getTranslation().distance2To(pt);
		if(curD < minD) {
			minD = curD;
			res = (*it)->index();
		}
	}
	
	return res;
}

void SkeletonSystem::calculateBindWeights(const Vector3F & pt, VectorN<unsigned> & ids, VectorN<float> & weights) const
{
	Vector3F q = pt;
	q.z = 0.f;
	
	unsigned i = closestJointIndex(q);
	SkeletonJoint * jointA = jointByIndex(i);
	Matrix44F spaceInv = jointA->worldSpace();
	spaceInv.inverse();
	
	Vector3F subP = spaceInv.transform(q);
	
	if(subP.x < 0.f || jointA->numChildren() < 1) {
		if(jointA->parent()) {
			i = jointA->parent()->index();
			jointA = jointByIndex(i);
			spaceInv = jointA->worldSpace();
			spaceInv.inverse();
			subP = spaceInv.transform(q);
		}
	}
	
	float la = jointA->length();
	
	if(subP.x < 0.f) {
		initIdWeight(1, ids, weights);
		*ids.at(0) = jointA->index();
		*weights.at(0) = 1.f;
		return;
	}
	
	float lowGate = la * .45f;
	float highGate = la * .55f;
	
	if(subP.x >= lowGate && subP.x <= highGate) {
		initIdWeight(1, ids, weights);
		*ids.at(0) = jointA->index();
		*weights.at(0) = 1.f;
		return;
	}
	
	SkeletonJoint * jointB;
	float w;
	
	if(subP.x < lowGate) {
		if(!jointA->parent()) {
			initIdWeight(1, ids, weights);
			*ids.at(0) = jointA->index();
			*weights.at(0) = 1.f;
			return;
		}
		
		jointB = jointByIndex(jointA->parent()->index());
		initIdWeight(2, ids, weights);
		*ids.at(0) = jointA->index();
		*ids.at(1) = jointB->index();
		
		w = .5f + .5f * subP.x / lowGate;
		*weights.at(0) = w;
		*weights.at(1) = 1.f - w;
	}
	else {
		jointB = jointByIndex(jointA->child(0)->index());
		initIdWeight(2, ids, weights);
		*ids.at(0) = jointA->index();
		*ids.at(1) = jointB->index();
		
		w = 1.f - .5f * (subP.x - highGate)/(la - highGate);
		*weights.at(0) = w;
		*weights.at(1) = 1.f - w;
	}
}

void SkeletonSystem::initIdWeight(unsigned n, VectorN<unsigned> & ids, VectorN<float> & weights) const
{
	ids.setZero(n);
	weights.setZero(n);
}
