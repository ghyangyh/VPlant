#include "MeshDataSettingsBox.h"

#include <QtGui>

MeshDataSettingsBox::MeshDataSettingsBox(QWidget* aParent):QGroupBox(tr("Mesh Settings"),aParent)
{
	m_label=new QLabel(tr("SamplesPerTri:"));
	m_line_edit=new QLineEdit(tr("0"));

	QHBoxLayout* m_central_layout=new QHBoxLayout;
	m_central_layout->addWidget(m_label);
	m_central_layout->addWidget(m_line_edit);

	setLayout(m_central_layout);
}

QString MeshDataSettingsBox::getLineEditStr() const
{
	return m_line_edit->text();
}