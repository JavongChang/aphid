/*
 *  main.cpp
 *  rotaConstraint
 *
 *  Created by jian zhang on 7/7/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MFnPlugin.h>
#include "sargassoCmd.h"
#include "sargassoNode.h"

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang Jian", "0.0.3 Tue Jul 22 00:22:32 CST 2015 first", "Any");

	status = plugin.registerNode( "sargassoNode", SargassoNode::id, SargassoNode::creator,
		SargassoNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = plugin.registerCommand( "sargasso", SargassoCmd::creator,
												SargassoCmd::newSyntax);
	if (!status) {
		status.perror("registerConstraintCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( SargassoNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = plugin.deregisterCommand( "sargasso" );
	if (!status) {
		status.perror("deregisterCmd");
		return status;
	}

	return status;
}