/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#pragma once

#include <vcl/dllapi.h>
#include <tools/color.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/point/b2dpoint.hxx>

namespace vcl::pdf
{
/** PDF Annotation marker
 *
 * All units in 100th mm.
 */
struct VCL_DLLPUBLIC PDFAnnotationMarker
{
    float mnWidth = 0.0;
    Color maFillColor = COL_TRANSPARENT;
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerCircle : public PDFAnnotationMarker
{
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerSquare : public PDFAnnotationMarker
{
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerInk : public PDFAnnotationMarker
{
    std::vector<basegfx::B2DPolygon> maStrokes;
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerPolygon : public PDFAnnotationMarker
{
    basegfx::B2DPolygon maPolygon;
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerLine : public PDFAnnotationMarker
{
    basegfx::B2DPoint maLineStart;
    basegfx::B2DPoint maLineEnd;
};

enum class PDFTextMarkerType
{
    Highlight,
    Underline,
    Squiggly,
    StrikeOut
};

struct VCL_DLLPUBLIC PDFAnnotationMarkerHighlight : public PDFAnnotationMarker
{
    std::vector<basegfx::B2DPolygon> maQuads;
    PDFTextMarkerType meTextMarkerType;

    PDFAnnotationMarkerHighlight(PDFTextMarkerType eTextMarkerType)
        : meTextMarkerType(eTextMarkerType)
    {
    }
};

} // namespace vcl::pdf

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
