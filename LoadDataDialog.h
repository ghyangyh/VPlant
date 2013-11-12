#ifndef _LOAD_DATA_DIALOG_H
#define _LOAD_DATA_DIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QStackedWidget;
//class SilhDataObj;
class MeshDataSettingsBox;
class PointsDataSettingsBox;

class LoadDataDialog : public QDialog
{
	Q_OBJECT
public:
	LoadDataDialog(QWidget* aParent=0);
	~LoadDataDialog();
public:
	QString getFileName() const{
		return m_data_file_name;
	}
	float getXRotation() const{
		return m_x_rotation;
	}

	float getYRotation() const{
		return m_y_rotation;
	}

	float getZRotation() const{
		return m_z_rotation;
	}
	int getSamplesPerTri() const{
		return m_samples_per_tri;
	}

	bool isPointCloudsData() const{
		return m_is_point_clouds;
	}
public slots:
	void openFile();
	void onOkBtnClicked();
	void onCancelBtnClicked();
	void setDataType(const QString& aTextStr);
	void accept();
	void reject();
private:
	QLabel* m_filename_label;
	QLabel* m_data_type_label;
	QLineEdit* m_filename_edit;
	QPushButton* m_open_file_btn;
	QPushButton* m_ok_btn;
	QPushButton* m_cancel_btn;
	QComboBox* m_data_type_combox;
	QStackedWidget* m_stacked_widget;
	QString m_data_file_name;
	//SilhDataObj* m_silh_data_obj;
	MeshDataSettingsBox* m_mesh_box;
	PointsDataSettingsBox* m_points_box;
	float m_x_rotation,m_y_rotation,m_z_rotation;
	int m_samples_per_tri;
	bool m_is_point_clouds;

};

#endif