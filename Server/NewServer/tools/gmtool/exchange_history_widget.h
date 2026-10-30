

#ifndef _H_UI_EXCHANGE_HISTORY_H_
#define _H_UI_EXCHANGE_HISTORY_H_

#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "../qt_cpp23.h"
#include <QtWidgets/QLabel>
#include "gm_def.h"
#include <vector>
#include <optional>

class CExchangeHistoryUI : public QLabel
{
public:
    CExchangeHistoryUI(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CExchangeHistoryUI();

    BOOL    UnInit();
    BOOL    SetData(INT32 nCount, const char *pcBuffer);

    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int m_candle_width = 6;
    std::optional<QPoint> m_mouse;
    std::vector<GM_EXCHANGE_PRICE_HISTORY> m_Data;
};

#endif //_H_UI_EXCHANGE_HISTORY_H_
