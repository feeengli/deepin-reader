/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include "../CustomItemWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */


class BookMarkItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkItemWidget)

public:
    explicit BookMarkItemWidget(DWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

    void setBSelect(const bool &paint);

protected:
    void initWidget() override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_bPaint = false;                    // 是否绘制选中item
};

#endif // BOOKMARKITEMWIDGET_H
