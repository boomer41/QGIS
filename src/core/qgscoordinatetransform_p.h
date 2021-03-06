/***************************************************************************
               qgscoordinatetransform_p.h
               --------------------------
    begin                : July 2016
    copyright            : (C) 2016 Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCOORDINATETRANSFORMPRIVATE_H
#define QGSCOORDINATETRANSFORMPRIVATE_H

#define SIP_NO_FILE
#include "qgsconfig.h"

/// @cond PRIVATE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QGIS API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//

#include <QSharedData>

#if PROJ_VERSION_MAJOR<6
typedef void *projPJ;
#ifndef USE_THREAD_LOCAL
#include <QThreadStorage>
#endif
typedef QPair< projPJ, projPJ > ProjData;
#else
struct PJconsts;
typedef struct PJconsts PJ;
typedef PJ *ProjData;
#endif

#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransformcontext.h"

#if PROJ_VERSION_MAJOR<6

typedef void *projCtx;

/**
 * \class QgsProjContextStore
 * \ingroup core
 * Used to create and store a proj projCtx object, correctly freeing the context upon destruction.
 */
class QgsProjContextStore
{
  public:

    QgsProjContextStore();
    ~QgsProjContextStore();

    projCtx get() { return context; }

  private:
    projCtx context;
};

#endif

class QgsCoordinateTransformPrivate : public QSharedData
{

  public:

    explicit QgsCoordinateTransformPrivate();

    QgsCoordinateTransformPrivate( const QgsCoordinateReferenceSystem &source,
                                   const QgsCoordinateReferenceSystem &destination,
                                   const QgsCoordinateTransformContext &context );

    QgsCoordinateTransformPrivate( const QgsCoordinateReferenceSystem &source,
                                   const QgsCoordinateReferenceSystem &destination,
                                   int sourceDatumTransform,
                                   int destDatumTransform );

    QgsCoordinateTransformPrivate( const QgsCoordinateTransformPrivate &other );

    ~QgsCoordinateTransformPrivate();

    bool checkValidity();

    void invalidate();

    bool initialize();

    void calculateTransforms( const QgsCoordinateTransformContext &context );

    ProjData threadLocalProjData();

    /**
     * Flag to indicate whether the transform is valid (ie has a valid
     * source and destination crs)
     */
    bool mIsValid = false;

    /**
     * Flag to indicate that the source and destination coordinate systems are
     * equal and not transformation needs to be done
     */
    bool mShortCircuit = false;

    //! QgsCoordinateReferenceSystem of the source (layer) coordinate system
    QgsCoordinateReferenceSystem mSourceCRS;

    //! QgsCoordinateReferenceSystem of the destination (map canvas) coordinate system
    QgsCoordinateReferenceSystem mDestCRS;

    QString mSourceProjString;
    QString mDestProjString;

    int mSourceDatumTransform = -1;
    int mDestinationDatumTransform = -1;

    bool mSourceAxisOrderSwapped = false;
    bool mDestAxisOrderSwapped = false;

#if PROJ_VERSION_MAJOR<6

    /**
     * Thread local proj context storage. A new proj context will be created
     * for every thread.
     */
#ifdef USE_THREAD_LOCAL
    static thread_local QgsProjContextStore mProjContext;
#else
    static QThreadStorage< QgsProjContextStore * > mProjContext;
#endif
#endif


    QReadWriteLock mProjLock;
    QMap < uintptr_t, ProjData > mProjProjections;

  private:

    //! Removes +nadgrids and +towgs84 from proj4 string
    QString stripDatumTransform( const QString &proj4 ) const;

    //! In certain situations, null grid shifts have to be added to src / dst proj string
    void addNullGridShifts( QString &srcProjString, QString &destProjString, int sourceDatumTransform, int destinationDatumTransform ) const;

    void setFinder();

    void freeProj();
};

/// @endcond

#endif // QGSCOORDINATETRANSFORMPRIVATE_H
