#ifndef POST3DWINDOWARROWGROUPSETTINGDIALOG_H
#define POST3DWINDOWARROWGROUPSETTINGDIALOG_H

#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowfacedataitem.h"

#include <QDialog>
#include <QMap>

#include <string>
#include <vector>

class ArrowSettingContainer;
class PostZoneDataContainer;
class QListWidgetItem;

namespace Ui
{
	class Post3dWindowArrowGroupSettingDialog;
}

class Post3dWindowArrowGroupSettingDialog : public QDialog
{
	Q_OBJECT

public:
	Post3dWindowArrowGroupSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowArrowGroupSettingDialog();

	void setZoneData(PostZoneDataContainer* zoneData);
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);
	const QMap<QString, Post3dWindowFaceDataItem::Setting>& faceMap();

	ArrowSettingContainer setting() const;
	void setSetting(const ArrowSettingContainer& shape);

public slots:
	void addFaceSetting();
	void removeFaceSetting();
	void switchFaceSetting(QListWidgetItem* current, QListWidgetItem* previous);

private slots:
	void checkSelectedNumber();
	void allSamplingToggled(bool toggled);
	void updateFaceMap();

private:
	std::vector<std::string> m_targets;
	std::vector<std::string> m_scalars;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_faceMap;
	bool m_isRemoving;

	Ui::Post3dWindowArrowGroupSettingDialog* ui;
};

#endif // POST3DWINDOWARROWGROUPSETTINGDIALOG_H
