#ifndef POINTS_DATA_SETTINGS_BOX_H
#define POINTS_DATA_SETTINGS_BOX_H

#include <QGroupBox>

class QLabel;
class QLineEdit;

class PointsDataSettingsBox : public QGroupBox
{
	Q_OBJECT
public:
	PointsDataSettingsBox(QWidget* aParent=0);
	QString getEditSample() const;
	QString getEditXRot() const;
	QString getEditYRot() const;
	QString getEditZRot() const;
private:
	QLabel* m_label_sample;
	QLabel* m_label_x_rot;
	QLabel* m_label_y_rot;
	QLabel* m_label_z_rot;
	QLineEdit* m_edit_sample;
	QLineEdit* m_edit_x_rot;
	QLineEdit* m_edit_y_rot;
	QLineEdit* m_edit_z_rot;
};

#endif