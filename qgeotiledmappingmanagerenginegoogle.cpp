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

#include "qgeotiledmappingmanagerenginegoogle.h"
#include "qgeotilefetchergoogle.h"
#include "qgeotiledmapgoogle.h"
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotilecache_p.h>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineGoogle::QGeoTiledMappingManagerEngineGoogle(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString)
    : QGeoTiledMappingManagerEngine()
{
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(19.0);
    setCameraCapabilities(cameraCaps);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> mapTypes;
    mapTypes << QGeoMapType(QGeoMapType::SatelliteMapDay, tr("Satellite map day"), tr("Google satellite map day"), true, false, 2)
             << QGeoMapType(QGeoMapType::HybridMap, tr("Hybrid map"), tr("Google hybrid map"), true, false, 1)
             << QGeoMapType(QGeoMapType::StreetMap, tr("Street map"), tr("Google street map"), true, false, 3)
                ;
    setSupportedMapTypes(mapTypes);

    QString szHost = "www.google.com";
    if (parameters.contains(QStringLiteral("google.mapping.host"))) {
        szHost = parameters.value(QStringLiteral("google.mapping.host")).toString();
    }

    if (parameters.contains(QStringLiteral("google.mapping.locale"))) {
        m_szLocale = parameters.value(QStringLiteral("google.mapping.locale")).toString();
    }
    if("cn" == m_szLocale && "www.google.com" == szHost)
    {
        szHost = "www.google.cn";
    }
    QGeoTileFetcherGoogle *tileFetcher = new QGeoTileFetcherGoogle(szHost, m_szLocale, this);
    setTileFetcher(tileFetcher);
    
    QString cacheDir;   
    if (parameters.contains(QStringLiteral("google.mapping.cache.directory"))) {
        cacheDir = parameters.value(QStringLiteral("google.mapping.cache.directory")).toString();
    }
    else
        cacheDir =  QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)
                + QLatin1String("/QtLocation/google");
    
    QGeoTileCache *tileCache = createTileCacheWithDir(cacheDir);

    if (parameters.contains(QStringLiteral("google.mapping.cache.disk.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QStringLiteral("here.mapping.cache.disk.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setMaxDiskUsage(cacheSize);
    }

    if (parameters.contains(QStringLiteral("google.mapping.cache.memory.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QStringLiteral("google.mapping.cache.memory.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setMaxMemoryUsage(cacheSize);
    }

    if (parameters.contains(QStringLiteral("google.mapping.cache.texture.size"))) {
      bool ok = false;
      int cacheSize = parameters.value(QStringLiteral("google.mapping.cache.texture.size")).toString().toInt(&ok);
      if (ok)
          tileCache->setExtraTextureUsage(cacheSize);
    }

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QGeoTiledMappingManagerEngineGoogle::~QGeoTiledMappingManagerEngineGoogle()
{
}

QGeoMap *QGeoTiledMappingManagerEngineGoogle::createMap()
{
    return new QGeoTiledMapGoogle(m_szLocale, this, 0);
}

QT_END_NAMESPACE
