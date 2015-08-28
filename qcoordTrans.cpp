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

#include "qcoordtrans.h"

QT_BEGIN_NAMESPACE

namespace TRANSCOORD {

struct Location
{
    double lat;
    double lng;
};

const double a = 6378245.0;
const double ee = 0.00669342162296594323;
const double pi = 3.14159265358979324;

int outOfChina(double lat, double lng)
{
    if (lng < 72.004 || lng > 137.8347)
        return 1;
    
    if (lat < 0.8293 || lat > 55.8271)
        return 1;
    
    return 0;
}

double transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(x > 0 ? x:-x);
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 *sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    
    return ret;
}

double transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(x > 0 ? x:-x);
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    
    return ret;
}

Location LocationMake(double lat, double lng)
{
    Location loc;
    loc.lat = lat, loc.lng = lng;
    
    return loc;
}

/**
 *  标准坐标-》中国坐标
 */
Location transformFromWGSToGCJ(Location wgLoc)
{
    Location mgLoc;
    if (outOfChina(wgLoc.lat, wgLoc.lng))
    {
        mgLoc = wgLoc;
        return mgLoc;
    }
    double dLat = transformLat(wgLoc.lng - 105.0, wgLoc.lat - 35.0);
    double dLon = transformLon(wgLoc.lng - 105.0, wgLoc.lat - 35.0);
    double radLat = wgLoc.lat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    mgLoc.lat = wgLoc.lat + dLat;
    mgLoc.lng = wgLoc.lng + dLon;
    
    return mgLoc;
}

/**
 *  中国坐标-》标准坐标
 */
Location transformFromGCJToWGS(Location gcLoc)
{
    Location wgLoc = gcLoc;
    Location currGcLoc, dLoc;
    while (1) {
        currGcLoc = transformFromWGSToGCJ(wgLoc);
        dLoc.lat = gcLoc.lat - currGcLoc.lat;
        dLoc.lng = gcLoc.lng - currGcLoc.lng;
        if (fabs(dLoc.lat) < 1e-7 && fabs(dLoc.lng) < 1e-7) {  // 1e-7 ~ centimeter level accuracy
            // Result of experiment:
            //   Most of the time 2 iterations would be enough for an 1e-8 accuracy (milimeter level).
            //
            return wgLoc;
        }
        wgLoc.lat += dLoc.lat;
        wgLoc.lng += dLoc.lng;
    }
    
    return wgLoc;
}

///
///  Transform GCJ-02 to BD-09
///
Location bd_encrypt(Location gcLoc)
{
    return LocationMake(gcLoc.lat + 0.006,gcLoc.lng + 0.0065);
}

///
///  Transform BD-09 to GCJ-02
///
Location bd_decrypt(Location bdLoc)
{
    return LocationMake(bdLoc.lat - 0.006,bdLoc.lng - 0.0065);
}

} //namespace TRANSCOORD 

QGeoCoordinate QCoordTrans::GCJToWGS(const QGeoCoordinate &pos)
{
    if(!pos.isValid())
        return QGeoCoordinate();

    TRANSCOORD::Location gcjLoc, wgsLoc;
    gcjLoc.lat = pos.latitude();
    gcjLoc.lng = pos.longitude();
    wgsLoc = TRANSCOORD::transformFromGCJToWGS(gcjLoc);
    return QGeoCoordinate(wgsLoc.lat, wgsLoc.lng);
}

QGeoCoordinate QCoordTrans::WGSToGCJ(const QGeoCoordinate &pos)
{
    if(!pos.isValid())
        return QGeoCoordinate();

    TRANSCOORD::Location gcjLoc, wgsLoc;
    wgsLoc.lat = pos.latitude();
    wgsLoc.lng = pos.longitude();
    gcjLoc = TRANSCOORD::transformFromWGSToGCJ(wgsLoc);
    return QGeoCoordinate(gcjLoc.lat, gcjLoc.lng);
}

QT_END_NAMESPACE
