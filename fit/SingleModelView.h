/*
 *  SingleModelView.h
 *  fit
 *
 *  Created by jian zhang on 5/6/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <Base3DView.h>

class ToolContext;
class PatchMesh;
class KdTree;
class AnchorGroup;
class Anchor;
class Ray;
class MeshTopology;

class SingleModelView : public Base3DView
{
    Q_OBJECT

public:
    /*enum InteractMode {
        SelectCompnent,
        TransformAnchor
    };*/
	
    SingleModelView(QWidget *parent = 0);
    ~SingleModelView();
	
	virtual bool anchorSelected(float wei);
	virtual bool removeLastAnchor(unsigned & idx);
	virtual bool removeActiveAnchor(unsigned & idx);
	
	bool pickupComponent(const Ray & ray, Vector3F & hit);
	
	virtual void clientDraw();
    virtual void clientSelect(Vector3F & origin, Vector3F & displacement, Vector3F & hit);
    virtual void clientDeselect();
    virtual void clientMouseInput(Vector3F & origin, Vector3F & displacement, Vector3F & stir);
    virtual void sceneCenter(Vector3F & dst) const;
    
	virtual void buildTree();
	virtual void buildTopology();
	virtual void loadMesh(std::string filename);
	virtual void saveMesh(std::string filename);
	
	void drawAnchors();
	AnchorGroup * getAnchors() const;
	KdTree * getTree() const;
	
	static ToolContext * InteractContext;
	int interactMode();
	
protected:
    void keyPressEvent(QKeyEvent *event);
    
public:
    PatchMesh * m_mesh;
	KdTree * m_tree;
	AnchorGroup * m_anchors;
	MeshTopology * m_topo;
	
private:
	
public slots:
	void open();
	void save();
};
