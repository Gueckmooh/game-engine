#include "audio_source_backend.hpp"

#include <iostream>

#include "audio_engine_backend.hpp"

namespace audio {

class AudioSource::Backend::Impl {
  private:
    AudioSource::Backend* super;

    AudioEngine::Backend& fEngine;

    bool fPlaying        = false;
    bool fLooping        = false;
    bool fRemoveOnFinish = false;

  public:
    Impl(AudioSource::Backend* super, AudioEngine::Backend& engine)
        : super(super), fEngine(engine) {}

    void playing(bool playing) { fPlaying = playing; }
    void looping(bool looping) { fLooping = looping; }
    void removeOnFinish(bool removeOnFinish) { fRemoveOnFinish = removeOnFinish; }

    bool looping() const { return fLooping; }
    bool playing() const { return fPlaying; }
    bool removeOnFinish() const { return fRemoveOnFinish; }

    void restart() { super->restart(); }

    void finish() {
        fPlaying = false;

        if (fRemoveOnFinish) {
            // @todo remove
            // fEngine.rem
        } else if (fLooping) {
            fPlaying = true;
            // @todo restart
            restart();
        }
    }
};

// $pimpl_class_namespace(AudioSource, Backend, AudioEngine::Backend&, engine);
AudioSource::Backend::Backend(AudioEngine::Backend& engine)
    : fpImpl(std::make_unique<Impl>(this, engine)) {}
$pimpl_class_delete_namespace(AudioSource, Backend);

$pimpl_method(AudioSource::Backend, void, playing, bool, playing);
$pimpl_method(AudioSource::Backend, void, looping, bool, looping);
$pimpl_method(AudioSource::Backend, void, removeOnFinish, bool, removeOnFinish);
$pimpl_method_const(AudioSource::Backend, bool, looping);
$pimpl_method_const(AudioSource::Backend, bool, playing);
$pimpl_method_const(AudioSource::Backend, bool, removeOnFinish);
$pimpl_method(AudioSource::Backend, void, finish);

}   // namespace audio
