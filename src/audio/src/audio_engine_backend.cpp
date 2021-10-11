#include "audio_engine_backend.hpp"

#include <vector>

#include "audio_source_backend.hpp"
namespace audio {
class AudioEngine::Backend::Impl {
  private:
    std::vector<std::unique_ptr<AudioSource>> fSources;
    std::vector<const AudioSource::Backend*> fSourcesToRemove;
    AudioEngine::Backend* super;

  public:
    Impl(AudioEngine::Backend* super) : super(super) {}

    void add(std::unique_ptr<AudioSource>&& source) {
        fSources.push_back(std::move(source));
    }

    void internalUpdate() { super->internalUpdate(); }

    void update() {
        // @todo We could use the delta-time or a speed factor as a parameter.

        for (const auto& source : fSources) {
            if (source->backend().playing()) { source->backend().update(); }
        }

        internalUpdate();

        // // Automatically remove sources that are marked to be removed.
        for (const auto* sourceToRemove : fSourcesToRemove) {
            for (auto iSource = fSources.begin(); iSource != fSources.end(); ++iSource) {
                if (&(*iSource)->backend() == sourceToRemove) {
                    fSources.erase(iSource);
                    break;
                }
            }
        }

        fSourcesToRemove.clear();
    }
};

AudioEngine::Backend::Backend() : fpImpl(std::make_unique<Impl>(this)) {}
$pimpl_class_delete_namespace(AudioEngine, Backend);

void AudioEngine::Backend::add(std::unique_ptr<AudioSource>&& source) {
    fpImpl->add(std::move(source));
}
void AudioEngine::Backend::update() { fpImpl->update(); }

}   // namespace audio
