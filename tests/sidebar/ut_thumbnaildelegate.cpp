/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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

#include "ThumbnailDelegate.h"
#include "DocSheet.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>
#include <QPainter>

class UT_ThumbnailDelegate : public ::testing::Test
{
public:
    UT_ThumbnailDelegate(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pView = new SideBarImageListView(sheet);
        m_tester = new ThumbnailDelegate(m_pView);
        m_pView->setItemDelegate(m_tester);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_pView;
    }

protected:
    ThumbnailDelegate *m_tester;
    SideBarImageListView *m_pView;
};

TEST_F(UT_ThumbnailDelegate, initTest)
{

}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_paint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QPainter *painter = new QPainter();
    QStyleOptionViewItem option;
    m_tester->paint(painter, option, m_pView->getImageModel()->index(0, 0));
    EXPECT_TRUE(m_tester->m_parent == m_pView);
    delete painter;
}

TEST_F(UT_ThumbnailDelegate, UT_ThumbnailDelegate_drawBookMark)
{
    QPainter *painter = new QPainter();
    m_tester->drawBookMark(painter, m_pView->geometry(), true);
    EXPECT_TRUE(m_pView != nullptr);
    delete painter;
}
