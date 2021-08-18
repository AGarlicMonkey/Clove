#include "Clove/SoundFile.hpp"

#include <Clove/Application.hpp>
#include <Clove/Audio/AhaBuffer.hpp>
#include <Clove/Audio/AhaDevice.hpp>
#include <Clove/Audio/AhaFactory.hpp>
#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <sndfile.h>

namespace clove {
    namespace {
        int getWhence(SoundFile::SeekPosition position) {
            switch(position) {
                case SoundFile::SeekPosition::Beginning:
                    return SEEK_SET;
                case SoundFile::SeekPosition::Current:
                    return SEEK_CUR;
                case SoundFile::SeekPosition::End:
                    return SEEK_END;
                default:
                    CLOVE_ASSERT(false, "{0}: Default statement hit", CLOVE_FUNCTION_NAME);
                    return 0;
            }
        }

        AhaBuffer::Format getBufferFormat(SoundFile::Format format, uint32_t channels) {
            bool const is16{ format == SoundFile::Format::S16 };

            if(channels == 1) {
                return is16 ? AhaBuffer::Format::Mono16 : AhaBuffer::Format::Mono8;
            } else {
                return is16 ? AhaBuffer::Format::Stereo16 : AhaBuffer::Format::Stereo8;
            }
        }
    }

    struct SoundFile::FileData {
        SNDFILE *file;
        SF_INFO fileInfo;
    };

    SoundFile::SoundFile(std::string_view file) {
        data       = std::make_unique<FileData>();
        data->file = sf_open(file.data(), SFM_READ, &data->fileInfo);
    }

    SoundFile::SoundFile(SoundFile &&other) noexcept = default;

    SoundFile &SoundFile::operator=(SoundFile &&other) noexcept = default;

    SoundFile::~SoundFile() {
        sf_close(data->file);
    }

    uint32_t SoundFile::getChannels() const {
        return data->fileInfo.channels;
    }

    uint32_t SoundFile::getFrames() const {
        return data->fileInfo.frames;
    }

    uint32_t SoundFile::getSampleRate() const {
        return data->fileInfo.samplerate;
    }

    SoundFile::Format SoundFile::getFormat() const {
        if((data->fileInfo.format & SF_FORMAT_PCM_S8) != 0) {
            return Format::S8;
        } else if((data->fileInfo.format & SF_FORMAT_PCM_16) != 0) {
            return Format::S16;
        } else if((data->fileInfo.format & SF_FORMAT_PCM_24) != 0) {
            return Format::S24;
        } else if((data->fileInfo.format & SF_FORMAT_PCM_32) != 0) {
            return Format::S32;
        }

        CLOVE_ASSERT(false, "{0}, Unknown file format", CLOVE_FUNCTION_NAME_PRETTY);
        return Format::Unknown;
    }

    std::unique_ptr<AhaBuffer> SoundFile::read(uint32_t const frames) {
        auto audioBuffer  = *Application::get().getAudioDevice()->getAudioFactory()->createAudioBuffer(AhaBuffer::Descriptor{
            .format     = getBufferFormat(getFormat(), getChannels()),
            .sampleRate = getSampleRate(),
        });
        auto [data, size] = readRaw(frames);

        audioBuffer->write(data.get(), size);

        return audioBuffer;
    }

    std::pair<std::unique_ptr<short>, size_t> SoundFile::readRaw(uint32_t const frames) {
        size_t const elementCount{ frames * getChannels() };
        auto buffer{ std::unique_ptr<short>(new short[elementCount]) };

        sf_readf_short(data->file, buffer.get(), frames);

        size_t const bufferSize{ elementCount * sizeof(short) };
        return { std::move(buffer), bufferSize };
    }

    void SoundFile::seek(SeekPosition position, uint32_t frames) {
        sf_seek(data->file, frames, getWhence(position));
    }
}
