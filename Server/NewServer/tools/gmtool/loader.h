/********************************************************************
	created:	4:7:2013   18:35
	file:		loader.h
	author:		Allan Tao
	
	purpose:	Login UI class
*********************************************************************/
#ifndef LOADER_H
#define LOADER_H

#include "gm_application.h"
#include <QtWidgets/QMainWindow>
#include "ui_loader.h"
#include <optional>
#include <memory>
#include <cstdint>
#include <string_view>

class ISocketSession;
class QTimer;

class loader : public QDialog
{
    Q_OBJECT

public:
    loader(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~loader();

    BOOL VerifyUser(INT32 result, BOOL exchange_mode, ISocketSession* socket);
    void ServerError(ISocketSession* socket, std::string_view text);

public slots:
    void Login();
    void Exit();
    void done(int result) override;


private:
    struct Attempt
    {
        std::uint64_t generation, serial;
        UINT32 session_id;
        ISocketSession* socket;
        std::shared_ptr<const char> connection_lifetime;
    };
    std::optional<Attempt> m_pending;
    std::uint64_t m_serial = 0;
    bool m_verified = false, m_submitting = false;
    QTimer *m_timeout, *m_watch;
    bool Current(const Attempt& attempt) const;
    void ClearPending();
    void Status(const QString& text);
    void CancelPending(const QString& text, bool close_peer);
    Ui::loginDialog ui;
};



#endif // LOADER_H
