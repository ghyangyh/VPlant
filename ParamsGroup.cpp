#include <QtGui>
#include "ParamsGroup.h"

using namespace SilhouetteTree;

ParamsGroup::ParamsGroup(const QString& aGroupTitle,QWidget* aParent):QGroupBox(aGroupTitle,aParent)
{
	m_p_label=new QLabel(tr("P    :"));
	m_N_label=new QLabel(tr("N    :"));
	m_n_label=new QLabel(tr("n    :"));
	m_theta_label=new QLabel(tr("theta:"));
	m_phi_label=new QLabel(tr("phi  :"));

	m_p_value_label=new QLabel;
	m_p_value_label->setNum(0);
	m_params.m_branch_habbit=0;

	m_N_value_label=new QLabel;
	m_N_value_label->setNum(2);
	m_params.m_inter_node_nums=3;

	m_n_value_label=new QLabel;
	m_n_value_label->setNum(1);
	m_params.m_leaf_nums=1;

	m_theta_value_label=new QLabel;
	m_theta_value_label->setNum(15);
	m_params.m_branch_trend=15;

	m_phi_value_label=new QLabel;
	m_phi_value_label->setNum(0);
	m_params.m_successive_angle=0;

	m_params.m_resample_param=0;
	m_params.m_sample_ratio=1.f;
	m_params.m_subtree_ratio=1.f;

	m_p_slider=new QSlider(Qt::Horizontal);
	m_p_slider->setFocusPolicy(Qt::StrongFocus);
	m_p_slider->setTickPosition(QSlider::TicksAbove);
	m_p_slider->setRange(0,10);
	m_p_slider->setTickInterval(1);
	m_p_slider->setSingleStep(1);
	
	m_N_slider=new QSlider(Qt::Horizontal);
	m_N_slider->setFocusPolicy(Qt::StrongFocus);
	m_N_slider->setTickPosition(QSlider::TicksAbove);
	m_N_slider->setRange(2,13);
	m_N_slider->setTickInterval(1);
	m_N_slider->setSingleStep(1);

	m_n_slider=new QSlider(Qt::Horizontal);
	m_n_slider->setFocusPolicy(Qt::StrongFocus);
	m_n_slider->setTickPosition(QSlider::TicksAbove);
	m_n_slider->setRange(1,5);
	m_n_slider->setTickInterval(1);
	m_n_slider->setSingleStep(1);

	m_theta_slider=new QSlider(Qt::Horizontal);
	m_theta_slider->setFocusPolicy(Qt::StrongFocus);
	m_theta_slider->setTickPosition(QSlider::TicksAbove);
	m_theta_slider->setRange(15,90);
	m_theta_slider->setTickInterval(15);
	m_theta_slider->setSingleStep(1);

	m_phi_slider=new QSlider(Qt::Horizontal);
	m_phi_slider->setFocusPolicy(Qt::StrongFocus);
	m_phi_slider->setTickPosition(QSlider::TicksAbove);
	m_phi_slider->setRange(0,360);
	m_phi_slider->setTickInterval(15);
	m_phi_slider->setSingleStep(1);

	connect(m_p_slider,SIGNAL(valueChanged(int)),this,SLOT(setPValue(int)));
	connect(m_N_slider,SIGNAL(valueChanged(int)),this,SLOT(setNValue(int)));
	connect(m_n_slider,SIGNAL(valueChanged(int)),this,SLOT(setnValue(int)));
	connect(m_theta_slider,SIGNAL(valueChanged(int)),this,SLOT(setThetaValue(int)));
	connect(m_phi_slider,SIGNAL(valueChanged(int)),this,SLOT(setPhiValue(int)));

	QGridLayout* a_layout=new QGridLayout;
	a_layout->addWidget(m_p_label,0,0);
	a_layout->addWidget(m_N_label,1,0);
	a_layout->addWidget(m_n_label,2,0);
	a_layout->addWidget(m_phi_label,3,0);
	a_layout->addWidget(m_theta_label,4,0);
	a_layout->addWidget(m_p_slider,0,1);
	a_layout->addWidget(m_N_slider,1,1);
	a_layout->addWidget(m_n_slider,2,1);
	a_layout->addWidget(m_phi_slider,3,1);
	a_layout->addWidget(m_theta_slider,4,1);

	a_layout->addWidget(m_p_value_label,0,2);
	a_layout->addWidget(m_N_value_label,1,2);
	a_layout->addWidget(m_n_value_label,2,2);
	a_layout->addWidget(m_phi_value_label,3,2);
	a_layout->addWidget(m_theta_value_label,4,2);

	setLayout(a_layout);
}

void ParamsGroup::setPValue(int aValue)
{
	double a_value=double(aValue)/10.0;
	m_p_value_label->setNum(a_value);
	m_params.m_branch_habbit=float(a_value);
}

void ParamsGroup::setNValue(int aValue)
{
	m_N_value_label->setNum(aValue);
	m_params.m_inter_node_nums=aValue;
}

void ParamsGroup::setnValue(int aValue)
{
	m_n_value_label->setNum(aValue);
	m_params.m_leaf_nums=aValue;
}

void ParamsGroup::setThetaValue(int aValue)
{
	m_theta_value_label->setNum(aValue);
	m_params.m_branch_trend=aValue;
}

void ParamsGroup::setPhiValue(int aValue)
{
	m_phi_value_label->setNum(aValue);
	m_params.m_successive_angle=aValue;
}
