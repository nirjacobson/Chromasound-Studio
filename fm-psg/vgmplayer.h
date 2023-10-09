#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include <QThread>
#include <QMutex>
#include <pigpio.h>

class VGMPlayer : public QThread
{
        Q_OBJECT

    public:
        explicit VGMPlayer(int spi, QObject *parent = nullptr);

        void setVGM(const QByteArray& vgm, const bool loop);

        void stop();
        void pause();

        uint32_t time();

    public slots:
        void start(Priority p = InheritPriority);

    private:
        static constexpr int SPI_DELAY = 30;

        enum {
            IDLE,
            REPORT_SPACE,
            RECEIVE_DATA,
            REPORT_TIME
        } Command;

        int _spi;

        QByteArray _vgm;
        uint32_t _time;
        uint32_t _position;

        QMutex _stopLock;
        QMutex _timeLock;
        bool _stop;

        bool _loop;

        void spi_write(char val);
        void spi_xfer(char* tx, char* rx);

        // QThread interface
    protected:
        void run();
};

#endif // VGMPLAYER_H
