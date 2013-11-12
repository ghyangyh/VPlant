#ifndef PARAMS_GROUP_H
#define PARAMS_GROUP_H

#include <QGroupBox>

class QSlider;
class QLabel;

#include "TreeModeler/tree_builder.h"

/*struct TreeModelParams
{
	float m_p;
	int m_N;
	int m_n;
	int m_theta;
	int m_phi;
};*/

class ParamsGroup : public QGroupBox
{
	Q_OBJECT
public:
	ParamsGroup(const QString& aGroupTitle,QWidget* aParent=0);
public:
	SilhouetteTree::TreeType getParams() const{
		return m_params;
	}
protected slots:
	void setPValue(int aValue);
	void setNValue(int aValue);
	void setnValue(int aValue);
	void setThetaValue(int aValue);
	void setPhiValue(int aValue);
private:
	QSlider* m_p_slider;
	QSlider* m_N_slider;
	QSlider* m_n_slider;
	QSlider* m_theta_slider;
	QSlider* m_phi_slider;
	QLabel* m_p_label;
	QLabel* m_N_label;
	QLabel* m_n_label;
	QLabel* m_theta_label;
	QLabel* m_phi_label;
	QLabel* m_p_value_label;
	QLabel* m_N_value_label;
	QLabel* m_n_value_label;
	QLabel* m_theta_value_label;
	QLabel* m_phi_value_label;
	SilhouetteTree::TreeType m_params;
};


#endif