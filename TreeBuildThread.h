#ifndef TREE_BUILD_THREAD_H
#define TREE_BUILD_THREAD_H

#include <QThread>

#include "TreeModeler/tree_config_loader.h"
#include "TreeModeler/silh_data_obj.h"
#include "TreeModeler/tree_skeleton.h"
#include "TreeModeler/tree_modeler.h"


class TreeBuildThread : public QThread
{
	Q_OBJECT
public:
	TreeBuildThread(QObject* aParent=0);

	~TreeBuildThread();
	void StartThread(SilhouetteTree::SilhDataObj* aSilhData,
					const SilhouetteTree::TreeParam& aTreeParam,
					float aTruncateRatio,
					float aRandOffset,
					float aScale=20.f,
					int aLevelResampleNums=0);
	SilhouetteTree::TreeSkeleton* getTreeSkeleton() const{
		return m_tree_skeleton;
	}
protected:
	void run();
signals:
	void finishedBuild(/*SilhouetteTree::TreeSkeleton* aTreeSkeleton*/);
private:
	SilhouetteTree::TreeSkeleton* m_tree_skeleton;
	SilhouetteTree::TreeModeler* m_tree_modeler;
	//const SilhouetteTree::SilhDataObj& m_silh_data_ref;
	//const SilhouetteTree::TreeParam& m_tree_param_ref;
	SilhouetteTree::SilhDataObj* m_silh_data_ptr;
	SilhouetteTree::TreeParam m_tree_param;
	float m_truncate_ratio;
	float m_rand_offset;
	float m_scale;
	int m_level_resample_nums;
};

#endif