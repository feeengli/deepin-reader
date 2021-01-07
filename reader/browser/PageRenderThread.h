/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
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
#ifndef PAGERENDERTHREAD_H
#define PAGERENDERTHREAD_H

#include "Global.h"
#include "Model.h"

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>

class DocSheet;
class BrowserPage;
class SideBarImageViewModel;

struct DocPageNormalImageTask {//正常取图
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect rect = QRect();       //整个大小
};

struct DocPageSliceImageTask {//取切片
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect whole = QRect();      //整个大小
    QRect slice = QRect();      //切片大小
};

struct DocPageBigImageTask {//取大图
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;           //任务艾迪
    QRect rect = QRect();       //整个大小
};

struct DocPageWordTask {//取页码文字
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
};

struct DocPageThumbnailTask {//缩略图
    DocSheet *sheet = nullptr;
    SideBarImageViewModel *model = nullptr;
    int index = -1;
};

struct DocOpenTask {
    DocSheet *sheet = nullptr;
};

/**
 * @brief The PageRenderThread class
 * 执行加载图片和文字等耗时操作的线程,由于pdfium非常线程不安全，所有操作都在本线程中进行
 */
class PageRenderThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief clearImageTasks
     * 清除需要读取图片的任务
     * @param sheet
     * @param item 项指针
     * @param pixmapId 删除不同的pixmapId,-1为删除所有
     * @return 是否成功
     */
    static bool clearImageTasks(DocSheet *sheet, BrowserPage *page, int pixmapId = -1);

    /**
     * @brief appendTask
     * 添加任务到队列
     * @param task
     */
    static void appendTask(DocPageNormalImageTask task);

    static void appendTask(DocPageSliceImageTask task);

    static void appendTask(DocPageBigImageTask task);

    static void appendTask(DocPageWordTask task);

    static void appendTask(DocPageThumbnailTask task);

    static void appendTask(DocOpenTask task);

    /**
     * @brief destroyForever
     * 销毁线程且不会再被创建
     */
    static void destroyForever();

private:
    explicit PageRenderThread(QObject *parent = nullptr);

    ~PageRenderThread();

    void run();

private:
    bool hasNextTask();

    bool popNextDocPageNormalImageTask(DocPageNormalImageTask &task);

    bool popNextDocPageSliceImageTask(DocPageSliceImageTask &task);

    bool popNextDocPageBigImageTask(DocPageBigImageTask &task);

    bool popNextDocPageWordTask(DocPageWordTask &task);

    bool popNextDocPageThumbnailTask(DocPageThumbnailTask &task);

    bool popNextDocOpenTask(DocOpenTask &task);

private:
    bool execNextDocPageNormalImageTask();

    bool execNextDocPageSliceImageTask();

    bool execNextDocPageWordTask();

    bool execNextDocPageThumbnailTask();

    bool execNextDocOpenTask();

signals:
    void sigDocPageNormalImageTaskFinished(DocPageNormalImageTask, QPixmap);

    void sigDocPageSliceImageTaskFinished(DocPageSliceImageTask, QPixmap);

    void sigDocPageBigImageTaskFinished(DocPageBigImageTask, QPixmap);

    void sigDocPageWordTaskFinished(DocPageWordTask, QList<deepin_reader::Word>);

    void sigDocPageThumbnailTaskFinished(DocPageThumbnailTask, QPixmap);

    void sigDocOpenTask(DocOpenTask, bool);

private slots:
    void onDocPageNormalImageTaskFinished(DocPageNormalImageTask task, QPixmap pixmap);

    void onDocPageSliceImageTaskFinished(DocPageSliceImageTask task, QPixmap pixmap);

    void onDocPageBigImageTaskFinished(DocPageBigImageTask task, QPixmap pixmap);

    void onDocPageWordTaskFinished(DocPageWordTask task, QList<deepin_reader::Word> words);

    void onDocPageThumbnailTask(DocPageThumbnailTask task, QPixmap pixmap);

    void onDocOpenTask(DocOpenTask task, bool result);

private:
    QMutex m_pageNormalImageMutex;
    QList<DocPageNormalImageTask> m_pageNormalImageTasks;

    QMutex m_pageSliceImageMutex;
    QList<DocPageSliceImageTask> m_pageSliceImageTasks;

    QMutex m_pageBigImageMutex;
    QList<DocPageBigImageTask> m_pageBigImageTasks;

    QMutex m_pageWordMutex;
    QList<DocPageWordTask> m_pageWordTasks;

    QMutex m_pageThumbnailMutex;
    QList<DocPageThumbnailTask> m_pageThumbnailTasks;

    QMutex m_openMutex;
    QList<DocOpenTask> m_openTasks;

    bool m_quit = false;

    static bool s_quitForever;

    static PageRenderThread *s_instance;

    static PageRenderThread *instance();
};

#endif // PAGERENDERTHREAD_H
