﻿/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     qpdfview
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "PDFModel.h"

#include <DeepinPdf>
#include <QDebug>

#define LOCK_ANNOTATION
#define LOCK_FORM_FIELD
#define LOCK_PAGE
#define LOCK_DOCUMENT
#define LOCK_LONGTIMEOPERATION QMutexLocker mutexLocker(m_mutex);

namespace deepin_reader {

PDFAnnotation::PDFAnnotation(QMutex *mutex, DPdfAnnot *annotation) : Annotation(),
    m_mutex(mutex),
    m_annotation(annotation)
{
}

PDFAnnotation::~PDFAnnotation()
{
    if (m_annotation) {
        delete m_annotation;
        m_annotation = nullptr;
    }
}

QList<QRectF> PDFAnnotation::boundary() const
{
    LOCK_ANNOTATION

    QList<QRectF> rectFList;

    //    if (m_annotation->type() == DPdfAnnot::AText) {
    //        rectFList.append(m_annotation->boundary().normalized());
    //    } else if (m_annotation->subType() == Poppler::Annotation::AHighlight) {
    //        QList<Poppler::HighlightAnnotation::Quad> quads = static_cast<Poppler::HighlightAnnotation *>(m_annotation)->highlightQuads();
    //        foreach (Poppler::HighlightAnnotation::Quad quad, quads) {
    //            QRectF rectbound;
    //            rectbound.setTopLeft(quad.points[0]);
    //            rectbound.setTopRight(quad.points[1]);
    //            rectbound.setBottomLeft(quad.points[3]);
    //            rectbound.setBottomRight(quad.points[2]);
    //            rectFList.append(rectbound);
    //        }
    //    }

    return rectFList;
}

QString PDFAnnotation::contents() const
{
    LOCK_ANNOTATION

    if (nullptr == m_annotation)
        return QString();

    return m_annotation->text();
}

int PDFAnnotation::type()
{
    if (nullptr == m_annotation)
        return -1;

    return m_annotation->type();
}

DPdfAnnot *PDFAnnotation::ownAnnotation()
{
    return m_annotation;
}

PDFPage::PDFPage(QMutex *mutex, DPdfPage *page) :
    m_mutex(mutex),
    m_page(page)
{
    m_pageSizef = QSizeF(page->width(), page->height());
}

PDFPage::~PDFPage()
{
    LOCK_LONGTIMEOPERATION

    delete m_page;

    m_page = nullptr;
}

QSizeF PDFPage::sizeF() const
{
    LOCK_PAGE

    return m_pageSizef;
}

QImage PDFPage::render(Dr::Rotation rotation, const double scaleFactor, const QRect &boundingRect) const
{
    return render(scaleFactor, scaleFactor, rotation, boundingRect);
}

QImage PDFPage::render(int width, int height, Qt::AspectRatioMode) const
{
    qreal horizontalResolution = 1.0 * width / m_page->width();
    qreal verticalResolution = 1.0 * height / m_page->height();

    return render(horizontalResolution, verticalResolution, Dr::RotateBy0, QRect());
}

QImage PDFPage::render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation, QRect boundingRect) const
{
    LOCK_LONGTIMEOPERATION

    if (m_page == nullptr)
        return QImage();

    int x = 0;
    int y = 0;
    int w = m_page->width();
    int h = m_page->height();

    if (!boundingRect.isNull()) {
        x = boundingRect.x();
        y = boundingRect.y();
        w = boundingRect.width();
        h = boundingRect.height();
    }

    return m_page->image(horizontalResolution, verticalResolution, x, y, w, h);
}

QString PDFPage::label() const
{
    LOCK_PAGE

    return m_page->label();
}

Link PDFPage::getLinkAtPoint(const QPointF &point) const
{
    LOCK_PAGE

    Link link;
    DPdfPage::Link dlInk = m_page->getLinkAtPoint(point.x(), point.y());
    if (dlInk.isValid()) {
        link.page = dlInk.nIndex + 1;
        link.urlOrFileName = dlInk.urlpath;
        link.left = dlInk.left;
        link.top = dlInk.top;
    }

    return link;
}

QString PDFPage::text(const QRectF &rect) const
{
    LOCK_PAGE

    return m_page->text(rect).simplified();
}

QList<Word> PDFPage::words(Dr::Rotation rotation)
{
    if (rotation == m_wordRotation)
        return m_words;

    m_wordRotation = rotation;

    int charCount = m_page->countChars();
    for (int i = 0; i < charCount; i++) {
        Word word;
        word.text = m_page->text(i);
        const auto textrects = m_page->getTextRect(i);
        if (textrects.size() > 0)
            word.boundingBox = m_page->getTextRect(i).first();
        m_words.append(word);
    }
    return m_words;
}

QVector<QRectF> PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    LOCK_PAGE

    QVector<QRectF> results;

    results = m_page->search(text, matchCase, wholeWords);

    return results;
}

QList< Annotation * > PDFPage::annotations() const
{
    LOCK_PAGE

    QList< Annotation * > annotations;

    foreach (DPdfAnnot *annotation, m_page->annots()) {
        if (annotation->type() == DPdfAnnot::AText || annotation->type() == DPdfAnnot::AHighlight) {
            annotations.append(new PDFAnnotation(m_mutex, annotation));
            continue;
        }

        delete annotation;
    }

    return annotations;
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color)
{
    LOCK_PAGE
    return new PDFAnnotation(m_mutex, m_page->createHightLightAnnot(boundarys, text, color));
}

bool PDFPage::removeAnnotation(deepin_reader::Annotation *annotation)
{
    LOCK_PAGE

    deepin_reader::PDFAnnotation *PDFAnnotation = static_cast< deepin_reader::PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr)
        return false;

    m_page->removeAnnot(PDFAnnotation->m_annotation);

    PDFAnnotation->m_annotation = nullptr;
    delete PDFAnnotation;
    PDFAnnotation = nullptr;

    return true;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    if (nullptr == annotation)
        return false;

    if (m_page->annots().contains(annotation->ownAnnotation())) {
        if (annotation->type() == DPdfAnnot::AText)
            m_page->updateTextAnnot(annotation->ownAnnotation(), text);
        else
            m_page->updateHightLightAnnot(annotation->ownAnnotation(), color, text);
        return true;
    }

    return false;
}

bool PDFPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    foreach (DPdfAnnot *annot, m_page->annots()) {
        if (annot && annot->pointIn(clickPoint)) {
            return true;
        }
    }

    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF rect, const QString text)
{
    if (nullptr == m_page)
        return nullptr;

    LOCK_PAGE

    return new PDFAnnotation(m_mutex, m_page->createTextAnnot(rect.center().toPoint(), text));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF rect)
{
    if (nullptr == m_page && nullptr == annot)
        return nullptr;

    LOCK_PAGE


    if (annot->ownAnnotation()) {
        m_page->updateTextAnnot(annot->ownAnnotation(), annot->ownAnnotation()->text(), rect.center());
        return annot;
    }
    return nullptr;
}

PDFDocument::PDFDocument(DPdfDoc *document) :
    m_mutex(),
    m_document(document)
{
}

PDFDocument::~PDFDocument()
{
    delete m_document;
    m_document = nullptr;
}

int PDFDocument::numberOfPages() const
{
    LOCK_DOCUMENT

    return m_document->pageCount();
}

Page *PDFDocument::page(int index) const
{
    LOCK_DOCUMENT

    if (DPdfPage *page = m_document->page(index)) {
        return new PDFPage(&m_mutex, page);
    }

    return nullptr;
}

QStringList PDFDocument::saveFilter() const
{
    return QStringList() << "Portable document format (*.pdf)";
}

bool PDFDocument::save(const QString &filePath, bool withChanges) const
{
    LOCK_DOCUMENT

    return false;
}

void collectOuleLine(const DPdfDoc::Outline &cOutline, deepin_reader::Outline &outline)
{
    for (const DPdfDoc::Section &cSection : cOutline) {
        deepin_reader::Section setction;
        setction.nIndex = cSection.nIndex;
        setction.title = cSection.title;
        setction.offsetPointF = cSection.offsetPointF;
        if (cSection.children.size() > 0) {
            collectOuleLine(cSection.children, setction.children);
        }
        outline << setction;
    }
}

Outline PDFDocument::outline() const
{
    Outline outline;

    LOCK_DOCUMENT

    const DPdfDoc::Outline &cOutline = m_document->outline();
    collectOuleLine(cOutline, outline);
    return outline;
}

Properties PDFDocument::properties() const
{
    LOCK_DOCUMENT

    Properties properties = m_document->proeries();

    return properties;
}

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password, int &status)
{
    status = DPdfDoc::tryLoadFile(filePath, password);
    if (status == DPdfDoc::SUCCESS) {
        DPdfDoc *document = new DPdfDoc(filePath, password);
        return new deepin_reader::PDFDocument(document);
    }
    return nullptr;
}

}

