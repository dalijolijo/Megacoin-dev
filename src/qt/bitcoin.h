// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MEGACOIN_QT_BITCOIN_H
#define MEGACOIN_QT_BITCOIN_H

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <QApplication>
#include <memory>
#include <vector>

class BitcoinGUI;
class ClientModel;
class NetworkStyle;
class OptionsModel;
class PaymentServer;
class PlatformStyle;
class WalletModel;

namespace interfaces {
class Handler;
class Node;
} // namespace interfaces

/** Class encapsulating Bitcoin Core startup and shutdown.
 * Allows running startup and shutdown in a different thread from the UI thread.
 */
class BitcoinCore: public QObject
{
    Q_OBJECT
public:
    explicit BitcoinCore(interfaces::Node& node);

public Q_SLOTS:
    void initialize();
    void shutdown();

Q_SIGNALS:
    void initializeResult(bool success);
    void shutdownResult();
    void runawayException(const QString &message);

private:
    /// Pass fatal exception message to UI thread
    void handleRunawayException(const std::exception *e);

    interfaces::Node& m_node;
};

/** Main Bitcoin application object */
class BitcoinApplication: public QApplication
{
    Q_OBJECT
public:
    explicit BitcoinApplication(interfaces::Node& node, int &argc, char **argv);
    ~BitcoinApplication();

#ifdef ENABLE_WALLET
    /// Create payment server
    void createPaymentServer();
#endif
    /// parameter interaction/setup based on rules
    void parameterSetup();
    /// Create options model
    void createOptionsModel(bool resetSettings);
    /// Create main window
    void createWindow(const NetworkStyle *networkStyle);
    /// Create splash screen
    void createSplashScreen(const NetworkStyle *networkStyle);

    /// Request core initialization
    void requestInitialize();
    /// Request core shutdown
    void requestShutdown();

    /// Get process return value
    int getReturnValue() const { return returnValue; }

    /// Get window identifier of QMainWindow (BitcoinGUI)
    WId getMainWinId() const;

    /// Setup platform style
    void setupPlatformStyle();

public Q_SLOTS:
    void initializeResult(bool success);
    void shutdownResult();
    /// Handle runaway exceptions. Shows a message box with the problem and quits the program.
    void handleRunawayException(const QString &message);
    void addWallet(WalletModel* walletModel);
    void removeWallet();

Q_SIGNALS:
    void requestedInitialize();
    void requestedShutdown();
    void stopThread();
    void splashFinished(QWidget *window);

private:
    QThread *coreThread;
    interfaces::Node& m_node;
    OptionsModel *optionsModel;
    ClientModel *clientModel;
    BitcoinGUI *window;
    QTimer *pollShutdownTimer;
#ifdef ENABLE_WALLET
    PaymentServer* paymentServer;
    std::vector<WalletModel*> m_wallet_models;
    std::unique_ptr<interfaces::Handler> m_handler_load_wallet;
#endif
    int returnValue;
    const PlatformStyle *platformStyle;
    std::unique_ptr<QWidget> shutdownWindow;

    void startThread();
};

#endif // MEGACOIN_QT_BITCOIN_H 