/*  Audio Device Info
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_AudioPipeline_AudioInfo_H
#define PokemonAutomation_AudioPipeline_AudioInfo_H

#include <string>
#include <vector>
#include <QtConfig>
#include "Common/Cpp/Pimpl.h"

class QString;
class QAudioFormat;

#if QT_VERSION_MAJOR == 5
class QAudioDeviceInfo;
using NativeAudioInfo = QAudioDeviceInfo;
#elif QT_VERSION_MAJOR == 6
class QAudioDevice;
using NativeAudioInfo = QAudioDevice;
#else
#error "Unknown Qt version."
#endif



namespace PokemonAutomation{



enum class AudioFormat{
    NONE,
    MONO_48000,
    DUAL_44100,
    DUAL_48000,
    MONO_96000,
    INTERLEAVE_LR_96000,
    INTERLEAVE_RL_96000,
    END_LIST,
};
extern const char* AUDIO_FORMAT_LABELS[];

//  Set the QAudioFormat to the our audio format enum.
void set_format(QAudioFormat& native_format, AudioFormat format);






class AudioDeviceInfo{
public:
    ~AudioDeviceInfo();
    AudioDeviceInfo(const AudioDeviceInfo& x);
    void operator=(const AudioDeviceInfo& x);

public:
    AudioDeviceInfo();
    AudioDeviceInfo(const std::string& device_name);

    operator bool() const;

    const QString& display_name() const;
    const std::string& device_name() const;

    const std::vector<AudioFormat>& supported_formats() const;
    int preferred_format_index() const;
    QAudioFormat preferred_format() const;

    const NativeAudioInfo& native_info() const;

    bool operator==(const AudioDeviceInfo& info);

    static std::vector<AudioDeviceInfo> all_input_devices();
    static std::vector<AudioDeviceInfo> all_output_devices();

private:
    struct Data;
    Pimpl<Data> m_body;
};





}
#endif
