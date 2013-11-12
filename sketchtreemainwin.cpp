#include "sketchtreemainwin.h"

#include "QDebugStream.h"//redirect stdout to qt text edit

#include <QtGui>
#include "GLWidget.h"
#include "ParamsGroup.h"
#include "LoadDataDialog.h"

#include <cassert>
#include <iostream>
using namespace std;


#include "TreeModeler/tree_builder.h"
#include "TreeModeler/tree_curve_skeleton_mesh.h"
#include "TreeModeler/tree_line_skeleton_mesh.h"
#include "TreeModeler/obj_exporter.h"
using namespace SilhouetteTree;

SketchTreeMainWin::SketchTreeMainWin(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),m_load_data_dialog(0),m_crown_rand_offset_val(0.f),
	  m_crown_truncate_ratio_val(1.f),m_trunk_root_offset_val(-15.f),
	  m_multilevel_enabled(false),m_is_display_skeleton(false),m_silh_data_obj(0),
	  m_is_crown_loaded(false),m_tree_mesh_line(0)
{
	ui.setupUi(this);

	//qRegisterMetaType<TreeSkeleton*>();
	//m_tree_build_thread=new TreeBuildThread(this);
	connect(&m_tree_build_thread,SIGNAL(finishedBuild(/*TreeSkeleton*/)),this,SLOT(onFinishedBuildTree(/*TreeSkeleton*/)));
	
	m_gl_widget_tree=new GLWidget(this);
	//m_gl_widget_mesh=new GLWidget(this);
	//m_params_panel=new ParamsGroup(tr("Level 1 Params"),this);

	m_build_mesh_btn=new QPushButton(tr("Build Crown Mesh"),this);
	m_build_tree_btn=new QPushButton(tr("Build Tree"),this);
	m_export_tree_obj=new QPushButton(tr("Export Tree Obj"),this);
	connect(m_build_mesh_btn,SIGNAL(clicked()),this,SLOT(buildCrownMesh()));
	connect(m_build_tree_btn,SIGNAL(clicked()),this,SLOT(buildTree()));
	connect(m_export_tree_obj,SIGNAL(clicked()),this,SLOT(onExportTree()));
	m_export_tree_obj->setEnabled(false);
	//m_build_tree_btn->setEnabled(false);

	/*m_tree_param_level_box=new QComboBox;
	m_tree_param_level_box->addItem(tr("level 1"));
	m_tree_param_level_box->addItem(tr("level 2"));*/

	constructMultilevelParam();

	constructSilhViewGroup();

	constructGlobalParamGroup();

	m_debug_window=new QTextEdit(this);
	m_debug_window->setReadOnly(true);
	m_debug_window->setFontPointSize(13);
	m_debug_window->setTextColor(QColor(255,255,255));
	QPalette p=m_debug_window->palette();
	p.setColor(QPalette::Base,QColor(139,131,134));
	m_debug_window->setPalette(p);
	m_qout=new QDebugStream(cout,m_debug_window);

	//layout of left ui part
	QVBoxLayout* a_v_layout1=new QVBoxLayout;
	a_v_layout1->addWidget(m_gl_widget_tree);
	a_v_layout1->addWidget(m_debug_window);
	QHBoxLayout* a_h_layout1=new QHBoxLayout;
	a_h_layout1->addWidget(m_build_mesh_btn);
	a_h_layout1->addWidget(m_build_tree_btn);
	a_h_layout1->addWidget(m_export_tree_obj);
	a_v_layout1->addLayout(a_h_layout1);

	//layout of right ui part
	QVBoxLayout* a_v_layout2=new QVBoxLayout;
	a_v_layout2->addWidget(m_stacked_param);
	a_v_layout2->addWidget(m_tree_param_level_box);
	a_v_layout2->addWidget(m_global_param_group);
	a_v_layout2->addWidget(m_silh_mesh_view_group);

	//whole ui
	QHBoxLayout* a_main_layout=new QHBoxLayout;
	a_main_layout->addLayout(a_v_layout1);
	a_main_layout->addLayout(a_v_layout2);

	/*QHBoxLayout* a_h_layout1=new QHBoxLayout;
	a_h_layout1->addWidget(m_gl_widget);
	a_h_layout1->addWidget(m_params_panel);

	QHBoxLayout* a_h_layout2=new QHBoxLayout;
	a_h_layout2->addWidget(m_build_mesh_btn);
	a_h_layout2->addWidget(m_build_tree_btn);

	QVBoxLayout* a_main_layout=new QVBoxLayout;
	a_main_layout->addLayout(a_h_layout1);
	a_main_layout->addLayout(a_h_layout2);*/

	setLayout(a_main_layout);
}

SketchTreeMainWin::~SketchTreeMainWin()
{
	delete m_qout;

	if(m_silh_data_obj!=0){
		delete m_silh_data_obj;
		m_silh_data_obj=0;
	}
	if(m_tree_mesh_line!=0){
		delete m_tree_mesh_line;
		m_tree_mesh_line=0;
	}
	//delete m_tree_build_thread;
}

void SketchTreeMainWin::constructMultilevelParam()
{
	m_stacked_param=new QStackedWidget;
	m_params_panel=new ParamsGroup(tr("Level 1 Params"),this);
	m_params_panel_l2=new ParamsGroup(tr("Level 2 Params"),this);
	m_stacked_param->addWidget(m_params_panel);
	m_stacked_param->addWidget(m_params_panel_l2);

	m_tree_param_level_box=new QComboBox;
	m_tree_param_level_box->addItem(tr("level 1"));
	m_tree_param_level_box->addItem(tr("level 2"));

	connect(m_tree_param_level_box,SIGNAL(activated(int)),m_stacked_param,SLOT(setCurrentIndex(int)));
}

void SketchTreeMainWin::constructSilhViewGroup()
{
	m_silh_mesh_view_group=new QGroupBox(tr("Tree Crown"));
	
	m_gl_widget_mesh=new GLWidget(this);

	QLabel* a_label_min_x=new QLabel(tr("MinX:"));
	QLabel* a_label_max_x=new QLabel(tr("MaxX:"));

	QLabel* a_label_min_y=new QLabel(tr("MinY:"));
	QLabel* a_label_max_y=new QLabel(tr("MaxY:"));

	QLabel* a_label_min_z=new QLabel(tr("MinZ:"));
	QLabel* a_label_max_z=new QLabel(tr("MaxZ:"));

	QLabel* a_label_silh_vertices=new QLabel(tr("Total Vertices: "));
	m_edit_silh_vertices=new QLineEdit;
	

	m_edit_silh_min_x=new QLineEdit;
	m_edit_silh_min_x->setReadOnly(true);

	m_edit_silh_max_x=new QLineEdit;
	m_edit_silh_max_x->setReadOnly(true);

	m_edit_silh_min_y=new QLineEdit;
	m_edit_silh_min_y->setReadOnly(true);

	m_edit_silh_max_y=new QLineEdit;
	m_edit_silh_max_y->setReadOnly(true);

	m_edit_silh_min_z=new QLineEdit;
	m_edit_silh_min_z->setReadOnly(true);

	m_edit_silh_max_z=new QLineEdit;
	m_edit_silh_max_z->setReadOnly(true);

	QGridLayout* a_grid_layout=new QGridLayout;
	a_grid_layout->addWidget(a_label_min_x,0,0);
	a_grid_layout->addWidget(a_label_min_y,1,0);
	a_grid_layout->addWidget(a_label_min_z,2,0);
	a_grid_layout->addWidget(a_label_silh_vertices,3,0);
	a_grid_layout->addWidget(m_edit_silh_vertices,3,1);
	a_grid_layout->addWidget(m_edit_silh_min_x,0,1);
	a_grid_layout->addWidget(m_edit_silh_min_y,1,1);
	a_grid_layout->addWidget(m_edit_silh_min_z,2,1);
	a_grid_layout->addWidget(a_label_max_x,0,2);
	a_grid_layout->addWidget(a_label_max_y,1,2);
	a_grid_layout->addWidget(a_label_max_z,2,2);
	a_grid_layout->addWidget(m_edit_silh_max_x,0,3);
	a_grid_layout->addWidget(m_edit_silh_max_y,1,3);
	a_grid_layout->addWidget(m_edit_silh_max_z,2,3);

	QVBoxLayout* a_main_layout=new QVBoxLayout;
	a_main_layout->addWidget(m_gl_widget_mesh);
	a_main_layout->addLayout(a_grid_layout);

	m_silh_mesh_view_group->setLayout(a_main_layout);

}

void SketchTreeMainWin::constructGlobalParamGroup()
{
	m_global_param_group=new QGroupBox(tr("Global Param Settings"));
	m_is_multilevel_enabled=new QCheckBox(tr("Enable Multi-level params"));

	QLabel* a_label_scale=new QLabel(tr("CrwonTruncateRatio:"));
	m_crown_truncate_ratio=new QSlider(Qt::Horizontal);
	m_crown_truncate_ratio->setTickPosition(QSlider::TicksAbove);
	m_crown_truncate_ratio->setRange(0,20);
	m_crown_truncate_ratio->setTickInterval(2);
	m_crown_truncate_ratio->setSingleStep(1);
	m_crown_truncate_ratio->setValue(0);
	m_label_truncate=new QLabel(tr("0"));
	connect(m_crown_truncate_ratio,SIGNAL(valueChanged(int)),this,SLOT(onSetCrownTruncate(int)));

	QLabel*  a_label_rand_offset=new QLabel(tr("CrownPointsRandOffset:"));
	m_crown_rand_offset=new QSlider(Qt::Horizontal);
	m_crown_rand_offset->setTickPosition(QSlider::TicksAbove);
	m_crown_rand_offset->setRange(0,10);
	m_crown_rand_offset->setTickInterval(1);
	m_crown_rand_offset->setSingleStep(1);
	m_label_offset=new QLabel(tr("0"));
	connect(m_crown_rand_offset,SIGNAL(valueChanged(int)),this,SLOT(onSetCrownRandOffset(int)));

	QLabel*  a_label_root_offset=new QLabel(tr("TreeTrunkRootOffset:"));
	m_trunk_root_offset=new QSlider(Qt::Horizontal);
	m_trunk_root_offset->setTickPosition(QSlider::TicksAbove);
	m_trunk_root_offset->setRange(0,30);
	m_trunk_root_offset->setTickInterval(5);
	m_trunk_root_offset->setSingleStep(1);
	m_trunk_root_offset->setValue(15);
	m_label_root_offset=new QLabel(tr("-15"));
	connect(m_trunk_root_offset,SIGNAL(valueChanged(int)),this,SLOT(onSetTrunkRootOffset(int)));

	QLabel* a_label_tree_display_type=new QLabel(tr("TreeDisplayType:"));
	m_display_type=new QComboBox;
	m_display_type->addItem(tr("BranchMesh"));
	m_display_type->addItem(tr("Skeleton"));
	connect(m_display_type,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(onTreeDisplayTypeChanged(const QString&)));
	
	QGridLayout* a_main_layout=new QGridLayout;
	a_main_layout->addWidget(m_is_multilevel_enabled,0,0);
	a_main_layout->addWidget(a_label_scale,1,0);
	a_main_layout->addWidget(m_crown_truncate_ratio,1,1);
	a_main_layout->addWidget(m_label_truncate,1,2);
	a_main_layout->addWidget(a_label_rand_offset,2,0);
	a_main_layout->addWidget(m_crown_rand_offset,2,1);
	a_main_layout->addWidget(m_label_offset,2,2);
	a_main_layout->addWidget(a_label_root_offset,3,0);
	a_main_layout->addWidget(m_trunk_root_offset,3,1);
	a_main_layout->addWidget(m_label_root_offset,3,2);
	a_main_layout->addWidget(a_label_tree_display_type,4,0);
	a_main_layout->addWidget(m_display_type,4,1);

	m_global_param_group->setLayout(a_main_layout);
}

void SketchTreeMainWin::buildCrownMesh()
{
	cout<<"Start Build CrownMesh..."<<endl;

	if(!m_load_data_dialog){
		m_load_data_dialog=new LoadDataDialog;
	}
	/*m_load_data_dialog->show();
	m_load_data_dialog->raise();
	m_load_data_dialog->activateWindow();*/
	int a_ret=m_load_data_dialog->exec();
	if( a_ret==QDialog::Accepted ){
		QString a_filename=m_load_data_dialog->getFileName();
		float a_x_rot=m_load_data_dialog->getXRotation();
		float a_y_rot=m_load_data_dialog->getYRotation();
		float a_z_rot=m_load_data_dialog->getZRotation();

		if(m_silh_data_obj!=0){
			delete m_silh_data_obj;
			m_silh_data_obj=0;
		}

		m_silh_data_obj=new SilhDataObj;

		m_silh_data_obj->readDataFromFile( a_filename.toStdString().c_str(),
											 m_load_data_dialog->getSamplesPerTri(),
											 m_load_data_dialog->isPointCloudsData(),
											 a_x_rot,
											 a_y_rot,
											 a_z_rot);
		m_tree_param.m_mesh_file_name=m_silh_data_obj->getDataFileName();
		m_tree_param.m_input_type=m_silh_data_obj->getDataFileType();
		m_tree_param.m_rot_x=a_x_rot;
		m_tree_param.m_rot_y=a_y_rot;
		m_tree_param.m_rot_z=a_z_rot;
		m_gl_widget_mesh->resetGL();
		m_gl_widget_mesh->drawTreeCrown(*m_silh_data_obj);
		m_edit_silh_min_x->setText(QString::number((double)m_silh_data_obj->getMinX()));
		m_edit_silh_max_x->setText(QString::number((double)m_silh_data_obj->getMaxX()));
		m_edit_silh_min_y->setText(QString::number((double)m_silh_data_obj->getMinY()));
		m_edit_silh_max_y->setText(QString::number((double)m_silh_data_obj->getMaxY()));
		m_edit_silh_min_z->setText(QString::number((double)m_silh_data_obj->getMinZ()));
		m_edit_silh_max_z->setText(QString::number((double)m_silh_data_obj->getMaxZ()));
		m_edit_silh_vertices->setText(QString::number(m_silh_data_obj->getVertexDataSize()/3));
		cout<<"Load crown data successfully!"<<endl;

		m_is_crown_loaded=true;
		//QMessageBox::information(this,tr("Information"),tr("Load data successfully!"));
	}
	else if(a_ret==QDialog::Rejected){
		cout<<"Loading crown data canceled!"<<endl;
		//QMessageBox::information(this,tr("Information"),tr("No data loaded!"));
	}

	//Build crown data here
	/*m_silh_data_obj.readDataFromFile("00567_clamped.obj",1,true,0,0,-90.f);
	m_tree_param.m_mesh_file_name="00567_clamped.obj";
	m_tree_param.m_input_type="PointClouds";
	m_tree_param.m_rot_z=-90.f;
	m_tree_param.m_rot_x=0.f;
	m_tree_param.m_rot_y=0.f;
	m_tree_param.m_yoffset=-15.f;*/
}

void SketchTreeMainWin::buildTree()
{
	if(m_is_crown_loaded==false){
		QMessageBox::information(this,tr("Error"),tr("Please load the crown data first!"));
		return;
	}
	m_export_tree_obj->setEnabled(false);
	//Retrive user parameters
	cout<<"Variational Tree Modeler start computing..."<<endl;
	TreeType a_tree_type_l1=m_params_panel->getParams();
	cout<<"Level 1:\nP: "<<a_tree_type_l1.m_branch_habbit
		<<",N: "<<a_tree_type_l1.m_inter_node_nums<<",n: "<<a_tree_type_l1.m_leaf_nums
		<<",theta: "<<a_tree_type_l1.m_branch_trend
		<<",phi: "<<a_tree_type_l1.m_successive_angle<<endl;
	m_tree_param.m_tree_type_vect.clear();
	m_tree_param.m_alpha=2.3f;
	m_tree_param.m_beta=2.0f;
	m_tree_param.m_radius=5.f;
	m_tree_param.m_start_prune_order=3;
	m_tree_param.m_stop_prune_order=1;
	m_tree_param.m_yoffset=m_trunk_root_offset_val;//-15.f;
	m_tree_param.m_tree_type_vect.push_back(a_tree_type_l1);

	int a_level_resample_nums(0);

	if(m_is_multilevel_enabled->isChecked()){
		a_level_resample_nums=30;
		TreeType a_tree_type_l2=m_params_panel_l2->getParams();
		cout<<"Level 2:\nP: "<<a_tree_type_l2.m_branch_habbit
			<<",N: "<<a_tree_type_l2.m_inter_node_nums<<",n: "<<a_tree_type_l2.m_leaf_nums
			<<",theta: "<<a_tree_type_l2.m_branch_trend
			<<",phi: "<<a_tree_type_l2.m_successive_angle<<endl;
		m_tree_param.m_tree_type_vect.push_back(a_tree_type_l2);
	}

	//Build tree
	/*TreeModeler a_tree_modeler;
	a_tree_modeler.preProcessData(m_silh_data_obj,m_tree_param,m_crown_truncate_ratio_val,m_crown_rand_offset_val);//1.0,0.f);
	a_tree_modeler.buildTree(m_tree_param,0);

	TreeSkeleton a_skeleton;
	a_skeleton.setTreeRootNodePtr( a_tree_modeler.getTreeRoot() );
	a_skeleton.constructSkeleton();
	cout<<"Finished Tree Skeleton Construction!"<<endl;*/

	float a_data_scale(20.f);
	cout<<"Building tree...waiting............."<<endl;
	m_tree_build_thread.StartThread(m_silh_data_obj,m_tree_param,m_crown_truncate_ratio_val,m_crown_rand_offset_val,a_data_scale,a_level_resample_nums);

	/*cout<<"Start setting up OpenGL rendering...."<<endl;

	if(m_display_type->currentText()==tr("Skeleton")){
		m_gl_widget_tree->drawTreeSkeleton(a_skeleton,true);
		cout<<"Finished OpenGL rendering of tree skeleton!"<<endl;
	}
	else{
		float a_max_radius=a_skeleton.getMaxAxisRadius();

		TreeLineSkeletonMesh* a_tree_mesh=new TreeLineSkeletonMesh();
		a_tree_mesh->setMaxRadius(a_max_radius);
		a_tree_mesh->setRadiusScaleRatio(1);
		a_tree_mesh->constructMesh(a_skeleton,10,10);
		m_gl_widget_tree->drawTreeSurface(*a_tree_mesh);
		cout<<"Finished OpenGL Rendering of tree branch mesh!"<<endl;
		delete a_tree_mesh;
	}*/


	//m_gl_widget->drawTreeSkeleton(a_skeleton,true);
	/*TreeCurveSkeletonMesh* a_tree_mesh=new TreeCurveSkeletonMesh();
	a_tree_mesh->setRadiusScaleRatio(0.2f);
	a_tree_mesh->constructMesh(a_skeleton,20,20);*/
	/*float a_max_radius=a_skeleton.getMaxAxisRadius();
	
	TreeLineSkeletonMesh* a_tree_mesh=new TreeLineSkeletonMesh();
	a_tree_mesh->setMaxRadius(a_max_radius);
	a_tree_mesh->setRadiusScaleRatio(1);
	a_tree_mesh->constructMesh(a_skeleton,10,10);
	
	m_gl_widget_tree->drawTreeSurface(*a_tree_mesh);

	cout<<"Finished opengl rendering!"<<endl;

	delete a_tree_mesh;*/

}

void SketchTreeMainWin::onExportTree()
{
	QString a_saved_file_name=QFileDialog::getSaveFileName(this,tr("Save Tree Model Obj"));
	ObjExporter a_obj_exporter;
	bool b_saved=a_obj_exporter.writeObjFile(a_saved_file_name.toStdString().c_str(),m_tree_mesh_line->getTriList(),m_tree_mesh_line->getVertList());
	if(b_saved){
		QMessageBox::information(this,tr("Info"),tr("Export tree model obj file successfully!"));
	}
	else{
		QMessageBox::information(this,tr("Info"),tr("Export tree model obj file failed!"));
	}
}

void SketchTreeMainWin::onFinishedBuildTree(/*TreeSkeleton* aTreeSkeleton*/)
{
	cout<<"\nFinished Tree Building!!"<<endl;
	cout<<"Start setting up OpenGL rendering...."<<endl;

	TreeSkeleton* a_tree_skeleton(0);
	a_tree_skeleton=m_tree_build_thread.getTreeSkeleton();

	//assert(a_tree_skeleton!=0);
	if(a_tree_skeleton==0){
		cout<<"Empty skeleton"<<endl;
		return;
	}

	if(m_display_type->currentText()==tr("Skeleton")){
		m_gl_widget_tree->drawTreeSkeleton(*a_tree_skeleton,true);
		cout<<"Finished OpenGL rendering of tree skeleton!"<<endl;
	}
	else{
		float a_max_radius=a_tree_skeleton->getMaxAxisRadius();
		if(m_tree_mesh_line!=0)
			delete m_tree_mesh_line;
		m_tree_mesh_line=new TreeLineSkeletonMesh;
		m_tree_mesh_line->setMaxRadius(a_max_radius);
		m_tree_mesh_line->setRadiusScaleRatio(1);
		m_tree_mesh_line->constructMesh(*a_tree_skeleton,10,10);
		m_gl_widget_tree->drawTreeSurface(*m_tree_mesh_line);
		cout<<"Finished OpenGL Rendering of tree branch mesh!"<<endl;
		m_export_tree_obj->setEnabled(true);
	}
}

void SketchTreeMainWin::onTreeDisplayTypeChanged(const QString& aText)
{
	cout<<"Tree display mode switched to "<<aText.toStdString()<<endl;
	TreeSkeleton* a_tree_skeleton(0);
	a_tree_skeleton=m_tree_build_thread.getTreeSkeleton();
	if(a_tree_skeleton!=0){
		if(aText==tr("Skeleton")){
			cout<<"Draw tree skeleton......"<<endl;
			m_gl_widget_tree->drawTreeSkeleton(*a_tree_skeleton,true);
			cout<<"Finished OpenGL rendering of tree skeleton!"<<endl;
		}
		else{
			cout<<"Draw tree branch mesh......"<<endl;
			float a_max_radius=a_tree_skeleton->getMaxAxisRadius();
			if(m_tree_mesh_line!=0)
				delete m_tree_mesh_line;
			m_tree_mesh_line=new TreeLineSkeletonMesh;
			m_tree_mesh_line->setMaxRadius(a_max_radius);
			m_tree_mesh_line->setRadiusScaleRatio(1);
			m_tree_mesh_line->constructMesh(*a_tree_skeleton,10,10);
			m_gl_widget_tree->drawTreeSurface(*m_tree_mesh_line);
			cout<<"Finished OpenGL Rendering of tree branch mesh!"<<endl;
			m_export_tree_obj->setEnabled(true);
		}
	}
}

void SketchTreeMainWin::onSetCrownTruncate(int aVal)
{
	m_crown_truncate_ratio_val=float(aVal)/20.f;
	m_label_truncate->setText(QString::number((double)m_crown_truncate_ratio_val));
}

void SketchTreeMainWin::onSetCrownRandOffset(int aVal)
{
	m_crown_rand_offset_val=float(aVal)/10.f;
	m_label_offset->setText( QString::number((double)m_crown_rand_offset_val));
}

void SketchTreeMainWin::onSetTrunkRootOffset(int aVal)
{
	m_trunk_root_offset_val=-(float)aVal;
	QString a_text=QString::number((double)m_trunk_root_offset_val);
	m_label_root_offset->setText(a_text);
}
