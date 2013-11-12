#ifndef SKETCHTREEMAINWIN_H
#define SKETCHTREEMAINWIN_H


#include <QtGui/QWidget>
#include "ui_sketchtreemainwin.h"

#include "TreeModeler/tree_modeler.h"
#include "TreeModeler/silh_data_obj.h"
#include "TreeModeler/tree_config_loader.h"

#include "TreeBuildThread.h"

//class TreeBuildThread;

class GLWidget;
class ParamsGroup;
class QPushButton;
class LoadDataDialog;
class QComboBox;
class QLabel;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class QSlider;
class QTextEdit;
class QDebugStream;
class QStackedWidget;

#include "TreeModeler/tree_skeleton.h"
#include "TreeModeler/tree_line_skeleton_mesh.h"
//Q_DECLARE_METATYPE(SilhouetteTree::TreeSkeleton*)

class SketchTreeMainWin : public QWidget
{
	Q_OBJECT
	
public:
	SketchTreeMainWin(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SketchTreeMainWin();
	QTextEdit* getTextEdit() const{
		return m_debug_window;
	}
protected:
	void constructSilhViewGroup();
	void constructGlobalParamGroup();
	void constructMultilevelParam();
protected slots:
	void buildCrownMesh();
	void buildTree();
	void onExportTree();
	void onSetCrownTruncate(int aVal);
	void onSetCrownRandOffset(int aVal);
	void onSetTrunkRootOffset(int aVal);
	void onFinishedBuildTree(/*SilhouetteTree::TreeSkeleton* aTreeSkeleton*/);
	void onTreeDisplayTypeChanged(const QString& aText);
private:
	Ui::SketchTreeMainWinClass ui;
	GLWidget* m_gl_widget_tree;
	GLWidget* m_gl_widget_mesh;
	ParamsGroup* m_params_panel;
	ParamsGroup* m_params_panel_l2;
	QPushButton* m_build_mesh_btn;
	QPushButton* m_build_tree_btn;
	QComboBox* m_tree_param_level_box;
	LoadDataDialog* m_load_data_dialog;
	QStackedWidget* m_stacked_param;
	//Crown Mesh Preview
	QGroupBox* m_silh_mesh_view_group;
	QLineEdit* m_edit_silh_min_x;
	QLineEdit* m_edit_silh_max_x;
	QLineEdit* m_edit_silh_min_y;
	QLineEdit* m_edit_silh_max_y;
	QLineEdit* m_edit_silh_min_z;
	QLineEdit* m_edit_silh_max_z;
	QLineEdit* m_edit_silh_vertices;
	//Tree global param settings
	QGroupBox* m_global_param_group;
	QCheckBox* m_is_multilevel_enabled;
	QSlider* m_crown_truncate_ratio;
	QSlider* m_crown_rand_offset;
	QSlider* m_trunk_root_offset;
	QLabel* m_label_truncate;
	QLabel* m_label_offset;
	QLabel* m_label_root_offset;
	QComboBox* m_display_type;
	float m_crown_truncate_ratio_val;
	bool m_multilevel_enabled;
	bool m_is_display_skeleton;
	float m_crown_rand_offset_val;
	float m_trunk_root_offset_val;

	QTextEdit* m_debug_window;
	QDebugStream* m_qout;

	QPushButton* m_export_tree_obj;

	//SilhouetteTree::TreeModeler m_tree_modeler;
	SilhouetteTree::SilhDataObj* m_silh_data_obj;
	SilhouetteTree::TreeParam m_tree_param;
	SilhouetteTree::TreeLineSkeletonMesh* m_tree_mesh_line;

	TreeBuildThread m_tree_build_thread;
	bool m_is_crown_loaded;
	
};

#endif // SKETCHTREEMAINWIN_H
