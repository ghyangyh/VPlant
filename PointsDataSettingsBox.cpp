#include "PointsDataSettingsBox.h"

#include <QtGui>

PointsDataSettingsBox::PointsDataSettingsBox(QWidget* aParent):QGroupBox(tr("Points Settings"),aParent)
{
	m_label_sample=new QLabel(tr("Samples per tri:"));
	m_label_x_rot=new QLabel(tr("X Rotation:"));
	m_label_y_rot=new QLabel(tr("Y Rotation:"));
	m_label_z_rot=new QLabel(tr("Z Rotation:"));

	m_edit_sample=new QLineEdit(tr("0"));
	m_edit_x_rot=new QLineEdit(tr("0"));
	m_edit_y_rot=new QLineEdit(tr("0"));
	m_edit_z_rot=new QLineEdit(tr("0"));

	QGridLayout* a_central_layout=new QGridLayout;
	a_central_layout->addWidget(m_label_sample,0,0);
	a_central_layout->addWidget(m_edit_sample,0,1);

	a_central_layout->addWidget(m_label_x_rot,1,0);
	a_central_layout->addWidget(m_edit_x_rot,1,1);

	a_central_layout->addWidget(m_label_y_rot,2,0);
	a_central_layout->addWidget(m_edit_y_rot,2,1);

	a_central_layout->addWidget(m_label_z_rot,3,0);
	a_central_layout->addWidget(m_edit_z_rot,3,1);

	setLayout(a_central_layout);
}

QString PointsDataSettingsBox::getEditSample() const
{
	return m_edit_sample->text();
}
QString PointsDataSettingsBox::getEditXRot() const
{
	return m_edit_x_rot->text();
}
QString PointsDataSettingsBox::getEditYRot() const
{
	return m_edit_y_rot->text();
}
QString PointsDataSettingsBox::getEditZRot() const
{
	return m_edit_z_rot->text();
}