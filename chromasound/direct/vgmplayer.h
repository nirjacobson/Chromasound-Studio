#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>
#include <QSettings>

#include "Chromasound_Studio.h"


class VGMPlayer : public QThread
{
        Q_OBJECT

    public:
        enum Mode {
            Playback,
            Interactive
        };

        explicit VGMPlayer(QObject *parent = nullptr);
        ~VGMPlayer();

        void setVGM(const QByteArray& vgm, const int currentOffsetData);
        void addVGM(const QByteArray& vgm);
        void setMode(const Mode mode);

        void uploadPCM(const QByteArray& pcm);

        bool isPlaying() const;
        bool isPaused() const;

        void stop();
        void pause();

        uint32_t length() const;
        uint32_t introLength() const;
        uint32_t loopLength() const;

        uint32_t time();
        void setTime(const uint32_t time);

        void fillWithPCM(const bool enable);
        void discretePCM(const bool enable);

        void setProfile(const Chromasound_Studio::Profile& profile);

    signals:
        void pcmUploadStarted();
        void pcmUploadFinished();

    private:
        static int SPI_DELAY;

        enum {
            IDLE,
            REPORT_SPACE,
            RECEIVE_DATA,
            REPORT_TIME,
            PAUSE_RESUME,
            STOP,
            RESET,
            FILL_WITH_PCM,
            STOP_FILL_WITH_PCM,
            DISCRETE_PCM,
            INTEGR8D_PCM
        } Command;

        Mode _mode;

        int _spiFd;

        QByteArray _vgm;
        QByteArray _pcmBlock;
        volatile uint32_t _time;
        uint32_t _position;

        uint32_t _length;
        uint32_t _introLength;
        uint32_t _loopLength;

        QMutex _stopLock;
        QMutex _timeLock;
        QMutex _vgmLock;
        volatile bool _stop;
        volatile bool _paused;

        int _loopOffsetSamples;
        int _loopOffsetData;

        QElapsedTimer _timer;
        bool _playing;

        QElapsedTimer _spiTimer;

        bool _fillWithPCM;
        bool _discretePCM;

        Chromasound_Studio::Profile _profile;

        quint32 fletcher32(const QByteArray& data);
        quint32 _lastPCMBlockChecksum;

        void spi_write_wait(uint8_t val);
        void spi_xfer_wait(uint8_t* tx, uint8_t* rx);

        void runInteractive();
        void runPlayback();


        void uploadPCMInner(const QByteArray& pcm);

        // QThread interface
    protected:
        void run();
};

#endif // VGMPLAYER_H
