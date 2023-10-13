#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include <QThread>
#include <QMutex>
#include <pigpio.h>

class VGMPlayer : public QThread
{
        Q_OBJECT

    public:
        enum Mode {
            Playback,
            Interactive
        };

        explicit VGMPlayer(int spi, QObject *parent = nullptr);

        void setVGM(const QByteArray& vgm, const bool loop);
        void setVGM(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData);
        void setMode(const Mode mode);

        bool isPlaying() const;

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
            REPORT_TIME,
            PAUSE_RESUME,
            STOP_START,
            SET_LOOP_TIME
        } Command;

        Mode _mode;

        int _spi;

        QByteArray _vgm;
        uint32_t _time;
        uint32_t _position;

        QMutex _stopLock;
        QMutex _timeLock;
        QMutex _vgmLock;
        bool _stop;
        bool _paused;

        int _loopOffsetSamples;
        int _loopOffsetData;

        void spi_write(char val);
        void spi_xfer(char* tx, char* rx);

        void runInteractive();
        void runPlayback();

        // QThread interface
    protected:
        void run();
};

#endif // VGMPLAYER_H
