#ifndef TMSLOADER_TMSREQUESTHANDLERXYZ_H
#define TMSLOADER_TMSREQUESTHANDLERXYZ_H

#include "tmsrequesthandler.h"
#include "../tmsrequestgooglemap.h"

namespace tmsloader {

class TmsRequestHandlerXYZ : public TmsRequestHandler
{
public:
	TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QObject *parent = nullptr);
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERXYZ_H
