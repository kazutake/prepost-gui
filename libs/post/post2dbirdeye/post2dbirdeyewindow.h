#ifndef POST2DBIRDEYEWINDOW_H
#define POST2DBIRDEYEWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QVector2D>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/windowwithobjectbrowserinterface.h>

class QAction;
class QToolBar;
class Post2dBirdEyeObjectBrowser;
class Post2dBirdEyeWindowDataModel;
class Post2dBirdEyeWindowProjectDataItem;
class Post2dBirdEyeWindowActionManager;
class Post2dBirdEyeWindowGraphicsView;
class Post2dBirdEyeWindowEditBackgroundColorCommand;

/// This class represents the two-dimensional post-processing window.
class Post2dBirdEyeWindow :
	public PostProcessorWindow,
	public AdditionalMenuWindowI,
	public WindowWithObjectBrowserInterface
{
	Q_OBJECT

public:
	Post2dBirdEyeWindow(QWidget* parent, int index, Post2dBirdEyeWindowProjectDataItem* pdi);
	~Post2dBirdEyeWindow();

	/// Informed that CGNS file is switched.
	void handleCgnsSwitch() {}
	/// switch to the new index.
	void changeIndex(uint /*newindex*/) {}
	QPixmap snapshot() override;
	vtkRenderWindow* getVtkRenderWindow() const override;

	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;

	ObjectBrowser* objectBrowser() const override;
	int index() const {return m_index;}

public slots:
	void cameraFit();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();
	void cameraXYPlane();
	void cameraYZPlane();
	void cameraZXPlane();
	void cameraParallelProjection();
	void cameraPerspectiveProjection();
	void updateProjectionMenu(QAction* parallel, QAction* perspective);
	void editBackgroundColor();
	void editZScale();

signals:
	void worldPositionChangedForStatusBar(const QVector2D& pos);

private:
	void init();
	/// Background color
	const QColor backgroundColor() const;
	/// Set background color;
	void setBackgroundColor(QColor& c);
	void setupDefaultGeometry(int index) override;
	Post2dBirdEyeObjectBrowser* m_objectBrowser;
	Post2dBirdEyeWindowDataModel* m_dataModel;
	Post2dBirdEyeWindowActionManager* m_actionManager;
	QByteArray m_initialState;

public:
	friend class Post2dBirdEyeWindowProjectDataItem;
	friend class Post2dBirdEyeWindowActionManager;
	friend class Post2dBirdEyeWindowDataModel;
	friend class Post2dBirdEyeWindowEditBackgroundColorCommand;
};

#endif // POST2DBIRDEYEWINDOW_H
