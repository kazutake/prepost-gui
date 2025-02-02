######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricPost2d
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += POST2D_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricAxis2d

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../dataitem/axis2d/debug"
	} else {
		LIBS += -L"../../dataitem/axis2d/release"
	}
}
unix {
	LIBS += -L"../../dataitem/axis2d"
}
LIBS += -liricAxis2d

# iricDistancemeasure

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../dataitem/distancemeasure/debug"
	} else {
		LIBS += -L"../../dataitem/distancemeasure/release"
	}
}
unix {
	LIBS += -L"../../distancemeasure/axis2d"
}
LIBS += -liricDistancemeasure

# iricMeasureddata

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../dataitem/measureddata/debug"
	} else {
		LIBS += -L"../../dataitem/measureddata/release"
	}
}
unix {
	LIBS += -L"../../measureddata/axis2d"
}
LIBS += -liricMeasureddata

# iricCs

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../cs/debug"
	} else {
		LIBS += -L"../../cs/release"
	}
}
unix {
	LIBS += -L"../../cs"
}
LIBS += -liricCs

# iricMisc

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../misc/debug"
	} else {
		LIBS += -L"../../misc/release"
	}
}
unix {
	LIBS += -L"../../misc"
}
LIBS += -liricMisc

# iricGuibase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guibase/debug"
	} else {
		LIBS += -L"../../guibase/release"
	}
}
unix {
	LIBS += -L"../../guibase"
}
LIBS += -liricGuibase

# iricGuicore

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../guicore/debug"
	} else {
		LIBS += -L"../../guicore/release"
	}
}
unix {
	LIBS += -L"../../guicore"
}
LIBS += -liricGuicore

# iricPostbase

win32 {
	CONFIG(debug, debug|release) {
		LIBS += -L"../../postbase/debug"
	} else {
		LIBS += -L"../../postbase/release"
	}
}
unix {
	LIBS += -L"../../postbase"
}
LIBS += -liricPostbase

######################
# External libraries #
######################

# Shapelib

win32{
	LIBS += -lshapelib_i
}

#gdal

win32 {
	LIBS += -lgdal_i
}
unix {
	LIBS += -lgdal
}

#geos

LIBS += -lgeos

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkCommonMath-6.1 \
	-lvtkCommonMisc-6.1 \
	-lvtkCommonTransforms-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersGeometry-6.1 \
	-lvtkFiltersGeneral-6.1 \
	-lvtkFiltersExtraction-6.1 \
	-lvtkFiltersFlowPaths-6.1 \
	-lvtkFiltersSources-6.1 \
	-lvtkFiltersTexture-6.1 \
	-lvtkGUISupportQt-6.1 \
	-lvtkInteractionWidgets-6.1 \
	-lvtkIOImage-6.1 \
	-lvtkIOCore-6.1 \
	-lvtkIOLegacy-6.1 \
	-lvtkRenderingAnnotation-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingFreeType-6.1 \
	-lvtkRenderingLabel-6.1 \
	-lvtkRenderingLOD-6.1

# Post-Build Event
win32 {
	QMAKE_POST_LINK += copy $(TargetPath) $(SolutionDir)\\libdlls\\$(Configuration)
}

# Input
HEADERS += post2d_global.h \
           post2dgridregionselectdialog.h \
           post2dobjectbrowser.h \
           post2dobjectbrowserview.h \
           post2dpropertybrowser.h \
           post2dwindow.h \
           post2dwindowactionmanager.h \
           post2dwindowdataitem.h \
           post2dwindowdatamodel.h \
           post2dwindowgraphicsview.h \
           post2dwindowprojectdataitem.h \
           datamodel/post2dwindowarrowstructuredsettingdialog.h \
           datamodel/post2dwindowarrowunstructuredsettingdialog.h \
           datamodel/post2dwindowbackgroundimagedataitem.h \
           datamodel/post2dwindowbackgroundimagesdataitem.h \
           datamodel/post2dwindowcellflagdataitem.h \
           datamodel/post2dwindowcellflaggroupdataitem.h \
           datamodel/post2dwindowcellflagsettingdialog.h \
           datamodel/post2dwindowcellscalargroupdataitem.h \
           datamodel/post2dwindowcellscalargrouptopdataitem.h \
           datamodel/post2dwindowcontoursettingdialog.h \
           datamodel/post2dwindowgeodatadataitem.h \
           datamodel/post2dwindowgeodatagroupdataitem.h \
           datamodel/post2dwindowgeodatatopdataitem.h \
           datamodel/post2dwindowgraphdataitem.h \
           datamodel/post2dwindowgraphgroupdataitem.h \
           datamodel/post2dwindowgraphsetting.h \
           datamodel/post2dwindowgraphsettingdialog.h \
           datamodel/post2dwindowgridshapedataitem.h \
           datamodel/post2dwindowgridtypedataitem.h \
           datamodel/post2dwindowmeasureddatatopdataitem.h \
           datamodel/post2dwindownodescalargroupdataitem.h \
           datamodel/post2dwindownodescalargroupdataitem_shapeexporter.h \
           datamodel/post2dwindownodescalargrouptopdataitem.h \
           datamodel/post2dwindownodevectorarrowdataitem.h \
           datamodel/post2dwindownodevectorarrowgroupdataitem.h \
           datamodel/post2dwindownodevectorarrowgroupstructureddataitem.h \
           datamodel/post2dwindownodevectorarrowgroupunstructureddataitem.h \
           datamodel/post2dwindownodevectorarrowsetting.h \
           datamodel/post2dwindownodevectorarrowstructuredsetting.h \
           datamodel/post2dwindownodevectorarrowunstructuredsetting.h \
           datamodel/post2dwindownodevectorparticledataitem.h \
           datamodel/post2dwindownodevectorparticlegroupdataitem.h \
           datamodel/post2dwindownodevectorparticlegroupstructureddataitem.h \
           datamodel/post2dwindownodevectorparticlegroupunstructureddataitem.h \
           datamodel/post2dwindownodevectorstreamlinedataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupdataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupstructureddataitem.h \
           datamodel/post2dwindownodevectorstreamlinegroupunstructureddataitem.h \
           datamodel/post2dwindowparticlesscalardataitem.h \
           datamodel/post2dwindowparticlesscalargroupdataitem.h \
           datamodel/post2dwindowparticlestopdataitem.h \
           datamodel/post2dwindowparticlestructuredsettingdialog.h \
           datamodel/post2dwindowparticlesvectordataitem.h \
           datamodel/post2dwindowparticlesvectorgroupdataitem.h \
           datamodel/post2dwindowparticleunstructuredsettingdialog.h \
           datamodel/post2dwindowpolydatagroupdataitem.h \
           datamodel/post2dwindowpolydatatopdataitem.h \
           datamodel/post2dwindowpolydatavaluedataitem.h \
           datamodel/post2dwindowrootdataitem.h \
           datamodel/post2dwindowstreamlinestructuredsettingdialog.h \
           datamodel/post2dwindowstreamlineunstructuredsettingdialog.h \
           datamodel/post2dwindowzonedataitem.h \
           datamodel/private/post2dwindowcellflaggroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowgraphgroupdataitem_impl.h \
           datamodel/private/post2dwindowgraphgroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowgraphsettingcustomregiondialog.h \
           datamodel/private/post2dwindowgraphsettingdialog_impl.h \
           datamodel/private/post2dwindowgridshapedataitem_setsettingcommand.h \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_setsettingcommand.h \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_setsettingcommand.h \
           datamodel/private/post2dwindowparticlesscalargroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowparticlestopdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowparticlesvectorgroupdataitem_setsettingcommand.h \
           datamodel/private/post2dwindowpolydatagroupdataitem_setbasicsettingcommand.h \
           datamodel/private/post2dwindowpolydatagroupdataitem_setsettingcommand.h
FORMS += post2dgridregionselectdialog.ui \
         datamodel/post2dwindowarrowstructuredsettingdialog.ui \
         datamodel/post2dwindowarrowunstructuredsettingdialog.ui \
         datamodel/post2dwindowcellflagsettingdialog.ui \
         datamodel/post2dwindowcontoursettingdialog.ui \
         datamodel/post2dwindowgraphsettingdialog.ui \
         datamodel/post2dwindowparticlestructuredsettingdialog.ui \
         datamodel/post2dwindowparticleunstructuredsettingdialog.ui \
         datamodel/post2dwindowstreamlinestructuredsettingdialog.ui \
         datamodel/post2dwindowstreamlineunstructuredsettingdialog.ui \
         datamodel/private/post2dwindowgraphsettingcustomregiondialog.ui
SOURCES += post2dgridregionselectdialog.cpp \
           post2dobjectbrowser.cpp \
           post2dobjectbrowserview.cpp \
           post2dpropertybrowser.cpp \
           post2dwindow.cpp \
           post2dwindowactionmanager.cpp \
           post2dwindowdataitem.cpp \
           post2dwindowdatamodel.cpp \
           post2dwindowgraphicsview.cpp \
           post2dwindowprojectdataitem.cpp \
           datamodel/post2dwindowarrowstructuredsettingdialog.cpp \
           datamodel/post2dwindowarrowunstructuredsettingdialog.cpp \
           datamodel/post2dwindowbackgroundimagedataitem.cpp \
           datamodel/post2dwindowbackgroundimagesdataitem.cpp \
           datamodel/post2dwindowcellflagdataitem.cpp \
           datamodel/post2dwindowcellflaggroupdataitem.cpp \
           datamodel/post2dwindowcellflagsettingdialog.cpp \
           datamodel/post2dwindowcellscalargroupdataitem.cpp \
           datamodel/post2dwindowcellscalargrouptopdataitem.cpp \
           datamodel/post2dwindowcontoursettingdialog.cpp \
           datamodel/post2dwindowgeodatadataitem.cpp \
           datamodel/post2dwindowgeodatagroupdataitem.cpp \
           datamodel/post2dwindowgeodatatopdataitem.cpp \
           datamodel/post2dwindowgraphdataitem.cpp \
           datamodel/post2dwindowgraphgroupdataitem.cpp \
           datamodel/post2dwindowgraphsetting.cpp \
           datamodel/post2dwindowgraphsettingdialog.cpp \
           datamodel/post2dwindowgridshapedataitem.cpp \
           datamodel/post2dwindowgridtypedataitem.cpp \
           datamodel/post2dwindowmeasureddatatopdataitem.cpp \
           datamodel/post2dwindownodescalargroupdataitem.cpp \
           datamodel/post2dwindownodescalargroupdataitem_shapeexporter.cpp \
           datamodel/post2dwindownodescalargrouptopdataitem.cpp \
           datamodel/post2dwindownodevectorarrowdataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupdataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorarrowgroupunstructureddataitem.cpp \
           datamodel/post2dwindownodevectorarrowsetting.cpp \
           datamodel/post2dwindownodevectorarrowstructuredsetting.cpp \
           datamodel/post2dwindownodevectorarrowunstructuredsetting.cpp \
           datamodel/post2dwindownodevectorparticledataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupdataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorparticlegroupunstructureddataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinedataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupdataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupstructureddataitem.cpp \
           datamodel/post2dwindownodevectorstreamlinegroupunstructureddataitem.cpp \
           datamodel/post2dwindowparticlesscalardataitem.cpp \
           datamodel/post2dwindowparticlesscalargroupdataitem.cpp \
           datamodel/post2dwindowparticlestopdataitem.cpp \
           datamodel/post2dwindowparticlestructuredsettingdialog.cpp \
           datamodel/post2dwindowparticlesvectordataitem.cpp \
           datamodel/post2dwindowparticlesvectorgroupdataitem.cpp \
           datamodel/post2dwindowparticleunstructuredsettingdialog.cpp \
           datamodel/post2dwindowpolydatagroupdataitem.cpp \
           datamodel/post2dwindowpolydatatopdataitem.cpp \
           datamodel/post2dwindowpolydatavaluedataitem.cpp \
           datamodel/post2dwindowrootdataitem.cpp \
           datamodel/post2dwindowstreamlinestructuredsettingdialog.cpp \
           datamodel/post2dwindowstreamlineunstructuredsettingdialog.cpp \
           datamodel/post2dwindowzonedataitem.cpp \
           datamodel/private/post2dwindowcellflaggroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowgraphgroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowgraphsettingcustomregiondialog.cpp \
           datamodel/private/post2dwindowgridshapedataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupstructureddataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindownodevectorarrowgroupunstructureddataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowparticlesscalargroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowparticlestopdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowparticlesvectorgroupdataitem_setsettingcommand.cpp \
           datamodel/private/post2dwindowpolydatagroupdataitem_setbasicsettingcommand.cpp \
           datamodel/private/post2dwindowpolydatagroupdataitem_setsettingcommand.cpp
RESOURCES += post2d.qrc
TRANSLATIONS += languages/iricPost2d_es_ES.ts \
                languages/iricPost2d_fr_FR.ts \
                languages/iricPost2d_id_ID.ts \
                languages/iricPost2d_ja_JP.ts \
                languages/iricPost2d_ko_KR.ts \
                languages/iricPost2d_ru_RU.ts \
                languages/iricPost2d_th_TH.ts \
                languages/iricPost2d_vi_VN.ts \
                languages/iricPost2d_zh_CN.ts
