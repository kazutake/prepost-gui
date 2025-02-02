#ifndef POSTBASEITERATIVESERIESDATACONTAINER_H
#define POSTBASEITERATIVESERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postseriesdatacontainer.h"
#include "postsolutioninfo.h"
#include <QString>

class GUICOREDLL_EXPORT PostBaseIterativeSeriesDataContainer : public PostSeriesDataContainer
{

public:
	PostBaseIterativeSeriesDataContainer(PostSolutionInfo::Dimension dim, const QString& biName, ProjectDataItem* parent);

protected:
	bool loadData(const int fn) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	QString m_baseIterativeName;
};

#endif // POSTBASEITERATIVESERIESDATACONTAINER_H
