#include "stdafx.h"
#include "exchange_history_widget.h"
#include "gm_exchange_stats.h"
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <limits>
#include <span>
#include <utility>

namespace
{
constexpr int volume_height=115, bottom_margin=20, top_margin=10;
int interval(int width)
{
    constexpr std::array thresholds{std::pair{3,1},std::pair{6,2},std::pair{12,3},std::pair{18,6},
        std::pair{24,8},std::pair{30,10},std::pair{36,14},std::pair{42,18}};
    int result=1;
    for (const auto [threshold,gap] : thresholds)
        if (width>=threshold) result=gap;
    return result;
}
struct Plot
{
    QRectF price,volume;
    int stride;
    std::size_t count;
};
std::optional<Plot> plot(const QWidget& widget,std::size_t records,int candle_width)
{
    const auto font=widget.fontMetrics();
    const int axis=std::max({80,font.horizontalAdvance(QString::fromStdString(gm::exchange_price(INT32_MIN)))+12,
        font.horizontalAdvance(QString::number(INT32_MIN))+12});
    const int right=widget.width()-axis;
    const int price_bottom=widget.height()-volume_height-10;
    const int stride=candle_width+interval(candle_width);
    if (!records || right<stride+2 || price_bottom-top_margin<100) return {};
    const auto count=std::min(records,static_cast<std::size_t>((right-2)/stride));
    if (!count) return {};
    return Plot{QRectF(0,top_margin,right,price_bottom-top_margin),
        QRectF(0,widget.height()-volume_height+5,right,volume_height-bottom_margin-5),stride,count};
}
}

CExchangeHistoryUI::CExchangeHistoryUI(QWidget* parent,Qt::WindowFlags flags) : QLabel(parent,flags)
{
    setMouseTracking(true);
}
CExchangeHistoryUI::~CExchangeHistoryUI() = default;
BOOL CExchangeHistoryUI::UnInit()
{
    m_Data.clear();m_mouse.reset();update();return TRUE;
}
BOOL CExchangeHistoryUI::SetData(INT32 count,const char* bytes)
{
    if (count<0 || count>MAX_EXCHANGE_HISTORY_NUM || (count && !bytes)) return FALSE;
    std::vector<GM_EXCHANGE_PRICE_HISTORY> candidate(count);
    if (count) std::memcpy(candidate.data(),bytes,candidate.size()*sizeof(candidate.front()));
    for (const auto& row : candidate)
        if (!std::memchr(row.szHistoryTime,0,sizeof(row.szHistoryTime))) return FALSE;
    m_Data.swap(candidate);m_mouse.reset();update();return TRUE;
}
void CExchangeHistoryUI::wheelEvent(QWheelEvent* event)
{
    const auto delta=event->angleDelta().y() ? event->angleDelta().y() : event->pixelDelta().y();
    if (!delta) { QLabel::wheelEvent(event);return; }
    m_candle_width=std::clamp(m_candle_width+(delta>0 ? 1 : -1),1,48);
    update();event->accept();
}
void CExchangeHistoryUI::mouseMoveEvent(QMouseEvent* event)
{
    m_mouse=rect().contains(event->pos()) ? std::optional(event->pos()) : std::nullopt;
    update();event->accept();
}
void CExchangeHistoryUI::leaveEvent(QEvent* event)
{
    m_mouse.reset();update();QLabel::leaveEvent(event);
}
void CExchangeHistoryUI::paintEvent(QPaintEvent* event)
{
    const auto frame=plot(*this,m_Data.size(),m_candle_width);
    if (!frame) { QLabel::paintEvent(event);return; }
    const auto rows=std::span<const GM_EXCHANGE_PRICE_HISTORY>(m_Data).last(frame->count);
    std::int64_t low=INT64_MAX,high=INT64_MIN,volume=1;
    for (const auto& row : rows)
    {
        const std::array<std::int64_t,4> prices{row.nLow,row.nHigh,row.nOpen,row.nClose};
        low=std::min(low,std::ranges::min(prices));high=std::max(high,std::ranges::max(prices));
        volume=std::max(volume,static_cast<std::int64_t>(row.nVolume));
    }
    const auto padding=std::max<std::int64_t>(10,(high-low)/20);
    low-=padding;high+=padding;
    const auto price_y=[&](std::int64_t value)
    {
        const auto fraction=static_cast<double>(value-low)/static_cast<double>(high-low);
        return frame->price.bottom()-std::clamp(fraction,0.0,1.0)*frame->price.height();
    };
    QPainter painter(this);
    painter.fillRect(rect(),Qt::black);
    const auto right=frame->price.right();
    const auto axis_width=width()-right-5;
    QPen grid(QColor(176,0,0));grid.setStyle(Qt::DashLine);painter.setPen(grid);
    const int lines=std::clamp(static_cast<int>(frame->price.height()/50),2,12);
    for (int line=0;line<=lines;++line)
    {
        const auto value=low+(high-low)*line/lines;
        const auto y=price_y(value);
        painter.drawLine(QLineF(0,y,right,y));
        painter.drawText(QRectF(right+5,y-fontMetrics().height()/2.0,axis_width,fontMetrics().height()),
            Qt::AlignLeft,QString::fromStdString(gm::exchange_price(value)));
    }
    for (int line=0;line<=4;++line)
    {
        const auto y=frame->volume.bottom()-frame->volume.height()*line/4.0;
        painter.drawLine(QLineF(0,y,right,y));
        painter.drawText(QRectF(right+5,y-fontMetrics().height()/2.0,axis_width,fontMetrics().height()),
            Qt::AlignLeft,QString::number(static_cast<qlonglong>(volume*line/4)));
    }
    painter.setPen(QColor(176,0,0));
    painter.drawLine(QLineF(right,0,right,height()));
    painter.drawLine(QLineF(0,frame->volume.top(),width(),frame->volume.top()));
    painter.drawLine(QLineF(0,frame->volume.bottom(),width(),frame->volume.bottom()));
    for (std::size_t index=0;index<rows.size();++index)
    {
        const auto& row=rows[index];
        const auto x=1.0+static_cast<double>(index)*frame->stride;
        const QColor colour=row.nClose>row.nOpen ? QColor(255,50,50) :
            row.nClose<row.nOpen ? QColor(84,255,255) : QColor(Qt::white);
        painter.setPen(colour);
        painter.drawLine(QLineF(x+m_candle_width/2.0,price_y(row.nHigh),x+m_candle_width/2.0,price_y(row.nLow)));
        const auto open=price_y(row.nOpen),close=price_y(row.nClose);
        if (row.nOpen==row.nClose) painter.drawLine(QLineF(x,open,x+m_candle_width,open));
        else
        {
            const QRectF body(x,std::min(open,close),m_candle_width,std::abs(open-close));
            painter.fillRect(body,row.nClose<row.nOpen ? colour : QColor(Qt::black));painter.drawRect(body);
        }
        const auto bar_height=frame->volume.height()*static_cast<double>(std::max<std::int64_t>(0,row.nVolume))/volume;
        const QRectF bar(x,frame->volume.bottom()-bar_height,m_candle_width,bar_height);
        painter.fillRect(bar,row.nClose<row.nOpen ? colour : QColor(Qt::black));painter.drawRect(bar);
    }
    if (m_mouse && m_mouse->x()>=1 && m_mouse->x()<right)
    {
        const auto index=static_cast<std::size_t>((m_mouse->x()-1)/frame->stride);
        if (index<rows.size())
        {
            painter.setPen(Qt::white);
            painter.drawLine(QLineF(m_mouse->x(),0,m_mouse->x(),height()));
            painter.drawLine(QLineF(0,m_mouse->y(),width(),m_mouse->y()));
            const auto time=QString::fromUtf8(rows[index].szHistoryTime);
            const auto text_x=std::clamp(m_mouse->x()+8,0,std::max(0,static_cast<int>(right)-fontMetrics().horizontalAdvance(time)));
            painter.drawText(QPointF(text_x,height()-4),time);
            if (m_mouse->y()>=frame->volume.top())
                painter.drawText(QPointF(right+5,std::clamp(m_mouse->y(),0,height()-4)),QString::number(rows[index].nVolume));
        }
    }
}
