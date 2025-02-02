#ifndef PROJECTPOSTPROCESSORS_H
#define PROJECTPOSTPROCESSORS_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include <QList>

class QMdiSubWindow;
class PostProcessorWindowProjectDataItem;
class PostProcessorWindowFactoryInterface;

class GUICOREDLL_EXPORT ProjectPostProcessors : public ProjectDataItem
{

public:
	ProjectPostProcessors(ProjectDataItem* parent);
	~ProjectPostProcessors();
	QMdiSubWindow* add(PostProcessorWindowProjectDataItem* newitem);
	void requestDelete(PostProcessorWindowProjectDataItem* item);
	void loadFromProjectMainFile(const QDomNode& node, bool import = false);
	int windowCount() const;
	void setFactory(PostProcessorWindowFactoryInterface* factory);
	void applyOffset(double x_diff, double y_diff);

protected:
	// Never used.
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doLoadFromProjectMainFile(const QDomNode& node, bool import = false);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QList<PostProcessorWindowProjectDataItem*> m_postProcessorWindows;
	PostProcessorWindowFactoryInterface* m_factory;
};

#endif // PROJECTPOSTPROCESSORS_H
