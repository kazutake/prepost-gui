#ifndef PREPROCESSORBCSETTINGGROUPDATAITEM_H
#define PREPROCESSORBCSETTINGGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>

class PreProcessorBCDataItem;
class PreProcessorBCSettingDataItem;
class Grid;
class QAction;

class PreProcessorBCSettingGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorBCSettingGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorBCSettingGroupDataItem();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void informGridUpdate();
	const QList<PreProcessorBCSettingDataItem*> conditions() const;
	PreProcessorBCSettingDataItem* condition(const QString& type, int num) const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	void updateBCMenu(PreProcessorBCSettingDataItem* item);
	void setupAddActions();
	const QList<QAction*>& addActions() const {return m_addActions;}
	QAction* dummyEditAction() const {return m_dummyEditAction;}
	QAction* dummyDeleteAction() const {return m_dummyDeleteAction;}

public slots:
	void executeMapping(bool noDraw = false);
	void updateItems();
	void loadItems();
	void addCondition();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QMap<PreProcessorBCDataItem*, PreProcessorBCSettingDataItem*> m_itemMap;
	QList<QAction*> m_addActions;
	QAction* m_dummyEditAction;
	QAction* m_dummyDeleteAction;
};

#endif // PREPROCESSORBCSETTINGGROUPDATAITEM_H
