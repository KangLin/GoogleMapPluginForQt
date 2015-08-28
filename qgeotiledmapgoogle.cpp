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

#include "qgeotiledmapgoogle.h"
#include "qcoordtrans.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

QGeoTiledMapGoogle::QGeoTiledMapGoogle(const QString &szLocale, 
         QGeoTiledMappingManagerEngineGoogle *engine, QObject *parent)
    : QGeoTiledMap(engine, parent),
      m_szLocale(szLocale)
{    
}

QGeoTiledMapGoogle::~QGeoTiledMapGoogle()
{
}

QGeoCoordinate QGeoTiledMapGoogle::itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport) const
{
    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }
    
    QGeoCoordinate coor = QGeoTiledMap::itemPositionToCoordinate(pos, clipToViewport);
    //GCJ-02 to WGS84 
    if("cn" == m_szLocale)
    {
        //qDebug() << "transformFromGCJToWGS:";
        //qDebug() << "lat:" << coor.latitude() << ";lng:" << coor.longitude();
        coor = QCoordTrans::GCJToWGS(coor);
        //qDebug() << "lat:" << coor.latitude() << ";lng:" << coor.longitude();
    }
    return coor;
}

QDoubleVector2D QGeoTiledMapGoogle::coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    QGeoCoordinate coor = coordinate;
    //WGS84 to GCJ-02  
    if("cn" == m_szLocale)
    {
        //qDebug() << "transformFromWGSToGCJ:";
        //qDebug() << "lat:" << coor.latitude() << ";lng:" << coor.longitude();
        coor = QCoordTrans::WGSToGCJ(coor);
        //qDebug() << "lat:" << coor.latitude() << ";lng:" << coor.longitude();
    }
    QDoubleVector2D pos = QGeoTiledMap::coordinateToItemPosition(coor, clipToViewport);

    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QDoubleVector2D(qQNaN(), qQNaN());
    }

    return pos;
}

QT_END_NAMESPACE
