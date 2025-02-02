#ifndef POSTSOLUTIONINFO_H
#define POSTSOLUTIONINFO_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"
#include "../solverdef/solverdefinition.h"
#include "postdataexportdialog.h"
#include "postexportsetting.h"

#include <QList>
#include <QMap>
#include <QStringList>

class CgnsFileOpener;
class PostCalculatedResult;
class PostDataContainer;
class PostIterationSteps;
class PostTimeSteps;
class PostZoneDataContainer;

class QDomElement;

class GUICOREDLL_EXPORT PostSolutionInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	enum Dimension {dim1D, dim2D, dim3D};
	/// Constructor
	PostSolutionInfo(ProjectDataItem* parent);
	~PostSolutionInfo();
	SolverDefinition::IterationType iterationType() const;
	void setIterationType(SolverDefinition::IterationType type);
	PostIterationSteps* iterationSteps() const;
	PostTimeSteps* timeSteps() const;
	int currentStep() const;
	double currentTimeStep();
	/// Returns true if the current CGNS file has results.
	bool hasResults();
	/// Emit signal cgnsStepsUpdated().
	void informCgnsSteps();
	void loadFromCgnsFile(const int fn) override;
	void closeCgnsFile() override;
	const QList<PostZoneDataContainer*>& zoneContainers1D() const;
	const QList<PostZoneDataContainer*>& zoneContainers2D() const;
	const QList<PostZoneDataContainer*>& zoneContainers3D() const;
	const QList<PostZoneDataContainer*>& zoneContainers(Dimension dim) const;
	PostZoneDataContainer* zoneContainer1D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer2D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer3D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer(Dimension dim, const std::string& zoneName) const;
	PostZoneDataContainer* firstZoneContainer() const;

	bool isDataAvailable() const;
	bool isDataAvailable1D() const;
	bool isDataAvailable2D() const;
	bool isDataAvailable3D() const;
	bool isDataAvailableBase() const;
	static int toIntDimension(Dimension dim);
	static Dimension fromIntDimension(int dim);
	bool open();
	void close();

	const PostExportSetting& exportSetting() const;
	const QString& particleExportPrefix() const;
	void setExportSetting(const PostExportSetting& setting);
	void setParticleExportPrefix(const QString& prefix);

	/// File ID that can be used with cgnslib functions.
	int fileId() const;
	void setCalculatedResultDisabled(bool disabled);

	void exportCalculationResult(const std::string& folder, const std::string& prefix, const std::vector<int> steps, PostDataExportDialog::Format format);

	void applyOffset(double x_diff, double y_diff);

protected:
	void timerEvent(QTimerEvent*) override;
	bool innerSetupZoneDataContainers(int fn, int dimiension, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap, QMap<std::string, std::vector<PostCalculatedResult*> > *results);
//	bool innerSetupDummy3DZoneDataContainers(int fn, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap);
	virtual void doLoadFromProjectMainFile(const QDomNode& node) override;
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	static void clearContainers(QList<PostZoneDataContainer*>* conts);

public slots:
	void informSolverStart();
	void informSolverFinish();
	bool setCurrentStep(unsigned int step, int fn = 0);
	void checkCgnsStepsUpdate();
	void exportCalculationResult();

private slots:
	void informStepsUpdated();

signals:
	void currentStepUpdated();
	void updated();
	void allPostProcessorsUpdated();
	void cgnsTimeStepsUpdated(const QList<double>& steps);
	void cgnsIterationStepsUpdated(const QList<int>& steps);
	void cgnsStepsUpdated(int fn);
	void zoneList1DUpdated();
	void zoneList2DUpdated();
	void zoneList3DUpdated();

private:
	bool stepsExist() const;
	void setupZoneDataContainers(int fn);
	void checkBaseIterativeDataExist(int fn);
	void loadCalculatedResult();
	void clearCalculatedResults(QMap<std::string, std::vector<PostCalculatedResult*> >* results);
	static const int TIMERINTERVAL = 500;
	SolverDefinition::IterationType m_iterationType;
	PostIterationSteps* m_iterationSteps;
	PostTimeSteps* m_timeSteps;
	int m_currentStep;
	QList<PostZoneDataContainer*> m_zoneContainers1D;
	QList<PostZoneDataContainer*> m_zoneContainers2D;
	QList<PostZoneDataContainer*> m_zoneContainers3D;
	bool m_baseIterativeDataExists;
	QList<PostDataContainer*> m_otherContainers;
	int m_timerId;
	CgnsFileOpener* m_opener;

	std::vector<std::string> m_zoneNames1D;
	std::vector<std::string> m_zoneNames2D;
	std::vector<std::string> m_zoneNames3D;

	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap1D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap2D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap3D;

	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults1D;
	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults2D;
	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults3D;

	PostDataExportDialog::Format m_exportFormat;
	bool m_disableCalculatedResult;

	PostExportSetting m_exportSetting;
	QString m_particleExportPrefix;

	QDomElement* m_loadedElement;
};

#endif // POSTSOLUTIONINFO_H
