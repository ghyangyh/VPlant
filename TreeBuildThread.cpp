#include "TreeBuildThread.h"

#include <QtGui>

#include "TreeModeler/tree_modeler.h"
using namespace SilhouetteTree;

TreeBuildThread::TreeBuildThread(QObject* aParent)
	:QThread(0),m_tree_skeleton(0),m_silh_data_ptr(0),
	m_rand_offset(0.f),m_truncate_ratio(1.f),m_scale(20.f),
	m_level_resample_nums(0),m_tree_modeler(0)
{

}

TreeBuildThread::~TreeBuildThread()
{
	if(m_tree_skeleton!=0)
		delete m_tree_skeleton;

	if(m_tree_modeler!=0)
		delete m_tree_modeler;
}

void TreeBuildThread::StartThread( SilhDataObj* aSilhData,
									const SilhouetteTree::TreeParam& aTreeParam,
									float aTruncateRatio,
									float aRandOffset,
									float aScale,
									int aLevelResampleNums)
{
	m_silh_data_ptr=aSilhData;
	m_tree_param=aTreeParam;
	m_truncate_ratio=aTruncateRatio;
	m_rand_offset=aRandOffset;
	m_scale=aScale;
	m_level_resample_nums=aLevelResampleNums;
	start();
}


void TreeBuildThread::run()
{
	//TreeModeler a_tree_modeler;
	if( m_tree_modeler!=0 ){
		delete m_tree_modeler;
		m_tree_modeler=0;
	}
	m_tree_modeler=new TreeModeler();
	m_tree_modeler->preProcessData(*m_silh_data_ptr,m_tree_param,m_truncate_ratio,m_rand_offset,m_scale);
	m_tree_modeler->buildTree(m_tree_param,m_level_resample_nums);

	if( m_tree_skeleton!=0 )
		delete m_tree_skeleton;
	m_tree_skeleton=new TreeSkeleton();
	m_tree_skeleton->setTreeRootNodePtr( m_tree_modeler->getTreeRoot() );
	m_tree_skeleton->constructSkeleton();

	emit finishedBuild(/*m_tree_skeleton*/);

}