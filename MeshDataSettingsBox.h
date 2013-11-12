#ifndef MESH_DATA_SETTINGS_BOX_H
#define MESH_DATA_SETTINGS_BOX_H

#include <QGroupBox>

class QLabel;
class QLineEdit;

class MeshDataSettingsBox : public QGroupBox
{
	Q_OBJECT
public:
	MeshDataSettingsBox(QWidget* aParent=0);
	QString getLineEditStr() const;
private:
	QLabel* m_label;
	QLineEdit* m_line_edit;

};


#endif