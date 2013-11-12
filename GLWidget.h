#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QGLWidget>

/*#include "TreeModeler/cdagtree.h"
#include "TreeModeler/cubic_bspline_curve.h"*/
#include "TreeModeler/tree_mesh.h"
#include "TreeModeler/tree_skeleton.h"
#include "TreeModeler/tree_mesh_base.h"
#include "TreeModeler/silh_data_obj.h"

#include <vector>

class GLWidget:public QGLWidget
{
	Q_OBJECT
public:
	GLWidget(QWidget* aParent=NULL);
	~GLWidget();
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void drawTreeSkeleton(const SilhouetteTree::TreeSkeleton& aTreeSkeleton,bool bLineMode);
	//void drawTreeSkeleton(SilhouetteTree::DAGNode* aRootNodePtr);
	//void drawTreeSkeleton(const std::vector<SilhouetteTree::CubicBsplineCurve>& aCurveList,int aMode);
	void drawTreeSurface(const SilhouetteTree::TreeMeshBase& aTreeMesh);
	void drawTreeCrown(const SilhouetteTree::SilhDataObj& aSilhData);
	void resetGL();
public slots:
	void setXRotation(int aRadians);
	void setYRotation(int aRadians);
	void setZRotation(int aRadians);
protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int aWidth,int aHeight);
	void mousePressEvent(QMouseEvent * aEvent);
	void mouseMoveEvent(QMouseEvent * aEvent);
	void wheelEvent(QWheelEvent * aEvent);
	void drawTreeNodeRec(SilhouetteTree::DAGNode* aNodePtr);
	
private:
	int m_x_rot;
	int m_y_rot;
	int m_z_rot;
	float m_last_scale;
	//float m_current_scale;
	QPoint m_last_pos;
	GLuint m_display_list_id;
};

#endif