/****************************************************************************
**
** Copyright (C) 2014 Canonical Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotilefetchergoogle.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtLocation/private/qgeotilespec_p.h>
#include "qgeotiledmapreplygoogle.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

QGeoTileFetcherGoogle::QGeoTileFetcherGoogle(const QString &szHost, const QString &szLocale, QObject *parent)
    : QGeoTileFetcher(parent),
      m_networkManager(new QNetworkAccessManager(this)),
    m_format("jpeg"),
    m_replyFormat("jpeg")
{
    //http://www.google.cn/maps/vt?lyrs=s&gl=cn&x=53027&y=27508&z=16 
    m_szUrl = "http://" + szHost + "/maps/vt?";
    if(!szLocale.isEmpty())
        m_szUrl += "gl=" + szLocale + "&";
}

QGeoTileFetcherGoogle::~QGeoTileFetcherGoogle()
{
    if(m_networkManager)
        delete m_networkManager;
}

void QGeoTileFetcherGoogle::setFormat(const QString &format)
{
    m_format = format;

    if (m_format == "png" || m_format == "png32" || m_format == "png64" || m_format == "png128" || m_format == "png256")
        m_replyFormat = "png";
    else if (m_format == "jpg70" || m_format == "jpg80" || m_format == "jpg90")
        m_replyFormat = "jpg";
    else if("jpeg" == m_format)
        m_replyFormat = "jpeg";
    else
        qWarning() << "Unknown map format " << m_format;
}

QGeoTiledMapReply *QGeoTileFetcherGoogle::getTileImage(const QGeoTileSpec &spec)
{
    QNetworkRequest request;
    QString szUrl = m_szUrl;
    
    switch(spec.mapId())
    {
    case 1:
        szUrl += "lyrs=y";
        setFormat("jpeg");
        break;
        ;
    case 2:
        szUrl += "lyrs=s";
        setFormat("jpeg");
        break;
        ;
    case 3:
        szUrl += "lyrs=m";
        setFormat("png");
        break;
        ;
    default:
        qWarning("Unknown map id %d\n", spec.mapId());
    }
    
    szUrl += "&x=" + QString::number(spec.x())
            + "&y=" + QString::number(spec.y())
            + "&z=" + QString::number(spec.zoom());
    qDebug() << szUrl;
    request.setUrl(QUrl(szUrl));

    QNetworkReply *reply = m_networkManager->get(request);

    return new QGeoTiledMapReplyGoogle(reply, spec, m_replyFormat);
}

/*
 n = 2 ^ zoom
 xtile = ((lon_deg + 180) / 360) * n
 ytile = (1 - (log(tan(lat_rad) + sec(lat_rad)) / π)) / 2 * n
*/
int QGeoTileFetcherGoogle::getTile(const QGeoTileSpec &spec, int &x, int &y)
{
    long n = 2 ^ spec.zoom();
    x = ((spec.y() + 180) / 360) * n;
    y = (1 - (log(tan(spec.x()) + 1 / cos(spec.x()) / 3.1415926))) / 2 * n;
    return 0;
}

QT_END_NAMESPACE
