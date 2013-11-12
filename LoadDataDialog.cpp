#include "LoadDataDialog.h"

#include <QtGui>

#include "MeshDataSettingsBox.h"
#include "PointsDataSettingsBox.h"

//#include "TreeModeler/silh_data_obj.h"
//using namespace SilhouetteTree;

LoadDataDialog::LoadDataDialog(QWidget* aParent/* =0 */)
	:QDialog(aParent),m_x_rotation(0.f),m_y_rotation(0.f),
	m_z_rotation(0.f)//,m_silh_data_obj(0)
{
	m_filename_label=new QLabel(tr("FileName:"));
	m_filename_edit=new QLineEdit();
	m_open_file_btn=new QPushButton(tr("..."));

	connect(m_open_file_btn,SIGNAL(clicked()),this,SLOT(openFile()));

	m_data_type_label=new QLabel(tr("DataType:"));
	m_data_type_combox=new QComboBox();
	m_data_type_combox->addItem(tr("Mesh"));
	m_data_type_combox->addItem(tr("PointClouds"));

	m_mesh_box=new MeshDataSettingsBox;
	m_points_box=new PointsDataSettingsBox;

	m_stacked_widget=new QStackedWidget;
	m_stacked_widget->addWidget(m_mesh_box);
	m_stacked_widget->addWidget(m_points_box);

	connect(m_data_type_combox,SIGNAL(activated(int)),m_stacked_widget,SLOT(setCurrentIndex(int)));

	m_ok_btn=new QPushButton(tr("Ok"));
	m_cancel_btn=new QPushButton(tr("Cancel"));

	connect(m_ok_btn,SIGNAL(clicked()),this,SLOT(accept()));
	connect(m_cancel_btn,SIGNAL(clicked()),this,SLOT(reject()));

	QHBoxLayout* a_h_layout1=new QHBoxLayout;
	a_h_layout1->addWidget(m_filename_label);
	a_h_layout1->addWidget(m_filename_edit);
	a_h_layout1->addWidget(m_open_file_btn);

	QHBoxLayout* a_h_layout2=new QHBoxLayout;
	a_h_layout2->addWidget(m_data_type_label);
	a_h_layout2->addWidget(m_data_type_combox);

	QHBoxLayout* a_h_layout3=new QHBoxLayout;
	a_h_layout3->addWidget(m_ok_btn);
	a_h_layout3->addWidget(m_cancel_btn);

	QVBoxLayout* a_central_layout=new QVBoxLayout;
	a_central_layout->addLayout(a_h_layout1);
	a_central_layout->addLayout(a_h_layout2);
	a_central_layout->addWidget(m_stacked_widget);
	a_central_layout->addLayout(a_h_layout3);

	setLayout(a_central_layout);
	setWindowTitle(tr("Load Data"));
}

LoadDataDialog::~LoadDataDialog()
{
	/*if( m_silh_data_obj )
		delete m_silh_data_obj;*/
}

void LoadDataDialog::openFile()
{
	m_data_file_name=QFileDialog::getOpenFileName(this,tr("Select a data file"));
	m_filename_edit->clear();
	m_filename_edit->setText(m_data_file_name);

}

void LoadDataDialog::onOkBtnClicked()
{
	/*if(m_silh_data_obj){
		delete m_silh_data_obj;
		m_silh_data_obj=new SilhDataObj();
	}

	m_silh_data_obj->readDataFromFile( m_data_file_name.toStdString().c_str(),
										m_samples_per_tri,
										m_is_point_clouds,
										m_x_rotation,
										m_y_rotation,
										m_z_rotation);*/
	/*if( m_data_type_combox->currentText()==tr("Mesh") )
		m_is_point_clouds=false;
	else
		m_is_point_clouds=true;

	emit QDialog::accepted();*/
}

void LoadDataDialog::onCancelBtnClicked()
{
	//emit QDialog::rejected();
}

void LoadDataDialog::setDataType(const QString& aTextStr)
{

}

void LoadDataDialog::reject()
{
	QDialog::reject();
}

void LoadDataDialog::accept()
{
	if( m_filename_edit->text().isEmpty() ){
		QMessageBox::information(this,tr("Warning"),tr("Empty data file name! No file will be loaded!"));
		QDialog::reject();
	}
	else{
		if( m_data_type_combox->currentText()==tr("Mesh") ){
			m_is_point_clouds=false;
			m_samples_per_tri=m_mesh_box->getLineEditStr().toInt();
		}
		else{
			m_is_point_clouds=true;
			m_samples_per_tri=m_points_box->getEditSample().toInt();
			m_x_rotation=m_points_box->getEditXRot().toFloat();
			m_y_rotation=m_points_box->getEditYRot().toFloat();
			m_z_rotation=m_points_box->getEditZRot().toFloat();
		}

		QDialog::accept();
	}

	
}