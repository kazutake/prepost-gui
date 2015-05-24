#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/structuredgridregion.h>

#include <QMap>
#include <QColor>
#include <QList>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStreamTracer.h>
#include <vtkClipPolyData.h>
#include <vtkPolyData.h>

class Post3dWindowNodeVectorStreamlineDataItem;
class Post3dWindowStructuredGridStreamlineSelectSolution;
class Post3dWindowStreamlineStructuredSetProperty;

class Post3dWindowNodeVectorStreamlineGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowNodeVectorStreamlineGroupDataItem(Post3dWindowDataItem* parent);
	virtual ~Post3dWindowNodeVectorStreamlineGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void updateZDepthRangeItemCount();
	virtual void assignActorZValues(const ZDepthRange& range);
	void update();
public slots:
	void exclusivelyCheck(Post3dWindowNodeVectorStreamlineDataItem*);
protected:
	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	vtkPointSet* getRegion();
	void setCurrentSolution(const QString& currentSol);
	const QString& currentSolution() {return m_currentSolution;}
	void innerUpdateZScale(double zscale);
	void applyZScale();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setupStreamTracer(vtkStreamTracer* tracer);

	QString m_currentSolution;
	StructuredGridRegion::RegionMode m_regionMode;
	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;

	QList<vtkActor*> m_streamlineActors;
	QList<vtkDataSetMapper*> m_streamlineMappers;
	QList<vtkStreamTracer*> m_streamTracers;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
private:
	void setDefaultValues();

	double m_zScale;
public:
	friend class Post3dWindowStructuredGridStreamlineSelectSolution;
	friend class Post3dWindowStreamlineStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
