#include <QtGui>
#include <QtOpenGL>
#include <gl/GLU.h>

#include "GLWidget.h"

#include <list>
#include <cstring>
using namespace std;

#include "TreeModeler/cubic_bspline_curve.h"
using namespace SilhouetteTree;

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

static Vectorn<float> computeNormal(const Vectorn<float>& aPt0,const Vectorn<float>& aPt1,const Vectorn<float>& aPt2){
	Vectorn<float> a_edge1=aPt1-aPt0;
	Vectorn<float> a_edge2=aPt2-aPt0;
	Vectorn<float> a_normal=cross3d(a_edge1,a_edge2);
	a_normal.normalize();
	return a_normal;
}

GLWidget::GLWidget(QWidget* aParent/* =NULL */):QGLWidget(aParent),m_x_rot(0),m_y_rot(0),m_z_rot(0),
												m_display_list_id(0),m_last_scale(1.f)//,m_current_scale(1.f)
{

}

GLWidget::~GLWidget()
{

}

void GLWidget::resetGL()
{
	m_x_rot=0;
	m_y_rot=0;
	m_z_rot=0;
	m_last_scale=1.f;
}

void GLWidget::drawTreeNodeRec(DAGNode* aNodePtr)
{
	list<DAGNode*>::iterator a_start_iter=aNodePtr->m_child_list.begin();

	for(;a_start_iter!=aNodePtr->m_child_list.end();++a_start_iter){

		glVertex3f(aNodePtr->m_junction_pos[0],aNodePtr->m_junction_pos[1],aNodePtr->m_junction_pos[2]);
		
		glVertex3f((*a_start_iter)->m_junction_pos[0],(*a_start_iter)->m_junction_pos[1],(*a_start_iter)->m_junction_pos[2]);

		drawTreeNodeRec( (*a_start_iter) );
	}
}

void GLWidget::drawTreeSkeleton(const TreeSkeleton& aTreeSkeleton,bool bLineMode)
{
	makeCurrent();

	m_display_list_id=glGenLists(1);

	if(m_display_list_id!=0){
		
		//glColor3f(1.f,0.f,0.f);
		glNewList(m_display_list_id,GL_COMPILE);
		const vector<CubicBsplineCurve>& a_curve_list=aTreeSkeleton.getCurveList();

		vector<CubicBsplineCurve>::const_iterator a_start_iter=a_curve_list.begin();
		for(;a_start_iter!=a_curve_list.end();++a_start_iter){
			glColor3f(uniform_range(0.f,1.f),uniform_range(0.f,0.6f),uniform_range(0.f,1.f));
			glBegin(GL_LINE_STRIP);
			
			//uniform cubic B-spline
			if(!bLineMode){
				for(int i=0;i<(*a_start_iter).getSpanNums();++i){
					const CubicBsplineSpan& a_span=(*a_start_iter).getSpanAt(i);
					float a_step=0.1f;
					for(int t=0;t<=10;++t){
						Vectorn<float> a_sample_pt=a_span.sampleAt(t*a_step);
						glVertex3f(a_sample_pt[0],a_sample_pt[1],a_sample_pt[2]);
					}
				}
			}
			//straight line segments
			else {
				const vector<DAGNode*>& a_node_list=(*a_start_iter).getKnotList();
				vector<DAGNode*>::const_iterator a_start_iter=a_node_list.begin();
				for(;a_start_iter!=a_node_list.end();++a_start_iter){
					glVertex3f((*a_start_iter)->m_junction_pos[0],(*a_start_iter)->m_junction_pos[1],(*a_start_iter)->m_junction_pos[2]);
				}
			}
			
			glEnd();

			glColor3f(0.f,0.f,0.f);
			glPointSize(2.5f);
			glBegin(GL_POINTS);
				Vectorn<float> a_start_pt=(*a_start_iter).getSpanAt(0).sampleAt(0);
				glVertex3f(a_start_pt[0],a_start_pt[1],a_start_pt[2]);
			glEnd();
		}

		glEndList();
	}
	updateGL();
}

/*void GLWidget::drawTreeSkeleton(DAGNode* aRootNodePtr)
{
	makeCurrent();

	m_display_list_id=glGenLists(1);

	if(m_display_list_id!=0){

		glColor3f(1.f,0.f,0.f);

		glNewList(m_display_list_id,GL_COMPILE);
	
		
		glBegin(GL_LINES);

		drawTreeNodeRec(aRootNodePtr);

		glEnd();

		glEndList();

	}
	
}

void GLWidget::drawTreeSkeleton(const vector<CubicBsplineCurve>& aCurveList,int aMode)
{
	makeCurrent();

	m_display_list_id=glGenLists(1);

	if(m_display_list_id!=0){
		
		//glColor3f(1.f,0.f,0.f);
		glNewList(m_display_list_id,GL_COMPILE);
		
		vector<CubicBsplineCurve>::const_iterator a_start_iter=aCurveList.begin();
		for(;a_start_iter!=aCurveList.end();++a_start_iter){
			glColor3f(uniform_range(0.f,1.f),uniform_range(0.f,0.6f),uniform_range(0.f,1.f));
			glBegin(GL_LINE_STRIP);
			
			//uniform cubic B-spline
			if(aMode==2){
				for(int i=0;i<(*a_start_iter).getSpanNums();++i){
					const CubicBsplineSpan& a_span=(*a_start_iter).getSpanAt(i);
					float a_step=0.1f;
					for(int t=0;t<=10;++t){
						Vectorn<float> a_sample_pt=a_span.sampleAt(t*a_step);
						glVertex3f(a_sample_pt[0],a_sample_pt[1],a_sample_pt[2]);
					}
				}
			}
			//straight line segments
			else if(aMode==1){
				const vector<DAGNode*>& a_node_list=(*a_start_iter).getKnotList();
				vector<DAGNode*>::const_iterator a_start_iter=a_node_list.begin();
				for(;a_start_iter!=a_node_list.end();++a_start_iter){
					glVertex3f((*a_start_iter)->m_junction_pos[0],(*a_start_iter)->m_junction_pos[1],(*a_start_iter)->m_junction_pos[2]);
				}
			}
			
			glEnd();

			glColor3f(0.f,0.f,0.f);
			glPointSize(2.5f);
			glBegin(GL_POINTS);
				Vectorn<float> a_start_pt=(*a_start_iter).getSpanAt(0).sampleAt(0);
				glVertex3f(a_start_pt[0],a_start_pt[1],a_start_pt[2]);
			glEnd();
		}

		glEndList();
	}

}*/

void GLWidget::drawTreeSurface(const TreeMeshBase& aTreeMesh)
{
	makeCurrent();
	glDeleteLists(m_display_list_id,1);
	m_display_list_id=glGenLists(1);
	if(m_display_list_id!=0){
		vector<Vectorn<float> > a_color_map;
		a_color_map.push_back(Vectorn<float>(205.f/255.f,170.f/255.f,125.f/255.f));//burlywood3
		a_color_map.push_back(Vectorn<float>(255.f/255.f,52.f/255.f,179.f/255.f));
		a_color_map.push_back(Vectorn<float>(250.f/255.f,128.f/255.f,114.f/255.f));//salmon
		a_color_map.push_back(Vectorn<float>(147.f/255.f,112.f/255.f,219.f/255.f));//medium purple
		a_color_map.push_back(Vectorn<float>(0.f/255.f,191.f/255.f,255.f/255.f));//
		a_color_map.push_back(Vectorn<float>(144.f/255.f,238.f/255.f,144.f/255.f));
		
		for(int i=6;i<=aTreeMesh.getMaximumAxisOrder();++i){
			a_color_map.push_back(Vectorn<float>(uniform_range(0.f,1.f),uniform_range(0.f,1.f),uniform_range(0.f,1.f)));
		}
		glNewList(m_display_list_id,GL_COMPILE);

		glBegin(GL_TRIANGLES);
		const vector<Vectorn<int> >& a_tri_list=aTreeMesh.getTriList();
		const vector<Vectorn<float> >& a_vert_list=aTreeMesh.getVertList();
		
		vector<Vectorn<int> >::const_iterator a_start_iter=a_tri_list.begin();
		
		for(;a_start_iter!=a_tri_list.end();++a_start_iter){
			Vectorn<float> a_color=a_color_map[ (*a_start_iter)[3]-1 ];
			glColor3f(a_color[0],a_color[1],a_color[2]);
			
			int a_vert_index1=(*a_start_iter)[0];
			int a_vert_index2=(*a_start_iter)[1];
			int a_vert_index3=(*a_start_iter)[2];

			Vectorn<float> a_vert1=a_vert_list[ a_vert_index1 ];
			Vectorn<float> a_vert2=a_vert_list[ a_vert_index2 ];
			Vectorn<float> a_vert3=a_vert_list[ a_vert_index3 ];

			Vectorn<float> a_normal=computeNormal(a_vert1,a_vert2,a_vert3);

			glNormal3f(a_normal[0],a_normal[1],a_normal[2]);
			glVertex3f(a_vert1[0],a_vert1[1],a_vert1[2]);
			
			glNormal3f(a_normal[0],a_normal[1],a_normal[2]);
			glVertex3f(a_vert2[0],a_vert2[1],a_vert2[2]);

			glNormal3f(a_normal[0],a_normal[1],a_normal[2]);
			glVertex3f(a_vert3[0],a_vert3[1],a_vert3[2]);

		}

		glEnd();
	

		/*glColor3f(1.f,0.f,0.f);
		const vector<Vectorn<float> >& a_vert_list=aTreeMesh.getVertList();
		const vector<InternodeSurf>& a_surf_list=aTreeMesh.getSurfList();
		vector<InternodeSurf>::const_iterator a_start_iter=a_surf_list.begin();
		for(;a_start_iter!=a_surf_list.end();++a_start_iter){
			const vector<CircleStrip>& a_strip_list=a_start_iter->getStripList();
			vector<CircleStrip>::const_iterator a_strip_iter=a_strip_list.begin();
			for(;a_strip_iter!=a_strip_list.end();++a_strip_iter){
				glBegin(GL_LINE_LOOP);
				for(int i=a_strip_iter->getStartId();i<a_strip_iter->getStopId();++i){
					Vectorn<float> a_vert=a_vert_list[i];
					glVertex3f(a_vert[0],a_vert[1],a_vert[2]);
				}
				glEnd();
			}
		}*/
		glEndList();
	}

	updateGL();
}


void GLWidget::drawTreeCrown(const SilhouetteTree::SilhDataObj& aSilhData)
{
	makeCurrent();
	glDeleteLists(m_display_list_id,1);
	m_display_list_id=glGenLists(1);
	if(m_display_list_id!=0){
		//draw tree crown here
		if( strcmp(aSilhData.getDataFileType(),"PointClouds")==0 ){
			const float* a_vertex_array=aSilhData.getVertexData();
			int a_array_size=aSilhData.getVertexDataSize();
			glNewList(m_display_list_id,GL_COMPILE);
			glColor3f(1.f,0.f,0.f);
			glPointSize(2.f);
			glBegin(GL_POINTS);
			for(int i=0;i<a_array_size;i+=3){
				glVertex3f(a_vertex_array[i],a_vertex_array[i+1],a_vertex_array[i+2]);
			}
			glEnd();
			glEndList();
		}
		else{//draw triangle mesh
			const float* a_vertex_array=aSilhData.getVertexData();
			int a_vertex_data_size=aSilhData.getVertexDataSize();
			const vector<int>& a_tri_index_array=aSilhData.getTriFaceIndexArray();
			glNewList(m_display_list_id,GL_COMPILE);
			glColor3f(1.f,0.f,0.f);
			for(int i=0;i<a_tri_index_array.size();i+=3){
				int a_v_idx_1=a_tri_index_array[i]-1;
				int a_v_idx_2=a_tri_index_array[i+1]-1;
				int a_v_idx_3=a_tri_index_array[i+2]-1;
				glBegin(GL_LINE_LOOP);
				glVertex3f(a_vertex_array[a_v_idx_1*3],a_vertex_array[a_v_idx_1*3+1],a_vertex_array[a_v_idx_1*3+2]);
				glVertex3f(a_vertex_array[a_v_idx_2*3],a_vertex_array[a_v_idx_2*3+1],a_vertex_array[a_v_idx_2*3+2]);
				glVertex3f(a_vertex_array[a_v_idx_3*3],a_vertex_array[a_v_idx_3*3+1],a_vertex_array[a_v_idx_3*3+2]);
				glEnd();
			}
			
			glEndList();
		}
	}
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50,50);
}

QSize GLWidget::sizeHint() const 
{
	return QSize(800,600);
}

void GLWidget::setXRotation(int aRadians)
{
	qNormalizeAngle(aRadians);
	if( aRadians!=m_x_rot ){
		m_x_rot=aRadians;
		updateGL();
	}
}

void GLWidget::setYRotation(int aRadians)
{
	qNormalizeAngle(aRadians);
	if( aRadians!=m_y_rot ){
		m_y_rot=aRadians;
		updateGL();
	}
}

void GLWidget::setZRotation(int aRadians)
{
	qNormalizeAngle(aRadians);
	if( aRadians!=m_z_rot ){
		m_z_rot=aRadians;
		updateGL();
	}
}

void GLWidget::initializeGL()
{
	//glClearColor(0.0,0.545,0.545,0.0);
	glClearColor(1.0,1.0,1.0,0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	//set lighting 
	
	//set the LIGHT0
	GLfloat a_light0_pos[]={1.f,1.f,0.f,0.f};
	GLfloat a_light0_ambient_color[]={0.f,0.f,0.f,1.f};
	GLfloat a_light0_diffuse_color[]={1.f,1.f,1.f,1.f};
	GLfloat a_light0_specular_color[]={1.f,1.f,1.f,1.f};
	glLightfv(GL_LIGHT0,GL_POSITION,a_light0_pos);
	glLightfv(GL_LIGHT0,GL_AMBIENT,a_light0_ambient_color);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,a_light0_diffuse_color);
	glLightfv(GL_LIGHT0,GL_SPECULAR,a_light0_specular_color);

	//set the global ambient light
	GLfloat a_ambient_color[]={0.6f,0.6f,0.6f,1.f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,a_ambient_color);

	//enable and set color material
	glEnable(GL_COLOR_MATERIAL);
	//set both the ambient and diffuse material of the object to a same color
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
	glRotatef(m_x_rot/16.f,1.f,0.f,0.f);
	glRotatef(m_y_rot/16.f,0.f,1.f,0.f);
	//glRotatef(m_z_rot/16.f,0.f,0.f,1.f);

	/*glColor3f(1.f,0.f,0.f);
	glBegin(GL_LINES);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(10.0,10.0,0.0);
	glEnd();*/
	glScalef(m_last_scale,m_last_scale,m_last_scale);
	glCallList(m_display_list_id);

	glPopMatrix();
	glFlush();
}

void GLWidget::resizeGL(int aWidth,int aHeight)
{
	glViewport(0,0,aWidth,aHeight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-15.0,15.0,-15.0,15.0,4.0,15.0);
	
	gluPerspective(60.0,GLfloat(aWidth)/GLfloat(aHeight),10,100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,60.0,0.0,0.0,0.0,0.0,1.0,0.0);

}

void GLWidget::mousePressEvent(QMouseEvent * aEvent)
{
	m_last_pos=aEvent->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent * aEvent)
{
	int a_dx=aEvent->x()-m_last_pos.x();
	int a_dy=aEvent->y()-m_last_pos.y();

	if(aEvent->buttons() & Qt::LeftButton){
		setXRotation(m_x_rot+8*a_dy);
		setYRotation(m_y_rot+8*a_dx);
	}
	m_last_pos=aEvent->pos();
}

void GLWidget::wheelEvent(QWheelEvent * aEvent)
{
	/*int a_degrees_num=aEvent->delta()/8;
	int a_step_num=a_degrees_num/15;*/

	if(aEvent->delta()>0){
		m_last_scale*=1.2f;
	}
	else{
		m_last_scale/=1.2f;
	}
	updateGL();
}
